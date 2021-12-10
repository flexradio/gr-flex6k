// SPDX-License-Identifier: LGPL-3.0-or-later
/// @file flex6k_command_impl.h $ 
/// @author nlong
///
/// @copyright: (c) 2018-2021 by Southwest Research Institute®
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Lesser General Public License as published by
/// the Free Software Foundation, version 3.
///
/// This program is distributed in the hope that it will be useful, but
/// WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
/// Lesser General Public License for more details.
///
/// You should have received a copy of the GNU Lesser General Public License
/// along with this program. If not, see <http://www.gnu.org/licenses/>.
///

#ifndef INCLUDED_FLEX6K_FLEX6K_COMMAND_IMPL_H
#define INCLUDED_FLEX6K_FLEX6K_COMMAND_IMPL_H

#include <flex6k/flex6k_command.h>
#include "flex_buffers.h"
#include "FlexRadio6000.h"
#include <array>
#include <thread>
namespace gr
{
  namespace flex6k
  {

    class flex6k_command_impl : public flex6k_command
    {
    private:
      // Nothing to declare in this block.
      // std::array<char,1000> test;
      std::shared_ptr<FlexRadio6000> m_radio;

      std::string m_waveform_long_name;
      std::string m_waveform_short_name;
      std::string m_address;

      flex::cmd_cb_fn m_cbBuffer;

      std::thread * m_thread;

    public:
      flex6k_command_impl(std::string waveform_long_name, std::string waveform_short_name, std::string address);
      ~flex6k_command_impl();

      // Where all the action really happens
      // void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      bool start();
      bool stop();

      void bufferReadLoop();
      // int general_work(int noutput_items,
      //                  gr_vector_int &ninput_items,
      //                  gr_vector_const_void_star &input_items,
      //                  gr_vector_void_star &output_items);

      //handle msg just transmits a command:
      void msg_handler(pmt::pmt_t msg);
    };

  } // namespace flex6k
} // namespace gr

#endif /* INCLUDED_FLEX6K_FLEX6K_COMMAND_IMPL_H */
