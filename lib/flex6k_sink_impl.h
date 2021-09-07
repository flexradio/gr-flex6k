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

#ifndef INCLUDED_FLEX6000_FLEX6K_SINK_IMPL_H
#define INCLUDED_FLEX6000_FLEX6K_SINK_IMPL_H

#include <Flex6000/flex6k_sink.h>
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

        class flex6k_sink_impl : public flex6k_sink
        {
        private:
            std::shared_ptr<FlexRadio6000> m_radio;

            std::string m_waveform_long_name;
            std::string m_waveform_short_name;
            std::string m_address;

            // int m_extraPacketsAtStart;
            // int m_minNumPackets;

            std::mutex m_stateMutex;
            flex::radioState m_state;

            int m_packetsToSend;
            int m_totalPacketsSent;
            enum txMode_t : int {
                CONT_FORCED_STOP,
                CONT_DELAY_STOP,
                CONT_AUTO_START_STOP,
                BURST_AUTO_NO_TS,
                BURST_AUTO_TS
            };

            txMode_t m_txMode;


            std::string m_sliceNum;
            
            enum paceMode_t
            {
                RX,
                TX
            };

            bool m_sobFound;
            uint64_t m_sobIndex;
            bool m_eobFound;
            uint64_t m_eobIndex;


            gr::buffer_sptr m_intern_writer;
            gr::buffer_reader_sptr m_intern_reader;
            size_t m_intern_nitems = 8192*64;

        //condition var stuff for buffer filling:
            std::condition_variable m_condVar;
            std::mutex m_bufLock;
            std::atomic<bool> m_finished;
            void waitForBufferSpace();
        
            std::chrono::nanoseconds m_next;
            std::chrono::nanoseconds m_currTime;

            FlexRadio6000::paceMode m_lastMode;

        // tx activation/deactivation stuff:
            std::condition_variable m_ctrlCondVar;
            std::mutex m_ctrlMutex;
            bool m_newState=false;

            //these are simple commands that can only be executed from the callback.
            //mainly tuning off the radio when all data has been processed.
            enum reqCommand{
                TX_OFF,
            };
            std::deque<reqCommand> m_vReqCmd;
            std::deque<reqCommand> m_vReqCmd_LOCAL;
            std::thread * m_txCtrl_thread;
            void tx_monitor();
            void waitForDataState();

            int m_countEmpty;

        // thread that processes data:
            // struct pacingData{
            //     FlexRadio6000::paceMode mode;
            //     std::chrono::nanoseconds ts;
            //     int numSamples;
            // };
            // bool m_newData=false;
            // pacingData m_pacingData;
            // std::mutex m_pacingMutex;
            // std::condition_variable m_pacingCondVar;

            void waitForPacingCB();
            
        public:
            flex6k_sink_impl(std::string waveform_long_name,
                                      std::string waveform_short_name,
                                      std::string address,
                                      int txMode);
            ~flex6k_sink_impl();

            bool start();
            bool stop();

            static inline void s_stateChangeCallback(FlexRadio6000::waveform_t *waveform,
                                                     FlexRadio6000::waveform_state state, void *arg)
            {
                auto ctx = (flex6k_sink_impl *)arg;

                ctx->stateChangeCallback(state);
            }
            void stateChangeCallback(FlexRadio6000::waveform_state state);

            static inline void s_pacingCallback(std::chrono::nanoseconds ts, 
                                        FlexRadio6000::paceMode mode, int numSamples, void *arg)
            {
                auto ctx = (flex6k_sink_impl *)arg;
                // ctx->m_pacingCondVar.notify_all();
                ctx->pacingCallback(ts, mode, numSamples);// do noy call directly, copy data needed to a buffer, then awaken the thread.


            }
            // void pacingCallback();
            void pacingCallback(std::chrono::nanoseconds ts, FlexRadio6000::paceMode mode, int numSamples);

            // Where all the action really happens
            int general_work(int noutput_items,
                             gr_vector_int &ninput_items,
                             gr_vector_const_void_star &input_items,
                             gr_vector_void_star &output_items);


        };

    } // namespace Flex6000
} // namespace gr

#endif /* INCLUDED_FLEX6000_FLEX6K_SINK_IMPL_H */
