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

#ifndef INCLUDED_FLEX6000_FLEX6K_CONFIG_IMPL_H
#define INCLUDED_FLEX6000_FLEX6K_CONFIG_IMPL_H

#include <Flex6000/flex6k_config.h>
#include "FlexRadio6000.h"
namespace gr
{
  namespace Flex6000
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

  } // namespace Flex6000
} // namespace gr

#endif /* INCLUDED_FLEX6000_FLEX6K_CONFIG_IMPL_H */
