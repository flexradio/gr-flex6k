/**
 * @file flex_buffers.cc $
 * @brief base interface for flex radio
 * @author nlong
 *
 * $Revision: LPI-LPD-Development_nlong/3 $
 * $Date: 2021/08/03 15:34:31 $
 * $LastEditedBy: nlong $
 *
 * $Copyright: ©2018-2021 by Southwest Research Institute®; all rights reserved $
 **************************************************************************/

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


