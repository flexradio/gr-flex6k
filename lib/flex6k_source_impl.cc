/* -*- c++ -*- */
/*
 * Copyright 2021 SWRI.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>

#include <utility>

#include "flex6k_source_impl.h"

namespace gr
{
    namespace Flex6000
    {

        flex6k_source::sptr
        flex6k_source::make(std::string waveform_long_name,
                                     std::string waveform_short_name,
                                     std::string address,
                                     bool swapIQ,
                                     bool sendZerosWhileTX)
        {
            return gnuradio::get_initial_sptr(new flex6k_source_impl(waveform_long_name,
                                                                              waveform_short_name,
                                                                              address,
                                                                              swapIQ,
                                                                              sendZerosWhileTX));
        }

        static const pmt::pmt_t TIME_KEY = pmt::string_to_symbol("rx_time");
        static const pmt::pmt_t RATE_KEY = pmt::string_to_symbol("rx_rate");
        static const pmt::pmt_t FREQ_KEY = pmt::string_to_symbol("rx_freq");

        /*
         * The private constructor
         */
        flex6k_source_impl::flex6k_source_impl(std::string waveform_long_name,
                                                                 std::string waveform_short_name,
                                                                 std::string address,
                                                                 bool swapIQ,
                                                                 bool sendZerosWhileTX)
            : gr::block("flex6k_source",
                        gr::io_signature::make(0, 0, 0),
                        gr::io_signature::make(1, 1, sizeof(std::complex<float>))),
              m_waveform_long_name(waveform_long_name),
              m_waveform_short_name(waveform_short_name),
              m_address(address),
              m_swapIQ(swapIQ)
        {
            m_radio = FlexRadio6000::Connect(address, waveform_long_name, waveform_short_name, "RAW");

            m_radio->enforceMaxInstances("flexIQRx", 10);

            m_radio->m_lockMap.get_lock("initLock")->stateLckCntRegister();
            m_radio->m_lockMap.get_lock("startedLock")->stateLckCntRegister();

            //activation requires waiting for status updates to be recieved, then updating configs.
            m_radio->m_lockMap.get_lock("activationDelayLock")->stateLckCntRegister();
            m_radio->m_lockMap.get_lock("activationLock")->stateLckCntRegister();

            m_radio->register_state_cb(s_stateChangeCallback, this);
            // //register tx_pacing callback:
            m_radio->register_pacingRX_cb(s_pacingCallback, this);

            m_intern_writer = gr::make_buffer(m_intern_nitems, sizeof(gr_complex));
            m_intern_reader = gr::buffer_add_reader(m_intern_writer, 0);

            GR_LOG_INFO(d_logger, "Constructed FlexRadio6000 Source Interface Block and Registered Callbacks");
        }

        /*
         * Our virtual destructor.
         */
        flex6k_source_impl::~flex6k_source_impl()
        {
        }

        bool flex6k_source_impl::start()
        {
            //   m_finished = false;

            m_radio->m_lockMap.get_lock("initLock")->waitOnStateLck();
            m_sampRate = m_radio->getSampRate();

            m_radio->m_lockMap.get_lock("startedLock")->waitOnStateLck();
            m_finished = false;

            GR_LOG_INFO(d_logger, "FlexRadio6000 Source Interface Block Started");
            return true;
        }

        bool flex6k_source_impl::stop()
        {
            GR_LOG_INFO(d_logger, "FlexRadio6000 Source Interface Block Stopping");
            m_radio->deregister_pacingRX_cb(s_pacingCallback, this);
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); //sleep to ensure any active callbacks complete.
            GR_LOG_INFO(d_logger, "FlexRadio6000 Source Interface Block Stopped");
            return true;
        }

        void flex6k_source_impl::stateChangeCallback(FlexRadio6000::waveform_state state)
        {
            std::lock_guard<std::mutex> lock(m_stateMutex);
            m_state.m_state_prev = m_state.m_state_curr;
            m_state.m_state_curr = state;

            switch (m_state.m_state_curr)
            {
            case FlexRadio6000::waveform_state::ACTIVE:

                std::cout << "source got ACTIVE cb" << std::endl;
                m_radio->m_lockMap.get_lock("activationDelayLock")->waitOnStateLck();
                m_radio->get_wvfrm_sliceNum(m_sliceNum);
                m_radio->m_lockMap.get_lock("activationLock")->waitOnStateLck();
                m_state.m_state_active = true;
                set_resendTags(true);
                break;
            case FlexRadio6000::waveform_state::INACTIVE:
                m_state.m_state_active = false;
                m_sliceNum = "";
                break;
            case FlexRadio6000::waveform_state::PTT_REQUESTED:
                break;
            case FlexRadio6000::waveform_state::UNKEY_REQUESTED:
                if (!m_sendZerosInTx)
                {
                    set_resendTags(true);
                }

                break;
            default:

                break;
            }

            m_state.m_state_changed = true;
        }

        int
        flex6k_source_impl::general_work(int noutput_items,
                                                  gr_vector_int &ninput_items,
                                                  gr_vector_const_void_star &input_items,
                                                  gr_vector_void_star &output_items)
        {
            //wait for data from radio using condition variable:
            waitForData();
            int itemsAvail = m_intern_reader->items_available();
            int itemsToPush = std::min(noutput_items, itemsAvail);
            if (itemsToPush > 0)
            {
                std::complex<float> *out = (std::complex<float> *)output_items[0];
                //copy data
                if (m_swapIQ)
                {
                    std::complex<float> *input = (std::complex<float> *)m_intern_reader->read_pointer();
                    for (int i = 0; i < itemsToPush; i++)
                    {
                        out[i].real(input[i].imag());
                        out[i].imag(input[i].real());
                    }
                }
                else
                {
                    std::copy((std::complex<float> *)m_intern_reader->read_pointer(), (std::complex<float> *)m_intern_reader->read_pointer() + itemsToPush, out);
                }

                //copy tags:
                std::vector<tag_t> tags;
                m_intern_reader->get_tags_in_range(tags, m_intern_reader->nitems_read(), m_intern_reader->nitems_read() + itemsToPush, 0);
                for (auto tag : tags)
                {
                    //copy directly to output -> offsets are the same.
                    //change the source to be from this block
                    tag.srcid = pmt::string_to_symbol(alias());
                    add_item_tag(0, tag);
                }
            }
            m_intern_reader->update_read_pointer(itemsToPush);
            return itemsToPush;
        }

        void flex6k_source_impl::waitForData()
        {
            std::unique_lock<std::mutex> lock(m_bufLock);

            if (m_intern_reader->items_available() == 0)
            {
                m_condVar.wait_for(lock, std::chrono::milliseconds(100), [&]
                                   { return m_intern_reader->items_available() > 0; });
            }
        }

        // void flex6k_source_impl::waitForPacingCB()
        // {
        //     std::unique_lock<std::mutex> lock(m_pacingMutex);

        //     if (m_newData == false)
        //     {
        //         m_pacingCondVar.wait(lock, [&]
        //                              { return m_newData || m_finished; });
        //     }
        // }

        void flex6k_source_impl::pacingCallback(std::chrono::nanoseconds ts,
                                                FlexRadio6000::paceMode mode, int numSamples,
                                                float *packet)
        {

            // while (!m_finished)
            {
                // waitForPacingCB();
                // if (m_finished)
                // {
                //     return; //done exit early...
                // }
                // m_newData = false;

                // {
                //     std::lock_guard<std::mutex> lock(m_pacingMutex);
                //     std::swap(m_pacingData_process, m_pacingData_ready);
                // }

                //numSampls is a float we measure in complex floats...
                if (mode == FlexRadio6000::paceMode::RX)
                {

                    int itemsAvail = numSamples / 2;
                    int nOutputSize = m_intern_writer->space_available();
                    int itemsToPush = std::min(itemsAvail, nOutputSize);

                    std::copy((std::complex<float> *)packet, (std::complex<float> *)packet + itemsToPush, (std::complex<float> *)m_intern_writer->write_pointer());

                    if (get_resendTags())
                    {
                        //convert ns to sec and buf:
                        auto ts_s = std::chrono::duration_cast<std::chrono::seconds>(ts);
                        m_tsEpoch = ts;
                        auto ts_ns = ts - std::chrono::duration_cast<std::chrono::nanoseconds>(ts_s);
                        double ts_ns_double = static_cast<double>(ts_ns.count()) / static_cast<double>(std::nano::den);
                        // convert timestamp to usrp style
                        pmt::pmt_t rx_time = pmt::make_tuple(pmt::from_uint64(ts_s.count()), pmt::from_double(ts_ns_double));
                        // push tag to buffer:
                        tag_t time_tag;
                        time_tag.key = TIME_KEY;
                        time_tag.value = rx_time;
                        time_tag.offset = m_intern_writer->nitems_written();

                        m_intern_writer->add_item_tag(time_tag);

                        tag_t rate_tag;
                        rate_tag.key = RATE_KEY;
                        rate_tag.value = pmt::from_double(m_sampRate);
                        rate_tag.offset = m_intern_writer->nitems_written();

                        m_intern_writer->add_item_tag(rate_tag);

                        tag_t freq_tag;
                        freq_tag.key = FREQ_KEY;
                        freq_tag.value = pmt::from_double(m_freq);
                        freq_tag.offset = m_intern_writer->nitems_written();

                        m_intern_writer->add_item_tag(freq_tag);
                        set_resendTags(false);
                    }

                    m_intern_writer->update_write_pointer(itemsToPush);
                    m_condVar.notify_all();
                }
                else if (mode == FlexRadio6000::paceMode::TX)
                {

                    if (m_sendZerosInTx)
                    {
                        int itemsAvail = 0;
                        auto pc_tp = std::chrono::system_clock::now();
                        std::chrono::nanoseconds ts = pc_tp.time_since_epoch();
                        uint64_t ns_since_epoch = ts.count() - m_tsEpoch.count();
                        double sampDur = ns_since_epoch * m_sampRate / std::nano::den;
                        int samps = round(sampDur);
                        itemsAvail = samps - m_intern_writer->nitems_written() + numSamples / 2;
                        if (m_modeLast == FlexRadio6000::paceMode::RX)
                        {
                            std::cout << itemsAvail << "extra samps for first transmit, " << m_intern_writer->space_available() << " items availiable" << std::endl;

                            tag_t info_tag;
                            info_tag.key = pmt::mp("tx_mode_null_data");
                            info_tag.value = pmt::mp("tx_mode_null_data");
                            info_tag.offset = m_intern_writer->nitems_written();
                            m_intern_writer->add_item_tag(info_tag);
                        }
                        else
                        {
                            // itemsAvail = numSamples / 2;
                        }

                        // itemsAvail = itemsAvail - (extraSamps-itemsAvail);
                        int nOutputSize = m_intern_writer->space_available() - 10;
                        int itemsToPush = std::min(itemsAvail, nOutputSize);
                        itemsToPush = std::max(0, itemsToPush);
                        std::vector<std::complex<float>> blank_items(itemsToPush, std::complex<float>(0.0001, 0.0001)); //if you send zeros he correlation can go to infinity... = bad
                        std::copy(blank_items.begin(), blank_items.begin() + itemsToPush, (std::complex<float> *)m_intern_writer->write_pointer());
                        m_intern_writer->update_write_pointer(itemsToPush);
                        m_condVar.notify_all();
                        // std::cout <<"wrote "<<itemsToPush<<std::endl;
                    }
                }
                m_modeLast = mode;
            }
        }

    } /* namespace Flex6000 */
} /* namespace gr */
