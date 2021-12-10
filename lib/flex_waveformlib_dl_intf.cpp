// SPDX-License-Identifier: LGPL-3.0-or-later
/// @file flex_waveformlib_dl_intf.cpp $ 
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
 * $Log: flex_waveformlib_dl_intf.cpp $
 * Revision LPI-LPD-Development_nlong/3 2021/08/03 15:34:31 nlong
 *    added/fixed copyrights
 * 
 * 
 **************************************************************************/

#include "flex_waveformlib_dl_intf.h"
flex::waveformlib_dl_intf::waveformlib_dl_intf(){
    std::cout<<"loading waveform lib..."<<std::endl;
    // m_handle = dlmopen(LM_ID_NEWLM,"libwaveform.so",RTLD_NOW | RTLD_LOCAL);
    // m_handle = dlmopen(LM_ID_NEWLM,"libwaveform.so", RTLD_NOW | RTLD_LOCAL | RTLD_NODELETE);
    // m_handle = dlmopen(LM_ID_NEWLM,"libwaveform.so", RTLD_NOW | RTLD_LOCAL );
    void* pthread = dlmopen(LM_ID_NEWLM,"libpthread.so.0", RTLD_NOW);
    Lmid_t ns;
    dlinfo(pthread,RTLD_DI_LMID, &ns);
    m_handle = dlmopen(ns,"libwaveform.so", RTLD_NOW | RTLD_LOCAL);
    // m_handle = dlopen("libwaveform.so",RTLD_NOW | RTLD_LOCAL);
    check_dlerror(dlerror);
    assert(m_handle); 
    std::cout<<"waveform lib loaded"<<std::endl;

    std::cout<<"loading symbols..."<<std::endl;

    *(void **) (&this->waveform_create) = dlsym(m_handle,"waveform_create");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_destroy) = dlsym(m_handle,"waveform_destroy");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_register_state_cb) = dlsym(m_handle,"waveform_register_state_cb");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_register_tx_data_cb) = dlsym(m_handle,"waveform_register_tx_data_cb");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_register_rx_data_cb) = dlsym(m_handle,"waveform_register_rx_data_cb");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_register_unknown_data_cb) = dlsym(m_handle,"waveform_register_unknown_data_cb");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_register_status_cb) = dlsym(m_handle,"waveform_register_status_cb");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_register_command_cb) = dlsym(m_handle,"waveform_register_command_cb");
    check_dlerror(dlerror);


    *(void **) (&this->waveform_send_api_command_cb) = dlsym(m_handle,"waveform_send_api_command_cb");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_send_timed_api_command_cb) = dlsym(m_handle,"waveform_send_timed_api_command_cb");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_register_meter) = dlsym(m_handle,"waveform_register_meter");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_meter_set_float_value) = dlsym(m_handle,"waveform_meter_set_float_value");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_meter_set_int_value) = dlsym(m_handle,"waveform_meter_set_int_value");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_meters_send) = dlsym(m_handle,"waveform_meters_send");
    check_dlerror(dlerror);


    *(void **) (&this->waveform_radio_create) = dlsym(m_handle,"waveform_radio_create");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_radio_destroy) = dlsym(m_handle,"waveform_radio_destroy");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_radio_wait) = dlsym(m_handle,"waveform_radio_wait");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_radio_start) = dlsym(m_handle,"waveform_radio_start");
    check_dlerror(dlerror);


    *(void **) (&this->waveform_send_data_packet) = dlsym(m_handle,"waveform_send_data_packet");
    check_dlerror(dlerror);
    *(void **) (&this->get_packet_len) = dlsym(m_handle,"get_packet_len");
    check_dlerror(dlerror);
    *(void **) (&this->get_packet_data) = dlsym(m_handle,"get_packet_data");
    check_dlerror(dlerror);
    *(void **) (&this->get_packet_ts_int) = dlsym(m_handle,"get_packet_ts_int");
    check_dlerror(dlerror);
    *(void **) (&this->get_packet_ts_frac) = dlsym(m_handle,"get_packet_ts_frac");
    check_dlerror(dlerror);
    *(void **) (&this->get_packet_ts) = dlsym(m_handle,"get_packet_ts");
    check_dlerror(dlerror);
    *(void **) (&this->get_stream_id) = dlsym(m_handle,"get_stream_id");
    check_dlerror(dlerror);
    *(void **) (&this->get_class_id) = dlsym(m_handle,"get_class_id");
    check_dlerror(dlerror);
    *(void **) (&this->get_packet_count) = dlsym(m_handle,"get_packet_count");
    check_dlerror(dlerror);


    *(void **) (&this->waveform_set_context) = dlsym(m_handle,"waveform_set_context");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_get_context) = dlsym(m_handle,"waveform_get_context");
    check_dlerror(dlerror);

    *(void **) (&this->waveform_register_meter_list) = dlsym(m_handle,"waveform_register_meter_list");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_discover_radio) = dlsym(m_handle,"waveform_discover_radio");
    check_dlerror(dlerror);
    *(void **) (&this->waveform_set_log_level) = dlsym(m_handle,"waveform_set_log_level");
    check_dlerror(dlerror);

    std::cout <<"syms loaded."<<std::endl;
}


flex::waveformlib_dl_intf::~waveformlib_dl_intf(){
    // exit(0);
    dlclose(m_handle);
    check_dlerror(dlerror);
}

//error checking:
void flex::waveformlib_dl_intf::check_dlerror(char* (dlerror) (void)){
    char* error;
    while ((error=dlerror())!=NULL){
        perror(error);
    }
}
