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
#include "flex_radio_6000_command_impl.h"
#include <boost/algorithm/string.hpp>

namespace gr
{
  namespace Flex6000
  {

    flex_radio_6000_command::sptr
    flex_radio_6000_command::make(std::string waveform_long_name, std::string waveform_short_name, std::string address)
    {
      return gnuradio::get_initial_sptr(new flex_radio_6000_command_impl(waveform_long_name, waveform_short_name, address));
    }

    /*
     * The private constructor
     */
    flex_radio_6000_command_impl::flex_radio_6000_command_impl(std::string waveform_long_name,
                                                               std::string waveform_short_name,
                                                               std::string address)
        : gr::block("flex_radio_6000_command",
                    gr::io_signature::make(0, 0, 0),
                    gr::io_signature::make(0, 0, 0)),
          m_cbBuffer(1000)
    {
      m_radio = FlexRadio6000::Connect(address, waveform_long_name, waveform_short_name, "RAW");
      //not connecting to any of the callbacks -> pass the command callback when sending the command
      m_radio->m_lockMap.get_lock("initLock")->stateLckCntRegister();
      m_radio->m_lockMap.get_lock("startedLock")->stateLckCntRegister();

      this->message_port_register_in(pmt::mp("cmd_in"));
      this->set_msg_handler(
          pmt::mp("cmd_in"),
          boost::bind(&flex_radio_6000_command_impl::msg_handler, this, _1));

      message_port_register_out(pmt::mp("resp_out"));
    }

    /*
     * Our virtual destructor.
     */
    flex_radio_6000_command_impl::~flex_radio_6000_command_impl()
    {
    }

    bool flex_radio_6000_command_impl::start()
    {
      m_thread = new std::thread([this]
                                 { bufferReadLoop(); });
      m_radio->m_lockMap.get_lock("initLock")->waitOnStateLck();
      m_radio->m_lockMap.get_lock("startedLock")->waitOnStateLck();
      return true;
    }

    bool flex_radio_6000_command_impl::stop()
    {
      m_cbBuffer.m_buffer.finish();
      m_thread->join();
      delete m_thread;
      return true;
    }

    void
    flex_radio_6000_command_impl::bufferReadLoop()
    {
      while (true)
      {
        m_cbBuffer.m_buffer.waitForData();
        if (m_cbBuffer.m_buffer.m_finished)
        {
          return;
        }

        //pull item and convert to strings:
        std::string tmp;
        m_cbBuffer.m_buffer.pop(tmp);

        message_port_pub(pmt::mp("resp_out"), pmt::mp(tmp));
      }
    }

    void
    flex_radio_6000_command_impl::msg_handler(pmt::pmt_t msg)
    {
      std::string str = pmt::symbol_to_string(msg);
      if (boost::algorithm::starts_with(str, "local"))
      {
        m_radio->send_local_command(m_cbBuffer.s_cmd_cb, &m_cbBuffer, str.c_str());
      }
      else
      {
        //send command to radio
        m_radio->send_api_command(m_cbBuffer.s_cmd_cb, &m_cbBuffer, str.c_str());
      }
    }
  } /* namespace Flex6000 */
} /* namespace gr */
