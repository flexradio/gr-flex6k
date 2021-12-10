// SPDX-License-Identifier: LGPL-3.0-or-later
/// @file flex6k_config_impl.cc $ 
/// @author nlong
///
/// $Revision: LPI-LPD-Development_nlong/8 $
/// $Date: 2021/08/03 15:34:31 $
/// $LastEditedBy: nlong $
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "flex6k_config_impl.h"

namespace gr
{
  namespace flex6k
  {

    flex6k_config::sptr
    flex6k_config::make(std::string longName, std::string shortName, std::string address, int enum_tsMode, int enum_SampRate, bool autostart,
                                        std::string autostart_txAnt,
                                        std::string autostart_rxAnt,
                                        double autostart_freq)
    {
      return gnuradio::get_initial_sptr(new flex6k_config_impl(longName, shortName, address, enum_tsMode, enum_SampRate, autostart, autostart_txAnt, autostart_rxAnt,
                                                                               autostart_freq));
    }

    /*
     * The private constructor
     */
    flex6k_config_impl::flex6k_config_impl(
        std::string longName, std::string shortName,
        std::string address, int enum_tsMode,
        int enum_SampRate,
        bool autostart,
        std::string autostart_txAnt,
        std::string autostart_rxAnt,
        double autostart_freq)
        : gr::block("flex6k_config",
                    gr::io_signature::make(0, 0, 0),
                    gr::io_signature::make(0, 0, 0))
    {
      m_radio = FlexRadio6000::Connect(address, longName, shortName, "RAW");

      m_config.tsMode = static_cast<FlexRadio6000::config::timestampMode_t>(enum_tsMode);
      m_config.fsIQ = static_cast<FlexRadio6000::config::sampleRateIQ_t>(enum_SampRate);

      m_config.autostart = autostart;
      m_config.autoStart_txAnt = autostart_txAnt;
      m_config.autoStart_rxAnt = autostart_rxAnt;
      m_config.autoStart_freq = autostart_freq;

      m_radio->setConfig(m_config);

      m_radio->enforceMaxInstances("flexConfiguration", 1);
      GR_LOG_INFO(d_logger, "FlexRadio6000 config constructed");
    }
    /*
     * Our virtual destructor.
     */
    flex6k_config_impl::~flex6k_config_impl()
    {
    }

  } /* namespace flex6k */
} /* namespace gr */
