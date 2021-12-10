// SPDX-License-Identifier: LGPL-3.0-or-later
/// @file flex6k_config_impl.h $ 
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


#ifndef INCLUDED_FLEX6K_FLEX6K_CONFIG_IMPL_H
#define INCLUDED_FLEX6K_FLEX6K_CONFIG_IMPL_H

#include <flex6k/flex6k_config.h>
#include "FlexRadio6000.h"
namespace gr
{
  namespace flex6k
  {

    class flex6k_config_impl : public flex6k_config
    {
    private:
      // Nothing to declare in this block.
      std::shared_ptr<FlexRadio6000> m_radio;

      FlexRadio6000::config m_config;

    public:
      flex6k_config_impl(std::string longName, std::string shortName,
                                         std::string address, int enum_tsMode, int enum_SampRate,
                                         bool autostart, std::string autostart_txAnt,
                                         std::string autostart_rxAnt, double autostart_freq);
      ~flex6k_config_impl();
    };

  } // namespace flex6k
} // namespace gr

#endif /* INCLUDED_FLEX6K_FLEX6K_CONFIG_IMPL_H */
