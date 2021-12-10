// SPDX-License-Identifier: LGPL-3.0-or-later
/// @file flex_waveformlib_static_intf.cpp $
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
 * $Log: flex_waveformlib_static_intf.cpp $
 * Revision LPI-LPD-Development_nlong/3 2021/08/03 15:34:31 nlong
 *    added/fixed copyrights
 * 
 * 
 **************************************************************************/

extern "C"{
    #include <waveform/waveform_api.h>
}

#include "flex_waveformlib_static_intf.h"

flex::waveformlib_static_intf::waveformlib_static_intf(){

    *(void **) (&this->waveform_create) = (void *) &::waveform_create;

    *(void **) (&this->waveform_destroy) = (void *) &::waveform_destroy;
 
    *(void **) (&this->waveform_register_state_cb) = (void *) &::waveform_register_state_cb;
 
    *(void **) (&this->waveform_register_tx_data_cb) = (void *) &::waveform_register_tx_data_cb;
 
    *(void **) (&this->waveform_register_rx_data_cb) = (void *) &::waveform_register_rx_data_cb;
 
    *(void **) (&this->waveform_register_unknown_data_cb) = (void *) &::waveform_register_unknown_data_cb;
 
    *(void **) (&this->waveform_register_status_cb) = (void *) &::waveform_register_status_cb;
 
    *(void **) (&this->waveform_register_command_cb) = (void *) &::waveform_register_command_cb;
 


    *(void **) (&this->waveform_send_api_command_cb) = (void *) &::waveform_send_api_command_cb;
 
    *(void **) (&this->waveform_send_timed_api_command_cb) = (void *) &::waveform_send_timed_api_command_cb;
 
    *(void **) (&this->waveform_register_meter) = (void *) &::waveform_register_meter;
 
    *(void **) (&this->waveform_meter_set_float_value) = (void *) &::waveform_meter_set_float_value;
 
    *(void **) (&this->waveform_meter_set_int_value) = (void *) &::waveform_meter_set_int_value;
 
    *(void **) (&this->waveform_meters_send) = (void *) &::waveform_meters_send;
 


    *(void **) (&this->waveform_radio_create) =  (void*) &::waveform_radio_create;
 
    *(void **) (&this->waveform_radio_destroy) = (void*) &::waveform_radio_destroy;
 
    *(void **) (&this->waveform_radio_wait) = (void*) &::waveform_radio_wait;
 
    *(void **) (&this->waveform_radio_start) = (void*) &::waveform_radio_start;
 


    *(void **) (&this->waveform_send_data_packet) = (void*) &::waveform_send_data_packet;
 
    *(void **) (&this->get_packet_len) = (void*) &::get_packet_len;
 
    *(void **) (&this->get_packet_data) = (void*) &::get_packet_data;
 
    *(void **) (&this->get_packet_ts_int) = (void*) &::get_packet_ts_int;
 
    *(void **) (&this->get_packet_ts_frac) = (void*) &::get_packet_ts_frac;
 
    *(void **) (&this->get_packet_ts) = (void*) &::get_packet_ts;
 
    *(void **) (&this->get_stream_id) = (void*) &::get_stream_id;
 
    *(void **) (&this->get_class_id) = (void*) &::get_class_id;
 
    *(void **) (&this->get_packet_count) = (void*) &::get_packet_count;
 


    *(void **) (&this->waveform_set_context) = (void*) &::waveform_set_context;
 
    *(void **) (&this->waveform_get_context) = (void*) &::waveform_get_context;
 

    *(void **) (&this->waveform_register_meter_list) = (void*) &::waveform_register_meter_list;
 
    *(void **) (&this->waveform_discover_radio) = (void*) &::waveform_discover_radio;
 
    *(void **) (&this->waveform_set_log_level) = (void*) &::waveform_set_log_level;
 

    // std::cout <<"syms loaded."<<std::endl;
}


flex::waveformlib_static_intf::~waveformlib_static_intf(){
 
}
