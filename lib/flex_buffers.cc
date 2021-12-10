// SPDX-License-Identifier: LGPL-3.0-or-later
/// @file flex_buffers.cc $ 
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
 * $Log: flex_buffers.cc $
 * Revision LPI-LPD-Development_nlong/3 2021/08/03 15:34:31 nlong
 *    added/fixed copyrights
 * 
 * 
 **************************************************************************/

#include "flex_buffers.h"
#include <iostream>
#include <string.h>
//buffers:

flex::cmd_buffer::cmd_buffer(size_t bufSize) : m_cmdBuf(bufSize)
{
    m_finished=false;
}

void flex::cmd_buffer::waitForData()
{
    std::unique_lock<std::mutex> lock(m_lock);

    if(m_cmdBuf.size()==0)
    {
        m_condVar.wait(lock,[&]()
        {
            return m_cmdBuf.size()>0 || m_finished==true;
        });
    }
}


flex::cmd_cb_fn::cmd_cb_fn(size_t bufSize) : m_buffer(bufSize)
{
}


