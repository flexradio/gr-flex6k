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

#include "flex_radio_6000_sink_impl.h"
#include <atomic>
#include <chrono>
#include <boost/lexical_cast.hpp>

#define tx_freq

namespace gr
{
  namespace Flex6000
  {

    flex_radio_6000_sink::sptr
    flex_radio_6000_sink::make(std::string waveform_long_name,
                               std::string waveform_short_name,
                               std::string address,
                               int txMode)
    {
      return gnuradio::get_initial_sptr(new flex_radio_6000_sink_impl(waveform_long_name,
                                                                      waveform_short_name,
                                                                      address,
                                                                      txMode));
    }

    /*
     * The private constructor
     */
    flex_radio_6000_sink_impl::flex_radio_6000_sink_impl(std::string waveform_long_name,
                                                         std::string waveform_short_name,
                                                         std::string address,
                                                         int txMode)
        : gr::block("flex_radio_6000_sink",
                    gr::io_signature::make(1, 1, sizeof(std::complex<float>)),
                    gr::io_signature::make(0, 0, 0)),
          m_waveform_long_name(waveform_long_name),
          m_waveform_short_name(waveform_short_name),
          m_address(address),
          m_txMode(static_cast<txMode_t>(txMode))
    {
      m_radio = FlexRadio6000::Connect(address, waveform_long_name, waveform_short_name, "RAW");
      //enforce highlander rules, only one instance of this block connected to a individual waveform allowed.
      m_radio->enforceMaxInstances("flexIQTx", 1);
      // add myself to state barrier(s):
      m_radio->m_lockMap.get_lock("initLock")->stateLckCntRegister();
      m_radio->m_lockMap.get_lock("startedLock")->stateLckCntRegister();

      //activation requires waiting for status updates to be recieved, then updating configs.
      m_radio->m_lockMap.get_lock("activationDelayLock")->stateLckCntRegister();
      m_radio->m_lockMap.get_lock("activationLock")->stateLckCntRegister();
      //register for state change callback
      m_radio->register_state_cb(s_stateChangeCallback, this);
      //register tx_pacing callback:
      m_radio->register_pacingTX_cb(s_pacingCallback, this);

      //create io buffers to transfer between cb and gnuradio:
      m_intern_writer = gr::make_buffer(m_intern_nitems, sizeof(gr_complex));
      m_intern_reader = gr::buffer_add_reader(m_intern_writer, 0);

      GR_LOG_INFO(d_logger, "Constructed FlexRadio6000 Sink Interface Block and Registered Callbacks");

      //spawn a thread to decide whether to activate tx.
      m_txCtrl_thread = new std::thread(
          [this]()
          {
            tx_monitor();
          });
    }

    /*
     * Our virtual destructor.
     */
    flex_radio_6000_sink_impl::~flex_radio_6000_sink_impl()
    {
      //not sure what is needed to do... maybe nothing...
    }

    int
    flex_radio_6000_sink_impl::general_work(int noutput_items,
                                            gr_vector_int &ninput_items,
                                            gr_vector_const_void_star &input_items,
                                            gr_vector_void_star &output_items)
    {
      const std::complex<float> *in = (const std::complex<float> *)input_items[0];

      waitForBufferSpace(); // will wake on timer or when there is space...
      int space_avail = m_intern_writer->space_available();
      if (space_avail > 0)
      { //check if we have space, or if it was a timer wakeup...
        //we can write:
        uint64_t minItemsInBuf = 4096;

        int itemsToCopy = std::min(space_avail, noutput_items);

        //add items to buffer:
        std::vector<tag_t> tags;
        get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + itemsToCopy);
        for (auto tag : tags)
        {
          //look for sob:
          if (pmt::eqv(tag.key, pmt::mp("sob")))
          {
            // burst active, wait for min samples or eob before starting tx.
            m_sobFound = true;
            m_sobIndex = tag.offset;
          }

          if (pmt::eqv(tag.key, pmt::mp("eob")))
          {
            // burst active, wait for min samples or eob before starting tx.
            m_eobFound = true;
            m_eobIndex = tag.offset;
            break;
          }

          //offset location is the same, you will never drop samples outside of internal buffer;
          m_intern_writer->add_item_tag(tag);
        }

        if (m_eobFound)
        {
          itemsToCopy = m_eobIndex - nitems_read(0) + 1;
        }

        std::copy(in, in + itemsToCopy, (std::complex<float> *)m_intern_writer->write_pointer());
        m_intern_writer->update_write_pointer(itemsToCopy);

        // either have more than min items in buffer or have found both sob and eob
        if (m_sobFound && (m_intern_writer->nitems_written() - m_sobIndex + itemsToCopy > minItemsInBuf || m_eobFound))
        {
          std::cout << "starting tx with " << m_intern_writer->bufsize() - m_intern_writer->space_available() << " items in buffer" << std::endl;
          m_radio->send_api_command(NULL, NULL, "xmit 1");
          m_sobFound = false;
        }

        m_eobFound = false;

        consume(0, itemsToCopy);
        // std::cout<<"copied " << itemsToCopy <<" items to the buffer" <<std::endl;
        return 0;
      }

      return 0;
    }

    void flex_radio_6000_sink_impl::tx_monitor()
    {
      while (true && m_finished == false)
      {
        waitForDataState();
        //state changed;
        {
          std::lock_guard<std::mutex> lock(m_ctrlMutex);
          m_vReqCmd_LOCAL = m_vReqCmd;
          m_vReqCmd.clear();
          m_newState = false;
        }

        for (auto cmd : m_vReqCmd_LOCAL)
        {
          if (cmd == reqCommand::TX_OFF)
          {
            m_radio->send_api_command(NULL, NULL, "xmit 0");
          }
        }
        m_vReqCmd_LOCAL.clear();
      }
    }

    void flex_radio_6000_sink_impl::waitForDataState()
    {
      std::unique_lock<std::mutex> lock(m_ctrlMutex);

      if (m_newState == false)
      {
        m_ctrlCondVar.wait_for(lock, std::chrono::milliseconds(100), [&]
                               { return m_newState != false || m_finished; });
      }
    }

    bool flex_radio_6000_sink_impl::start()
    {
      m_finished = false;
      m_radio->m_lockMap.get_lock("initLock")->waitOnStateLck();
      m_radio->m_lockMap.get_lock("startedLock")->waitOnStateLck();

      GR_LOG_INFO(d_logger, "FlexRadio6000 Sink Interface Block Started");

      return true;
    }

    bool flex_radio_6000_sink_impl::stop()
    {
      GR_LOG_INFO(d_logger, "FlexRadio6000 Sink Interface Block stopping");
      m_finished = true;
      m_condVar.notify_all();
      {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        if (m_state.m_state_curr == FlexRadio6000::waveform_state::PTT_REQUESTED && m_state.m_state_active == true)
        //need to check if I am active and transmitting.. turn off transmitter.
        {
          m_radio->send_api_command(NULL, NULL, "xmit 0");
        }
      }
      m_radio->deregister_pacingTX_cb(s_pacingCallback, this);
      m_txCtrl_thread->join();                                     //join the tx monitor thread.
      std::this_thread::sleep_for(std::chrono::milliseconds(100)); //sleep to ensure any active callbacks complete.
      GR_LOG_INFO(d_logger, "FlexRadio6000 Sink Interface Block stopped");
      return true;
    }

    void flex_radio_6000_sink_impl::stateChangeCallback(FlexRadio6000::waveform_state state)
    {
      std::lock_guard<std::mutex> lock(m_stateMutex);
      m_state.m_state_prev = m_state.m_state_prev;
      m_state.m_state_curr = state;

      if (m_state.m_state_curr == FlexRadio6000::waveform_state::ACTIVE)
      {
        std::cout << "sink got ACTIVE cb" << std::endl;
        m_radio->m_lockMap.get_lock("activationDelayLock")->waitOnStateLck();
        m_radio->get_wvfrm_sliceNum(m_sliceNum);
        m_radio->m_lockMap.get_lock("activationLock")->waitOnStateLck();
        m_state.m_state_active = true;
      }

      if (m_state.m_state_curr == FlexRadio6000::waveform_state::INACTIVE)
      {
        //deactivated set slicenum to zero:
        m_state.m_state_active = false;
        m_sliceNum = "";
      }

      m_state.m_state_changed = true;
    }

    // std::vector<tag_t> tags;
    // m_intern_reader->get_tags_in_range(tags, m_intern_reader->nitems_read(), m_intern_reader->nitems_read() + sampToPush, 0);
    // for (auto tag : tags)
    // {
    //   if (pmt::eqv(tag.key, pmt::mp("tx_freq")))
    //   {
    //     std::cout << "found tx freq tag " << std::endl;
    //     //later these should be timed commands:
    //     double freq = pmt::to_double(tag.value);
    //     // freq=40.11234567890123456781234567812345678;
    //     std::string freq_str = boost::lexical_cast<std::string>(freq);
    //     m_radio->send_api_command(NULL, NULL, "slice tune " + m_sliceNum + " " + freq_str);
    //     std::cout << "retuned to " << freq_str << "MHz" << std::endl;
    //   }
    //   if (pmt::eqv(tag.key, pmt::mp("tx_time")))
    //   {
    //     std::cout << "found tx time tag " << std::endl;
    //     //later these should be timed commands:

    //     //convert timestamp to chrono:
    //     uint64_t txepochSec = pmt::to_uint64(pmt::tuple_ref(tag.value, 0));

    //     double txepochFrac = pmt::to_double(pmt::tuple_ref(tag.value, 1));
    //     uint64_t txepochns = txepochFrac * std::nano::den;

    //     std::chrono::nanoseconds reqTxTime = std::chrono::nanoseconds(txepochns) + std::chrono::seconds(txepochSec);

    //     auto pc_tp = std::chrono::system_clock::now();
    //     auto ts_ns = pc_tp.time_since_epoch();

    //     std::cout << "tried to transmit at " << reqTxTime.count() << " (ns), actually transmitted at " << ts.count() << "\nLate by " << ts.count() - reqTxTime.count() << " ns"
    //               << "samp_avail for tx = " << itemsAvail << "\n"
    //               << "true true tx time: " << ts_ns.count() << "\n"
    //               << "ture true lateness: " << ts_ns.count() - reqTxTime.count() << std::endl;
    //   }
    // }

    void flex_radio_6000_sink_impl::pacingCallback(std::chrono::nanoseconds ts, FlexRadio6000::paceMode mode, int numSamples)
    {
      int itemsAvail = m_intern_reader->items_available();
      int numComplexSamples = (numSamples) / 2;

      //we are the only ones able to do auto-deactivation based on lack of dat:
      if (itemsAvail == 0 && mode == FlexRadio6000::paceMode::TX)
      {
        //deactivate TX, we have no samples to TX, and will send 0 until deactivated (or we get more data).
        m_countEmpty++;
        std::cout << "No data availiable in internal buffer for tx, this occured "<<m_countEmpty<<" times in a row"<< std::endl;
        if(m_countEmpty>2){
          //only send end of tx if we have no data for multiple callbacks.
          std::cout << "requesting TX_OFF "<< std::endl;
          std::lock_guard<std::mutex> lock(m_ctrlMutex);
          m_vReqCmd.push_back(reqCommand::TX_OFF);
          m_newState = true;
          m_ctrlCondVar.notify_all();
          
        }

      }

      //someone else puts us into transmit mode, they better make sure that out internal buffer is filled enough to start, else there will be gaps
      if (mode == FlexRadio6000::paceMode::TX && itemsAvail > 0)
      {
        m_countEmpty=0;
        int packetsToSend = 1;
        int packetsSent = 0;

        while (packetsSent < packetsToSend)
        {
          if (packetsSent > 0 && itemsAvail < numComplexSamples)
          {
            //send at least one packet.
            std::cout << "not enough samples, and sent 1 tx" << std::endl;
            break;
          }
          int sampToPush = std::min(itemsAvail, numComplexSamples);
          std::vector<std::complex<float>> vecData(numComplexSamples, 0);
          std::copy((std::complex<float> *)m_intern_reader->read_pointer(), (std::complex<float> *)m_intern_reader->read_pointer() + sampToPush, vecData.begin());

          m_radio->send_data_packet((float *)vecData.data(), numComplexSamples * 2, FlexRadio6000::waveform_packet_type::TRANSMITTER_DATA);

          m_intern_reader->update_read_pointer(sampToPush);
          m_condVar.notify_all();

          // std::cout << "sent " << sampToPush << " items, padded to " << numComplexSamples << " to radio "<< std::endl;
          packetsSent++;
          itemsAvail = itemsAvail - sampToPush;
        }
      }
      else if (mode == FlexRadio6000::paceMode::TX && itemsAvail == 0)
      {
        std::cout << "no samples to push" << std::endl;
      }

      m_lastMode = mode;
    }

    void flex_radio_6000_sink_impl::waitForBufferSpace()
    {
      std::unique_lock<std::mutex> lock(m_bufLock);

      if (m_intern_writer->space_available() == 0)
      {
        m_condVar.wait_for(lock, std::chrono::milliseconds(50), [&]
                           { return m_intern_writer->space_available() > 0 || m_finished; });
      }
    }
  } /* namespace Flex6000 */
} /* namespace gr */
