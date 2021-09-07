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

#ifndef INCLUDED_FLEX6000_FLEX6K_SOURCE_H
#define INCLUDED_FLEX6000_FLEX6K_SOURCE_H

#include <Flex6000/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace Flex6000 {

    /*!
     * \brief <+description of block+>
     * \ingroup Flex6000
     *
     */
    class FLEX6000_API flex6k_source : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<flex6k_source> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of Flex6000::flex6k_source.
       *
       * To avoid accidental use of raw pointers, Flex6000::flex6k_source's
       * constructor is in a private implementation
       * class. Flex6000::flex6k_source::make is the public interface for
       * creating new instances.
       */
      static sptr make(std::string waveform_long_name,
                       std::string waveform_short_name,
                       std::string address,
                       bool swapIQ,
                                        bool sendZerosWhileTX);
    };

  } // namespace Flex6000
} // namespace gr

#endif /* INCLUDED_FLEX6000_FLEX6K_SOURCE_H */

