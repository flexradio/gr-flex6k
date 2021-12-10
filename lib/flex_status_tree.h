// SPDX-License-Identifier: LGPL-3.0-or-later
/// @file flex_status_tree.h $ 
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


/**
 * $Log: flex_status_tree.h $
 * Revision LPI-LPD-Development_nlong/3 2021/08/03 15:34:31 nlong
 *    added/fixed copyrights
 * 
 * 
 **************************************************************************/

#ifndef FLEX_STATUS_TREE
#define FLEX_STATUS_TREE

#include <mutex>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include "flex_waveformlib_ptrdef.h"
#include <vector>
#include <iostream>
namespace flex
{

    class status_tree : private waveformlib_ptrdef
    {
    
        std::mutex m_lock;
        // boost::ptree


        boost::property_tree::ptree m_tree;
    public:

        status_tree();
        ~status_tree();


        static inline int s_statusParser(waveform_t *waveform, unsigned int argc,
                                          char *argv[], void *arg)
        {
            auto ctx = (status_tree *)arg;
            return ctx->statusParser(argc, argv);
        }

        int statusParser(unsigned int argc, char *argv[]);

        void send_local_command(flex::waveformlib_ptrdef::waveform_response_cb_t cb, void * arg, const std::string &command);

        void printTree();

        int getSliceNum(const std::string shortName, std::vector<std::string> &sliceVec );

        template <typename T>
        T getParam(std::string path)
        {
            std::lock_guard<std::mutex> guard(m_lock);
            return m_tree.get<T>(path);

        }
    };

}

#endif
