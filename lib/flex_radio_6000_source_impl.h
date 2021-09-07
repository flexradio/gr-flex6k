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

#ifndef INCLUDED_FLEX6000_FLEX_RADIO_6000_SOURCE_IMPL_H
#define INCLUDED_FLEX6000_FLEX_RADIO_6000_SOURCE_IMPL_H

#include <Flex6000/flex_radio_6000_source.h>
#include "FlexRadio6000.h"

#include <chrono>
#include <gnuradio/buffer.h>

#include <mutex>
#include <condition_variable>

#include "flex_data_structures.h"

namespace gr
{
    namespace Flex6000
    {

        class flex_radio_6000_source_impl : public flex_radio_6000_source //, FlexRadio6000::Waveform::Listener
        {
        public:
            flex_radio_6000_source_impl(std::string waveform_long_name,
                                        std::string waveform_short_name,
                                        std::string address,
                                        bool swapIQ,
                                        bool sendZerosWhileTX);
            ~flex_radio_6000_source_impl();

            // void next_vita49_packet(waveform_vita_packet* packet, size_t packet_size) override;

            bool start();
            bool stop();
            // Where all the action really happens

            static inline void s_stateChangeCallback(FlexRadio6000::waveform_t *waveform,
                                                     FlexRadio6000::waveform_state state, void *arg)
            {
                auto ctx = (flex_radio_6000_source_impl *)arg;
                ctx->stateChangeCallback(state);
            }
            void stateChangeCallback(FlexRadio6000::waveform_state state);

            static inline void s_pacingCallback(std::chrono::nanoseconds ts,
                                                FlexRadio6000::paceMode mode, int numSamples,
                                                float *packet, void *arg)
            {
                auto ctx = (flex_radio_6000_source_impl *)arg;
                ctx->pacingCallback(ts,mode,numSamples,packet);
                // ctx->m_pacingData_callback.mode = mode;
                // ctx->m_pacingData_callback.ts = ts;
                // ctx->m_pacingData_callback.numSamples = numSamples;

                // if (mode == FlexRadio6000::paceMode::RX)
                // {
                //     ctx->m_pacingData_callback.data.resize(numSamples);
                //     std::copy(packet, packet + numSamples, ctx->m_pacingData_callback.data.begin());
                // }

                // {
                //     std::lock_guard<std::mutex> lock(ctx->m_pacingMutex);
                //     std::swap(ctx->m_pacingData_callback, ctx->m_pacingData_ready);
                //     ctx->m_newData = true;
                // }
                // ctx->m_pacingCondVar.notify_all();
            }
            // void pacingCallback();
            void pacingCallback(std::chrono::nanoseconds ts,
                                                FlexRadio6000::paceMode mode, int numSamples,
                                                float *packet);

            int general_work(int noutput_items,
                             gr_vector_int &ninput_items,
                             gr_vector_const_void_star &input_items,
                             gr_vector_void_star &output_items);

        private:
            std::shared_ptr<FlexRadio6000> m_radio;

            std::string m_waveform_long_name;
            std::string m_waveform_short_name;
            std::string m_address;
            bool m_swapIQ;

            bool m_sendZerosInTx = true;
            FlexRadio6000::paceMode m_modeLast = FlexRadio6000::paceMode::RX;
            std::chrono::nanoseconds m_tsEpoch;
            double m_sampRate = 0;
            double m_freq = 0;

            std::mutex m_resendTags_Mutex;
            bool m_resendTags = true;
            bool get_resendTags()
            {
                std::lock_guard<std::mutex> lock(m_resendTags_Mutex);
                return m_resendTags;
            }
            void set_resendTags(bool resendTags)
            {
                std::lock_guard<std::mutex> lock(m_resendTags_Mutex);
                m_resendTags = resendTags;
            }

            gr::buffer_sptr m_intern_writer;
            gr::buffer_reader_sptr m_intern_reader;
            size_t m_intern_nitems = 8192 * 32;

            std::mutex m_stateMutex;
            flex::radioState m_state;
            std::string m_sliceNum;

            //condition var stuff:
            std::condition_variable m_condVar;
            std::mutex m_bufLock;
            void waitForData();

            // struct pacingData
            // {
            //     FlexRadio6000::paceMode mode;
            //     std::chrono::nanoseconds ts;
            //     int numSamples;
            //     std::vector<float> data;
            // };
            // bool m_newData = false;
            //three states to pass data: swap between
            //callback->ready->process->return->callback
            // pacingData m_pacingData_callback;
            // pacingData m_pacingData_ready;
            // pacingData m_pacingData_process;

            // std::mutex m_pacingMutex;
            // std::condition_variable m_pacingCondVar;
            // std::thread *m_pacing_thread;

            void waitForPacingCB();
            bool m_finished;
        };

    } // namespace Flex6000
} // namespace gr

#endif /* INCLUDED_FLEX6000_FLEX_RADIO_6000_SOURCE_IMPL_H */
