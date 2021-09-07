/**
 * @file flex_waveformlib_dl_intf.h $
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
 * $Log: flex_waveformlib_dl_intf.h $
 * Revision LPI-LPD-Development_nlong/3 2021/08/03 15:34:31 nlong
 *    added/fixed copyrights
 * 
 * 
 **************************************************************************/

#ifndef FLEX_WAVEFORMLIB_INTF_H
#define FLEX_WAVEFORMLIB_INTF_H

#include <dlfcn.h>
#include <cassert>
#include <iostream>

#include "flex_waveformlib_ptrdef.h"
#include <vector>
namespace flex
{
    class waveformlib_dl_intf : public waveformlib_ptrdef
    {
    private:
        static void check_dlerror(char *(dlerror)(void));
        void* m_handle;
    public:
        waveformlib_dl_intf();
        ~waveformlib_dl_intf();
    };

} // namespace flex

#endif
