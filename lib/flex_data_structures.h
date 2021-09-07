/**
 * @file flex_data_structures.h $
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
 * $Log: flex_data_structures.h $
 * Revision LPI-LPD-Development_nlong/3 2021/08/03 15:34:31 nlong
 *    added/fixed copyrights
 * 
 * 
 **************************************************************************/
#ifndef FLEX_DATA_STRUCTURES
#define FLEX_DATA_STRUCTURES

namespace flex
{

    //radio state that the RX and TX blocks use to keep track of state w/ the callback...

    class radioState
    {
    public:
        FlexRadio6000::waveform_state m_state_curr = FlexRadio6000::waveform_state::INACTIVE;
        FlexRadio6000::waveform_state m_state_prev = FlexRadio6000::waveform_state::INACTIVE;
        bool m_state_active = false;
        bool m_state_changed = false;

    public:
        // FlexRadio6000::waveform_state inline get_curr()
        // {
        //     std::lock_guard<std::mutex> lock(m_stateMutex);
        //     return m_state_curr;
        // }

        // FlexRadio6000::waveform_state inline get_prev()
        // {
        //     std::lock_guard<std::mutex> lock(m_stateMutex);
        //     return m_state_prev;
        // }

        // bool inline get_active()
        // {
        //     std::lock_guard<std::mutex> lock(m_stateMutex);
        //     return m_state_active;
        // }

        // bool inline get_changed()
        // {
        //     std::lock_guard<std::mutex> lock(m_stateMutex);
        //     return m_state_changed;
        // }

        // void inline set_curr(FlexRadio6000::waveform_state state_curr)
        // {
        //     std::lock_guard<std::mutex> lock(m_stateMutex);
        //     m_state_curr = state_curr;
        //     return;
        // }

        // void inline set_prev(FlexRadio6000::waveform_state state_prev)
        // {
        //     std::lock_guard<std::mutex> lock(m_stateMutex);
        //     m_state_prev = state_prev;
        //     return;
        // }

        // void inline set_active(bool state_active)
        // {
        //     std::lock_guard<std::mutex> lock(m_stateMutex);
        //     m_state_active = state_active;
        //     return;
        // }

        // void inline set_changed(bool state_changed)
        // {
        //     std::lock_guard<std::mutex> lock(m_stateMutex);
        //     m_state_changed = state_changed;
        //     return;
        // }
    };




}

#endif
