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

#ifndef INCLUDED_FLEX6K_FLEX6K_CONFIG_H
#define INCLUDED_FLEX6K_FLEX6K_CONFIG_H

#include <flex6k/api.h>
#include <gnuradio/block.h>

namespace gr
{
  namespace flex6k
  {

    /*!
     * \brief <+description of block+>
     * \ingroup flex6k
     *
     */
    class FLEX6K_API flex6k_config : virtual public gr::block
    {
    public:
      typedef boost::shared_ptr<flex6k_config> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of flex6k::flex6k_config.
       *
       * To avoid accidental use of raw pointers, flex6k::flex6k_config's
       * constructor is in a private implementation
       * class. flex6k::flex6k_config::make is the public interface for
       * creating new instances.
       */
      static sptr make(std::string longName, std::string shortName,
                       std::string address, int enum_tsMode, int enum_SampRate,
                       bool autostart, std::string autostart_txAnt,
                       std::string autostart_rxAnt, double autostart_freq);
    };

  } // namespace flex6k
} // namespace gr

#endif /* INCLUDED_FLEX6K_FLEX6K_CONFIG_H */
