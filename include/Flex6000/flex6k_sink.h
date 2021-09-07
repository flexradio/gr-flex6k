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

#ifndef INCLUDED_FLEX6000_FLEX6K_SINK_H
#define INCLUDED_FLEX6000_FLEX6K_SINK_H

#include <Flex6000/api.h>
#include <gnuradio/sync_block.h>

namespace gr
{
  namespace Flex6000
  {

    /*!
     * \brief <+description of block+>
     * \ingroup Flex6000
     *
     */
    class FLEX6000_API flex6k_sink : virtual public gr::block
    {
    public:
      typedef boost::shared_ptr<flex6k_sink> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of Flex6000::flex6k_sink.
       *
       * To avoid accidental use of raw pointers, Flex6000::flex6k_sink's
       * constructor is in a private implementation
       * class. Flex6000::flex6k_sink::make is the public interface for
       * creating new instances.
       * 
       * \param txMode
       * this is the transmitter mode, a enum type:
       * unimplemented Continuous with Forced Stop:
       * ----Externaly activated/deactivated.
       * ----When in tx mode, sends data as availiable.
       * ----when Tx requests to stop, stops immediately.
       * 
       * unimplemented Continuous with Delayed Stop:
       * ----Externaly activated/deactivated.
       * ----When in tx mode, sends data as availiable.
       * ----when Tx requests to stop, continues to transmit until out of data.
       * 
       * Continuous with Auto Start and Auto Stop:
       * ----Auto-activates when data is on the input buffer and sob.
       * ----When in tx mode, sends data while availiable.
       * ----When no more data, auto deactivates.
       * 
       * unimplemented Burst with Auto Start and Auto Stop w/o Timestamps:
       * ----Requires tx_sob and tx_eob tags.
       * ----Auto-activates when data is on the input buffer.
       * ----When in tx mode, sends data as availiable.
       * ----When eob, auto deactivates.
       * 
       * unimplemented Burst with Auto Start and Auto Stop w/ Timestamps:
       * ----Requires tx_sob and tx_eob tags.
       * ----Auto-activates when data is on the input buffer.
       * ----When in tx mode, sends data as availiable.
       * ----When eob, auto deactivates.
       * 
       * All of the above can accept tx_freq, and tx_command tags
       * TSB are not implemented...
       * 
       */

      static sptr make(std::string waveform_long_name,
                       std::string waveform_short_name,
                       std::string address,
                       int txMode);
    };

  } // namespace Flex6000
} // namespace gr

#endif /* INCLUDED_FLEX6000_FLEX6K_SINK_H */
