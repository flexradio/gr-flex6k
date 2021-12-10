// SPDX-License-Identifier: LGPL-3.0-or-later
/// @file flex_waveformlib_dl_intf.h $
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
