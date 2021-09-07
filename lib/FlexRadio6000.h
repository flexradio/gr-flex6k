/**
 * @file FlexRadio6000.h $
 * @brief base interface for flex radio
 * @author nlong
 *
 * $Revision: LPI-LPD-Development_nlong/8 $
 * $Date: 2021/08/03 15:34:31 $
 * $LastEditedBy: nlong $
 *
 * $Copyright: ©2018-2021 by Southwest Research Institute®; all rights reserved $
 **************************************************************************/

/**
 * $Log: FlexRadio6000.h $
 * Revision LPI-LPD-Development_nlong/8 2021/08/03 15:34:31 nlong
 *    added/fixed copyrights
 * 
 * 
 **************************************************************************/

#ifndef GR_FLEX6000_FLEXRADIO6000_H
#define GR_FLEX6000_FLEXRADIO6000_H

#include <mutex>
#include <condition_variable>

#include <netinet/in.h>
#include <map>
#include <deque>
#include <complex>
#include <set>
#include <vector>
#include <thread>
#include <array>
#include "flex_status_tree.h"
#include "string.h"
#include <atomic>
#include <algorithm>

#ifdef USE_DYNAMIC
#include "flex_waveformlib_dl_intf.h"
class FlexRadio6000 : public flex::waveformlib_dl_intf
#else
#include "flex_waveformlib_static_intf.h"
class FlexRadio6000 : public flex::waveformlib_static_intf
#endif
{

    //class structure:
    class stateLock
    {

        //state lock: forces the initialization between multiple blocks to wait until all blocks register.
        //this is bad, b/c its a self made primitive.

        //potentially has issues if someone tries to relock while it is still emptying.

        std::mutex m_stateLck_mutex;
        std::condition_variable m_stateLck_conditionVar;
        int m_stateLck_numWaiting = 0;
        int m_stateLck_numSub = 0;
        int m_stateLck_numThruLck = 0;
        int m_stateLck_minSubNum = 0;

        std::condition_variable m_stateLck_readyConditionVar;
        bool m_stateLck_ready = true;

    public:
        stateLock();
        ~stateLock();
        //state gating
        void stateLckCntRegister()
        {
            stateLckCntRegister(false);
        }
        void stateLckCntDeregister()
        {
            stateLckCntDeregister(false);
        }

        void stateLckCntRegister(bool increment_min_subscriber_cnt);
        void stateLckCntDeregister(bool decrement_min_subscriber_cnt);
        void waitOnStateLck();
    };

    class stateLockMap
    {
        std::map<std::string, std::shared_ptr<stateLock>> m_lockMap;
        std::mutex m_lockMapMutex;

    public:
        std::shared_ptr<stateLock> get_lock(std::string lockName);
    };

public:
    static std::shared_ptr<FlexRadio6000> Connect(const std::string &address,
                                                  const std::string &longName,
                                                  const std::string &shortName,
                                                  const std::string &mode);
    static int Disconnect(const std::string &address,
                          const std::string &shortName);
    // void Disconnect(); //disconnections do not work at the moment...
    // bool valid();

    friend std::shared_ptr<FlexRadio6000> std::make_shared<FlexRadio6000>(const sockaddr_in &addr, const std::string &longName,
                                                                          const std::string &shortName, const std::string &mode);
    // friend std::shared_ptr<FlexRadio6000>;
    const static std::vector<std::string> s_validModes;

    // public: // I wish they could be private...
    ~FlexRadio6000();
    explicit FlexRadio6000(const sockaddr_in &addr, const std::string &longName,
                           const std::string &shortName, const std::string &mode);

    //expose callback registration:

    int inline register_state_cb(flex::waveformlib_ptrdef::waveform_state_cb_t cb, void *arg)
    {
        std::lock_guard<std::mutex> lock(m_radio_api_lock);
        return waveform_register_state_cb(m_waveform, cb, arg);
    }

    int inline register_unknown_data_cb(flex::waveformlib_ptrdef::waveform_data_cb_t cb, void *arg)
    {
        std::lock_guard<std::mutex> lock(m_radio_api_lock);
        return waveform_register_unknown_data_cb(m_waveform, cb, arg);
    }

    int inline register_status_cb(const std::string &status_name, flex::waveformlib_ptrdef::waveform_cmd_cb_t cb, void *arg)
    {
        char pc_status_name[status_name.size() + 1];
        strncpy(pc_status_name, status_name.c_str(), sizeof(pc_status_name));
        std::lock_guard<std::mutex> lock(m_radio_api_lock);
        return waveform_register_status_cb(m_waveform, pc_status_name, cb, arg);
    }

    int inline register_command_cb(const std::string &command_name, flex::waveformlib_ptrdef::waveform_cmd_cb_t cb, void *arg)
    {
        char pc_command_name[command_name.size() + 1];
        strncpy(pc_command_name, command_name.c_str(), sizeof(pc_command_name));
        std::lock_guard<std::mutex> lock(m_radio_api_lock);
        return waveform_register_command_cb(m_waveform, pc_command_name, cb, arg);
    }

    long inline send_api_command(flex::waveformlib_ptrdef::waveform_response_cb_t cb, void *arg, const std::string &command)
    {
        char pc_command[command.size() + 1];
        strncpy(pc_command, command.c_str(), sizeof(pc_command));
        std::lock_guard<std::mutex> lock(m_radio_api_lock);
        return waveform_send_api_command_cb(m_waveform, cb, arg, pc_command);
    }

    //method for debugging and passing data via messages...
    long inline send_local_command(flex::waveformlib_ptrdef::waveform_response_cb_t cb, void *arg, const std::string &command)
    {
        m_statusTree.send_local_command(cb, arg, command);
        return 0;
    }

    void inline get_wvfrm_sliceNum(std::string &str)
    {
        std::vector<std::string> strVec;
        m_statusTree.getSliceNum(m_shortName, strVec);
        if (strVec.size() > 0)
        {
            str = strVec[0];
        }
        else
        {
            str = "";
        }
    }

    int enforceMaxInstances(std::string name, int maxInst);

    //register callback function types for data pacing callbacks
    enum paceMode
    {
        RX,
        TX
    };

    // the Tx callback just needs the adjusted timestamp, and the current mode, no data.
    // typedef void (*pacingTX_cb_t)(timespec *ts, paceMode mode, void *arg);
    //num samples is number of 64 bit combine iq samples. this is important for tx, as it specifies how many samples should be
    typedef void (*pacingTX_cb_t)(std::chrono::nanoseconds ts, paceMode mode, int numSamples, void *arg);
    //the Rx callback needs the adjusted timestamp, current mode, and the data.
    typedef void (*pacingRX_cb_t)(std::chrono::nanoseconds ts, paceMode mode, int numSamples, float *packetData, void *arg);

    //pacing callback is a combination of both the TX and RX callbacks,
    //coallated together in order to always recieve info.
    //the return type is a custom data type, and includes a custom field with a compensated timestamp value

    int inline register_pacingTX_cb(pacingTX_cb_t cb, void *arg)
    {
        std::lock_guard<std::mutex> lock(m_callback_list_mutex);
        std::pair<pacingTX_cb_t, void *> pair(cb, arg);
        m_tx_callbackVec.push_back(pair);
        return 0;
    }
    int deregister_pacingTX_cb(pacingTX_cb_t cb, void *arg);

    int inline register_pacingRX_cb(pacingRX_cb_t cb, void *arg)
    {
        std::lock_guard<std::mutex> lock(m_callback_list_mutex);
        std::pair<pacingRX_cb_t, void *> pair(cb, arg);
        m_rx_callbackVec.push_back(pair);
        return 0;
    }
    int deregister_pacingRX_cb(pacingRX_cb_t cb, void *arg);

    void inline send_data_packet(float *samples, size_t num_samples, waveform_packet_type type)
    {

        waveform_send_data_packet(m_waveform, samples, num_samples, type);
    }

    stateLockMap m_lockMap;

    struct config
    {
        enum timestampMode_t : int
        {
            PC,
            RADIO
        };
        enum sampleRateIQ_t : int
        {
            FS240000
        };

    
        bool autostart = false;
        std::string autoStart_txAnt="xvtr";
        std::string autoStart_rxAnt="rx";
        double autoStart_freq=0;

        timestampMode_t tsMode = timestampMode_t::PC;
        sampleRateIQ_t fsIQ = sampleRateIQ_t::FS240000;
    };

    void inline setConfig(config config)
    {
        std::lock_guard<std::mutex> lock(m_configMutex);
        m_config = config;
        if (m_config.fsIQ == m_config.FS240000)
        {
            m_config_fs = 24000.0;
        }
    }

    double inline getSampRate(){
        std::lock_guard<std::mutex> lock(m_configMutex);
        return m_config_fs;
    }

    //print response of commands to terminal, these are all encapsulated in a static lock to make sure prints succeed.
    //this is only used for debug...
public:
    static inline void s_printCmd(struct waveform_t *waveform, unsigned int code, char *message, void *arg)
    {
        auto ctx = (FlexRadio6000 *)arg;
        ctx->printCmd(code, message, NULL);
    }
    void printCmd(unsigned int code, char *message, void *arg);

    static inline int s_printCmd_cb(struct waveform_t *waveform, unsigned int argc, char *argv[], void *arg)
    {
        auto ctx = (FlexRadio6000 *)arg;
        ctx->printCmd_cb("command", argc, argv);
        return 0;
    }
    void printCmd_cb(const char *cb_name, unsigned int argc, char *argv[]);

private:
    //static and const stuff:
    const int s_start_wait_time = 25;

    static std::mutex s_lock;
    static std::map<std::pair<std::string, std::string>, std::weak_ptr<FlexRadio6000>> s_FlexRadio6000Instances;

private:
    static inline void s_processRX(struct waveform_t *waveform, struct waveform_vita_packet *packet, size_t packet_size, void *arg)
    {
        auto ctx = (struct FlexRadio6000 *)arg;
        ctx->processRX(packet, packet_size);
    }
    void processRX(struct waveform_vita_packet *packet, size_t packet_size);

    static inline void s_processTX(struct waveform_t *waveform, struct waveform_vita_packet *packet, size_t packet_size, void *arg)
    {
        auto ctx = (struct FlexRadio6000 *)arg;
        ctx->processTX(packet, packet_size);
    }
    void processTX(struct waveform_vita_packet *packet, size_t packet_size);

    static inline void s_processUnknown(struct waveform_t *waveform, struct waveform_vita_packet *packet, size_t packet_size, void *arg)
    {
        auto ctx = (struct FlexRadio6000 *)arg;
        ctx->processUnknown(packet, packet_size);
    }
    void processUnknown(struct waveform_vita_packet *packet, size_t packet_size);

    static inline void s_processState(struct waveform_t *waveform, enum waveform_state state, void *arg)
    {
        auto ctx = (struct FlexRadio6000 *)arg;
        ctx->processState(state);
    }
    void processState(enum waveform_state state);

    // api mutex
    std::mutex m_radio_api_lock;
    //input parameters:
    sockaddr_in m_addr; //???
    std::string const m_longName;
    std::string const m_shortName;
    std::string const m_mode;

    //general config that applies to the whole radio, set with the radio config block:
    //should never rally change after a sigle guy sets, but whatever...
    std::mutex m_configMutex;
    config m_config;
    double m_config_fs = 24000;

    // these need to be freed separately
    struct radio_t *m_radio; //???
    waveform_t *m_waveform;

    //subobject instances, kept separate for cleanliness:
    flex::status_tree m_statusTree;

    //highlander rules: enforce single instatiation of certain classes (ie tx)
    std::map<std::string, std::pair<int, int>> m_highlanderMap;
    std::mutex m_highlanderMutex;

    //thread thats waits for the radio to die...
    std::thread *m_kill_thread;

    //pacing and timestamp adjust wrapper:

    std::mutex m_callback_list_mutex;
    std::vector<std::pair<pacingTX_cb_t, void *>> m_tx_callbackVec;
    // std::vector<std::pair<pacingTX_cb_t *,void *>> m_tx_callbackVec;
    std::vector<std::pair<pacingRX_cb_t, void *>> m_rx_callbackVec;

    std::chrono::nanoseconds m_last_cb_time;
    std::chrono::nanoseconds m_est_next_cb_time;
    uint64_t m_packetNum = 0;
    void init_pacing_cb();

    int inline register_tx_data_cb(flex::waveformlib_ptrdef::waveform_data_cb_t cb, void *arg)
    {
        std::lock_guard<std::mutex> lock(m_radio_api_lock);
        return waveform_register_tx_data_cb(m_waveform, cb, arg);
    }

    int inline register_rx_data_cb(flex::waveformlib_ptrdef::waveform_data_cb_t cb, void *arg)
    {
        std::lock_guard<std::mutex> lock(m_radio_api_lock);
        return waveform_register_rx_data_cb(m_waveform, cb, arg);
    }

    static inline void s_txCallback(FlexRadio6000::waveform_t *waveform,
                                    FlexRadio6000::waveform_vita_packet *packet,
                                    size_t packet_size, void *arg)
    {
        auto ctx = (FlexRadio6000 *)arg;
        ctx->pacingCallback(packet, packet_size, paceMode::TX);
    }

    static inline void s_rxCallback(FlexRadio6000::waveform_t *waveform,
                                    FlexRadio6000::waveform_vita_packet *packet,
                                    size_t packet_size, void *arg)
    {
        auto ctx = (FlexRadio6000 *)arg;
        ctx->pacingCallback(packet, packet_size, paceMode::RX);
    }

    void pacingCallback(waveform_vita_packet *packet,
                        size_t packet_size, paceMode mode);

    //timestamp debug:
    std::ofstream m_ts_debugStream;
    bool m_startOfDebug = true;
    uint64_t m_nsStartOffest;
};

#endif //GR_FLEX6000_FLEXRADIO6000_H
