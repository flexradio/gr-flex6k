// SPDX-License-Identifier: LGPL-3.0-or-later
/// @file flex_buffers.h $ 
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
 * $Log: flex_buffers.h $
 * Revision LPI-LPD-Development_nlong/3 2021/08/03 15:34:31 nlong
 *    added/fixed copyrights
 * 
 * 
 **************************************************************************/

#ifndef FLEX_BUFFERS_H
#define FLEX_BUFFERS_H

#include <mutex>
#include <condition_variable>
#include <boost/circular_buffer.hpp>
#include "flex_waveformlib_ptrdef.h"
#include <iostream>
#include <string.h>
#include <atomic>

namespace flex
{
    class cmd_buffer 
    {
    public:
        boost::circular_buffer<std::string> m_cmdBuf;
        std::mutex m_lock;
        std::condition_variable m_condVar;
        std::atomic<bool> m_finished;

        cmd_buffer(size_t bufSize);

        void waitForData();

        inline void finish(){
            m_finished=true;
            m_condVar.notify_all();
        }

        inline void push(std::string val)
        {   
            std::lock_guard<std::mutex> guard(m_lock);
            if (m_cmdBuf.size()>=m_cmdBuf.capacity()-1)
            {
                std::cerr << "cmdBuf Full"<<std::endl;
                return;
            }
            m_cmdBuf.push_back(val);
            m_condVar.notify_all();
        }

        inline void pop(std::string &val)
        {
            val = m_cmdBuf[0];
            {
                std::lock_guard<std::mutex> guard(m_lock);
                m_cmdBuf.pop_front();
            }
            return;
        }
    };

    class cmd_cb_fn : private waveformlib_ptrdef
    {
        void inline cmd_cb(unsigned int code, const char *message)
        {
            size_t len = strlen(message);
            std::string str(message,len);
            m_buffer.push(str);
        }
    public:
        cmd_cb_fn(size_t bufSize);
        cmd_buffer m_buffer;

        static inline void s_cmd_cb(waveform_t *waveform,
                                    unsigned int code, char *message,
                                    void *arg)
        {
            auto ctx = (cmd_cb_fn *)arg;
            // std::cout<<message<<std::endl;
            ctx->cmd_cb(code, message);
        }
    };
}



#endif
