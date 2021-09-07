/**
 * @file FlexRadio6000.cpp $
 * @brief base interface for flex radio
 * @author nlong
 *
 * $Revision: LPI-LPD-Development_nlong/11 $
 * $Date: 2021/08/09 18:10:01 $
 * $LastEditedBy: nlong $
 *
 * $Copyright: ©2018-2021 by Southwest Research Institute®; all rights reserved $
 **************************************************************************/

/**
 * $Log: FlexRadio6000.cpp $
 * Revision LPI-LPD-Development_nlong/11 2021/08/09 18:10:01 nlong
 *    fixed disconttinuity bug
 * 
 * Revision LPI-LPD-Development_nlong/10 2021/08/03 15:34:31 nlong
 *    added/fixed copyrights
 * 
 * Revision LPI-LPD-Development_nlong/9 2021/08/02 15:35:47 nlong
 *    added header file
 * 
 **************************************************************************/

#include <iostream>
#include <fstream>
#include "FlexRadio6000.h"
#include <thread>
#include <vector>
#include <algorithm>
// #include <unistd.h>
#include <arpa/inet.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <algorithm>
#include <chrono>
#include <memory>
#include <boost/lexical_cast.hpp>

std::mutex FlexRadio6000::s_lock;
std::map<std::pair<std::string, std::string>, std::weak_ptr<FlexRadio6000>> FlexRadio6000::s_FlexRadio6000Instances;

const std::vector<std::string> FlexRadio6000::s_validModes{"OFF", "USB", "TEST", "LSB",
                                                           "FM", "NFM", "DFM", "AM", "CW",
                                                           "DIGU", "DIGL", "SAM", "RAPM",
                                                           "RTTY", "RAW", "ARQ"};

std::shared_ptr<FlexRadio6000> FlexRadio6000::Connect(const std::string &address,
                                                      const std::string &longName,
                                                      const std::string &shortName,
                                                      const std::string &mode)
{
    //do prechecking to ensure validity of parameters:
    //check mode parameter:
    //todo: check that all instances
    {
        bool found = (std::find(s_validModes.begin(), s_validModes.end(), mode) != s_validModes.end());
        if (!found)
        {
            std::cout << "Radio mode \"" << mode << "\" is not a valid radio mode. Mode must be one of: ";
            for (const auto &value : s_validModes)
            {
                std::cout << value << " ";
            }
            std::cout << std::endl;
            return nullptr;
        }
    }
    std::string shortNameUp = boost::to_upper_copy(shortName);
    if (shortNameUp.size() > 4)
    {
        std::cout << "Waveform short name \"" << shortNameUp << "\" must be less than 4 charecters" << std::endl;
        return nullptr;
    }

    {
        bool found = (std::find(s_validModes.begin(), s_validModes.end(), shortNameUp) != s_validModes.end());
        if (found)
        {
            std::cout << "Waveform short name \"" << shortNameUp << "\" is not a valid name (is a default mode). Short name must not be one of: ";
            for (const auto &value : s_validModes)
            {
                std::cout << value << " ";
            }
            std::cout << std::endl;
            return nullptr;
        }
    }

    //check that radio ip address is valid,and that the new mode does not conflict.

    //look through current connections for instance of the radio.
    std::pair<std::string, std::string> id_pair(address, shortName);
    std::lock_guard<std::mutex> guard(s_lock);
    auto instance_iterator = s_FlexRadio6000Instances.find(id_pair);
    if (instance_iterator == s_FlexRadio6000Instances.end())
    {
        //check that the long name is not already registered:
        for (auto iter = s_FlexRadio6000Instances.begin();
             iter != s_FlexRadio6000Instances.end(); ++iter)
        {
            //make a temp shared pointer to grab the long name:
            std::shared_ptr<FlexRadio6000> sharedObject = std::shared_ptr<FlexRadio6000>(iter->second);
            std::string member_str = sharedObject->m_longName;
            if ((longName.compare(member_str)) == 0)
            {
                std::cout << "Waveform long name \"" << longName << "\" already exists" << std::endl;

                return nullptr;
            }
        }

//In the case where we are compiled without -DUSE_DYNAMIC
//we can only have one instance of a radio/waveform.
#ifndef USE_DYNAMIC
        //no dynamic, check that this is the first item in the map.
        if (s_FlexRadio6000Instances.size() >= 1)
        {
            std::cerr << "Simultaneous multiple waveforms not supported without dynamic loading the FlexRadio API, recompile gr-Flex6000 with -DUSE_DYNAMIC=True cmake command to enable" << std::endl;
            assert(0);
        }
#endif

        //we could not find a instance of the waveform allready in existance.
        // struct timeval timeout ={.tv_sec = 10, .tv_usec = 0};

        std::vector<std::string> parts;
        boost::split(parts, address, boost::is_any_of(":"));

        if (parts.size() > 2)
        {
            std::cout << "invalid radio address format" << std::endl;
            return nullptr;
        }

        //auto addr = waveform_discover_radio(&timeout);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = 32787;

        if (parts.size() == 2)
        {
            addr.sin_port = atoi(parts[1].c_str());
        }

        std::cout << "Connecting to " << parts[0] << " port " << addr.sin_port << std::endl;

        inet_pton(AF_INET, parts[0].c_str(), &addr.sin_addr);

        auto ip_addr = addr.sin_addr.s_addr;
        printf("Radio address %d.%d.%d.%d:%d\n",
               0xff & ip_addr,
               0xff & (ip_addr >> 8),
               0xff & (ip_addr >> 16),
               ip_addr >> 24,
               addr.sin_port);

        //try to connect to radio check that the address is valid and that the waveform names do not conflict.
        //this does not work, just hope that you get ok values...
        // {
        //     struct radio_t* tmp_radio = waveform_radio_create(&addr);
        //     waveform_t* tmp_waveform = waveform_create(tmp_radio, "diag", "diag", "OFF", "1.0.0");
        //     waveform_radio_start(tmp_radio);

        //     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //     long val = waveform_send_api_command_cb(tmp_waveform,NULL,NULL,"info");

        //     waveform_destroy(tmp_waveform);
        //     waveform_radio_destroy(tmp_radio);

        //     std::cout<<"val is "<< val<<std::endl;
        // }

        //would prefer that the constructor/destructor is private.
        // FlexRadio6000 * evilPointer = new FlexRadio6000(addr, longName, shortNameUp, mode);
        // std::shared_ptr<FlexRadio6000> newInstance = std::shared_ptr<FlexRadio6000> (evilPointer);
        std::shared_ptr<FlexRadio6000> newInstance = std::make_shared<FlexRadio6000>(addr, longName, shortNameUp, mode);
        s_FlexRadio6000Instances[id_pair] = std::weak_ptr<FlexRadio6000>(newInstance);
        return newInstance;
    }

    //increase radio subscriber count;

    std::shared_ptr<FlexRadio6000> objectInstance = std::shared_ptr<FlexRadio6000>(s_FlexRadio6000Instances[id_pair]);
    return objectInstance;
}

int FlexRadio6000::Disconnect(const std::string &address, const std::string &shortName)
{

    std::lock_guard<std::mutex> guard(s_lock);
    std::pair<std::string, std::string> id_pair(address, shortName);
    auto instance_iterator = s_FlexRadio6000Instances.find(id_pair);
    if (instance_iterator == s_FlexRadio6000Instances.end())
    {
        std::cout << "Could not find radio at address " << address << std::endl;
        return EXIT_FAILURE;
    }
    else
    {
        //destroy object:
        // delete s_FlexRadio6000Instances[id_pair];
        // unsubscribe... here:

        //delete pointer to object:
        std::cout << "Radio at address " << id_pair.first << ":" << id_pair.second << "deletion not supported" << std::endl;
        // s_FlexRadio6000Instances.erase(id_pair);
        // std::cout << "Radio at address " << address << " erased" << std::endl;
    }
    return 0;
}

FlexRadio6000::FlexRadio6000(const sockaddr_in &addr, const std::string &longName,
                             const std::string &shortName, const std::string &mode) : m_addr(addr),
                                                                                      m_longName(longName),
                                                                                      m_shortName(shortName),
                                                                                      m_mode(mode)
{
    // waveform_set_log_level(WF_LOG_INFO);
    // waveform_set_log_level(WF_LOG_TRACE);

    //create the radio & waveform:

    m_radio = waveform_radio_create(&m_addr);
    std::cout << "starting radio, not this will fail (almost silently) if bad address" << std::endl;
    printf("radio pointer %p \n", this->m_radio);

    //convert string names to char* names:

    char pc_long_name[m_longName.size() + 1];
    char pc_short_name[m_shortName.size() + 1];
    char pc_mode[m_mode.size() + 1];
    strncpy(pc_long_name, m_longName.c_str(), sizeof(pc_long_name));
    strncpy(pc_short_name, m_shortName.c_str(), sizeof(pc_short_name));
    strncpy(pc_mode, m_mode.c_str(), sizeof(pc_mode));

    m_waveform = waveform_create(this->m_radio, pc_long_name, pc_short_name, pc_mode, "1.0.0");
    std::cout << "created waveform: " << &m_waveform << std::endl;

    waveform_set_context(m_waveform, this); //this is unused...

    //register status callback.
    this->register_status_cb("radio", m_statusTree.s_statusParser, &m_statusTree);
    std::cout << "registered status tree: " << &m_statusTree << std::endl;

    this->init_pacing_cb();
    // print status info for debug:
    // this->register_status_cb("radio",this->s_printCmd_cb,&m_statusTree);
    this->register_state_cb(s_processState, this);
    // this->register_unknown_data_cb(s_processUnknown, this);

    //open timestamp debug file stream:
    m_ts_debugStream.open("ts_debug.txt", std::ios::out | std::ios::trunc);

    //register state locks, some other blocks may selectively use some of these:
    //wait for initilization to be complete:
    m_lockMap.get_lock("initLock")->stateLckCntRegister(true);
    //wait for radio to get started:
    m_lockMap.get_lock("startedLock")->stateLckCntRegister();

    //wait for activation status to be updated:
    m_lockMap.get_lock("activationDelayLock")->stateLckCntRegister();
    //wait for all blocks to perform activation actions:
    m_lockMap.get_lock("activationLock")->stateLckCntRegister();

    m_kill_thread = new std::thread(
        [this]()
        {
            //register the status callbacks...
            //wait for initialization.
            m_lockMap.get_lock("initLock")->waitOnStateLck();
            //init complete start radio:
            waveform_radio_start(m_radio);
            std::this_thread::sleep_for(std::chrono::milliseconds(s_start_wait_time));
            //sub to everything... might be excessive...
            this->send_api_command(NULL, NULL, "sub client all");
            this->send_api_command(NULL, NULL, "sub tx all");
            this->send_api_command(NULL, NULL, "sub atu all");
            this->send_api_command(NULL, NULL, "sub amplifier all");
            this->send_api_command(NULL, NULL, "sub meter all");
            this->send_api_command(NULL, NULL, "sub pan all");
            this->send_api_command(NULL, NULL, "sub slice all");
            this->send_api_command(NULL, NULL, "sub gps all");
            this->send_api_command(NULL, NULL, "sub audio_stream all");
            this->send_api_command(NULL, NULL, "sub cwx all");
            this->send_api_command(NULL, NULL, "sub xvtr all");
            this->send_api_command(NULL, NULL, "sub memories all");
            this->send_api_command(NULL, NULL, "sub daxiq all");
            this->send_api_command(NULL, NULL, "sub dax all");
            this->send_api_command(NULL, NULL, "sub usb_cable all");
            // this->send_api_command(NULL, NULL,"sub tnf all");
            this->send_api_command(NULL, NULL, "sub spot all");
            this->send_api_command(NULL, NULL, "sub rapidm all");
            //let everyone else continue, radio started and subbed

            //auto start for debug:
            if (m_config.autostart)
            {
                this->send_api_command(NULL, NULL, "slice r 0");
                //cannot create slice in mode for some reason...
                this->send_api_command(NULL, NULL, "slice create");
                std::cout << "FlexRadio6000 auto-starting waveform on slice 0" << std::endl;
                if (!(m_config.autoStart_rxAnt.compare("")==0)){
                    std::cout << "FlexRadio6000 auto setting waveform RX ANT to  "<<m_config.autoStart_rxAnt << std::endl;
                    this->send_api_command(NULL, NULL, "slice s 0 rxant="+ m_config.autoStart_rxAnt);
                }
                if (!(m_config.autoStart_txAnt.compare("")==0)){
                    std::cout << "FlexRadio6000 auto setting waveform TX ANT to  "<<m_config.autoStart_txAnt << std::endl;
                    this->send_api_command(NULL, NULL, "slice s 0 txant="+ m_config.autoStart_txAnt);
                }
                if (m_config.autoStart_freq!=0){
                    std::string freq_str = boost::lexical_cast<std::string>(m_config.autoStart_freq);
                    std::cout << "FlexRadio6000 auto tuneing waveform to "<<freq_str<<" MHz" << std::endl;
                    this->send_api_command(NULL, NULL, "slice tune 0 "+ freq_str);
                }
                std::cout << "FlexRadio6000 auto activating waveform "<<m_shortName << std::endl;
                this->send_api_command(NULL, NULL, "slice s 0 mode=" + m_shortName);
                

            }

            // this->send_api_command(NULL, NULL,"slice s 0 freq=10");
            m_lockMap.get_lock("startedLock")->waitOnStateLck();

            //subscribe to all here:

            //wait for activation status to be updated:
            std::cout << "FlexRadio6000 instance waiting for radio to exit" << std::endl;
            this->waveform_radio_wait(m_radio);
            std::cout << "FlexRadio6000 instance radio exiting" << std::endl;
            //this is so bad... but easy... but bad...:
            //if you are reading it you should fix it by having the gnuradio
            //subscibers to the shared radio object check if it exists, or have this guy set a flag or something better than this...
            //that being said there might be a issue if the radio exits mid function, I have no idea.
            assert(0);
        });
}

FlexRadio6000::~FlexRadio6000()
{
    waveform_destroy(m_waveform);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    waveform_radio_destroy(m_radio);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void FlexRadio6000::printCmd(unsigned int code, char *message, void *arg)
{
    std::cout << "**********************************\n"
              << "cmd resp from "
              << "something "
              << " message: " << message << " code: " << std::hex << code << "\n"
              << "**********************************\n"
              << std::endl;
    // std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void FlexRadio6000::printCmd_cb(const char *cb_name, unsigned int argc, char *argv[])
{

    std::cout << "**********************************\n"
              << "Begin Waveform " << cb_name << " "
              << "\n"
              << "**********************************\n";

    for (size_t i = 0; i < argc; i++)
    {
        std::cout << strlen(argv[i]) << std::endl;
        std::string s(argv[i]);
        std::cout << argv << "\n";
    }
    std::cout << "**********************************\n"
              << "End Waveform " << cb_name << " "
              << "\n"
              << "**********************************\n"
              << std::endl;
}

void FlexRadio6000::processRX(struct waveform_vita_packet *packet, size_t packet_size)
{
    std::cout << m_shortName << " got process rx" << std::endl;
}

void FlexRadio6000::processTX(struct waveform_vita_packet *packet, size_t packet_size)
{
    std::cout << m_shortName << " got process tx" << std::endl;
}

void FlexRadio6000::processUnknown(struct waveform_vita_packet *packet, size_t packet_size)
{
    std::cout << m_shortName << " got process unknown" << std::endl;
}

void FlexRadio6000::processState(enum waveform_state state)
{
    switch (state)
    {
    case ACTIVE:
        std::cout << "radio object got ACTIVE cb" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        m_lockMap.get_lock("activationDelayLock")->waitOnStateLck();
        m_lockMap.get_lock("activationLock")->waitOnStateLck();
        std::cout << "ACTIVE" << std::endl;
        break;
    case INACTIVE:
        std::cout << "INACTIVE" << std::endl;
        break;
    case PTT_REQUESTED:
        std::cout << "PTT_REQUESTED" << std::endl;
        break;
    case UNKEY_REQUESTED:
        std::cout << "UNKEY_REQUESTED" << std::endl;
        break;
    default:
        std::cout << "UNRECOGNIZED STATE" << std::endl;
        break;
    }
}

int FlexRadio6000::enforceMaxInstances(std::string name, int maxInst)
{
    std::lock_guard<std::mutex> lock(m_highlanderMutex);

    auto instance_iterator = m_highlanderMap.find(name);
    if (instance_iterator == m_highlanderMap.end())
    {

        //add name to map:
        int currInst = 1;
        std::pair<int, int> pair(maxInst, currInst);
        m_highlanderMap[name] = pair;
        return 0;
    }
    else
    {
        //instance already exists is it below the max?
        if (m_highlanderMap[name].first < m_highlanderMap[name].second + 1)
        {
            //fail, too many duplicates:
            std::cerr << "Too many instances of " << name << std::endl;
            assert(0);
        }
        else
        {
            m_highlanderMap[name].second = m_highlanderMap[name].second + 1;
            return 0;
        }
    }
}

FlexRadio6000::stateLock::stateLock() : m_stateLck_numWaiting(0),
                                        m_stateLck_numSub(0),
                                        m_stateLck_numThruLck(0)
{
}
FlexRadio6000::stateLock::~stateLock()
{
}

void FlexRadio6000::stateLock::stateLckCntRegister(bool increment_min_subscriber_cnt)
{
    std::lock_guard<std::mutex> lock(m_stateLck_mutex);
    m_stateLck_numSub++;
    if (increment_min_subscriber_cnt)
    {
        m_stateLck_minSubNum++;
    }
}

void FlexRadio6000::stateLock::stateLckCntDeregister(bool decrement_min_subscriber_cnt)
{
    std::lock_guard<std::mutex> lock(m_stateLck_mutex);
    m_stateLck_numSub--;
    if (decrement_min_subscriber_cnt)
    {
        m_stateLck_minSubNum--;
    }
}

void FlexRadio6000::stateLock::waitOnStateLck()
{
    std::unique_lock<std::mutex> lock(m_stateLck_mutex);

    if (!m_stateLck_ready)
    {
        m_stateLck_readyConditionVar.wait(lock, [&]()
                                          { return m_stateLck_numWaiting >= m_stateLck_numSub && m_stateLck_numSub > m_stateLck_minSubNum; });
    }

    // increment the number done
    m_stateLck_numWaiting++;
    if (m_stateLck_numWaiting < m_stateLck_numSub || m_stateLck_numSub <= m_stateLck_minSubNum)
    {
        m_stateLck_conditionVar.wait(lock, [&]()
                                     { return m_stateLck_numWaiting >= m_stateLck_numSub && m_stateLck_numSub > m_stateLck_minSubNum; });
    }
    else
    {
        //first guy through:
        m_stateLck_ready = false;
        m_stateLck_conditionVar.notify_all();
    }

    m_stateLck_numThruLck++;

    if (m_stateLck_numThruLck >= m_stateLck_numSub)
    {
        //all subscribers passed through, reset lock:
        m_stateLck_numWaiting = 0;
        m_stateLck_numThruLck = 0;
        //set condition ready:
        m_stateLck_ready = true;
        m_stateLck_conditionVar.notify_all();
    }
}

std::shared_ptr<FlexRadio6000::stateLock> FlexRadio6000::stateLockMap::get_lock(std::string lockName)
{
    std::lock_guard<std::mutex> lock(m_lockMapMutex);
    auto instance_iterator = m_lockMap.find(lockName);
    if (instance_iterator == m_lockMap.end())
    {
        //lock not found, create it:
        m_lockMap[lockName] = std::make_shared<FlexRadio6000::stateLock>();
    }
    return m_lockMap[lockName];
}

//pacing callback implementation:
void FlexRadio6000::init_pacing_cb()
{
    register_rx_data_cb(s_rxCallback, this);
    register_tx_data_cb(s_txCallback, this);
}

void FlexRadio6000::pacingCallback(
    FlexRadio6000::waveform_vita_packet *packet,
    size_t packet_size, paceMode mode)
{
    //packet size includes timestamp (2 uint64), classid (uint64), streamid (uint32) and payload (varies, multiple of 4 bytes... usually):
    //get pc time:
    m_packetNum++;

    std::chrono::nanoseconds ts_ns;
    if (m_config.tsMode == config::timestampMode_t::PC)
    {
        auto pc_tp = std::chrono::system_clock::now();
        ts_ns = pc_tp.time_since_epoch();
    }
    else if (m_config.tsMode == config::timestampMode_t::RADIO)
    {
        timespec ts;
        get_packet_ts(packet, &ts);
        ts_ns = std::chrono::nanoseconds(ts.tv_nsec);
        ts_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(ts.tv_sec)) + ts_ns;
    }

    //in the future packet_len might change...(probably be a multiple of the input for transmit)
    //due to different sample rates between audio and IQ streams.
    uint16_t packet_len = get_packet_len(packet);
    float *packet_data = get_packet_data(packet);

    {

        // if (m_startOfDebug)
        // {
        //     m_nsStartOffest = ts_ns.count();
        //     m_startOfDebug = false;
        // }
        // //debug timestamps stuff:
        // // //recieve other data packets:
        // // uint64_t classid = get_class_id(packet);
        // // uint32_t streamid = get_stream_id(packet);
        // // uint16_t packet_len = get_packet_len(packet);

        // //calculate diff between last call and current call:
        // std::chrono::nanoseconds callback_period = ts_ns - m_last_cb_time;

        // //positive means current timestamp is late, negative means it is early:
        // std::chrono::nanoseconds ts_diff_from_est = ts_ns - m_est_next_cb_time;
        // // std::cout << "ts error: "<<ts_diff_from_est.count()<<std::endl;

        // m_ts_debugStream << ts_ns.count() - m_nsStartOffest << std::endl;

        // // //save off member vals:
        // // //save last radio time:
        // // m_last_cb_time = ts_ns;

        // // //estimate next packet time:
        // double packet_duration = packet_len / 2 / m_config_fs;
        // m_est_next_cb_time = ts_ns + std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(packet_duration));

        // // if (std::abs(ts_diff_from_est.count()) > 1500000)
        // {
        //     std::cout << "discontinuity? ts packet # " << m_packetNum << " late by " << ts_diff_from_est.count() << " ns." << std::endl;

        // std::cout << "got mode " << mode << " packet with ts = " << ts_ns.count() << " nsec packet size = " << packet_size << "\n";
        //     // std::cout << "pc_time " << sec.count() << " sec and " << nsec.count() << std::endl;
        //     // std::cout << "stream id: " << std::hex << streamid << " class id: " << classid << std::dec << std::endl;
        // }
        // // std::cout << "packet_size bytes: "<< packet_size << "packet_size 32 bit (4 byte chunks): "<<get_packet_len(packet)<<std::endl;
    }

    {
        std::lock_guard<std::mutex> lock(m_callback_list_mutex);
        for (auto callback : m_rx_callbackVec)
        {
            callback.first(ts_ns, mode, packet_len, packet_data, callback.second);
        }

        for (auto callback : m_tx_callbackVec)
        {
            callback.first(ts_ns, mode, packet_len, callback.second);
        }


    }
}

int FlexRadio6000::deregister_pacingTX_cb(pacingTX_cb_t cb, void *arg)
{
    std::lock_guard<std::mutex> lock(m_callback_list_mutex);
    std::pair<pacingTX_cb_t, void *> pair(cb, arg);

    auto it = std::find(m_tx_callbackVec.begin(), m_tx_callbackVec.end(), pair);
    if (it != m_tx_callbackVec.end())
    {
        m_tx_callbackVec.erase(it);
    }
    return 0;
}

int FlexRadio6000::deregister_pacingRX_cb(pacingRX_cb_t cb, void *arg)
{
    std::lock_guard<std::mutex> lock(m_callback_list_mutex);
    std::pair<pacingRX_cb_t, void *> pair(cb, arg);

    auto it = std::find(m_rx_callbackVec.begin(), m_rx_callbackVec.end(), pair);
    if (it != m_rx_callbackVec.end())
    {
        m_rx_callbackVec.erase(it);
    }
    return 0;
}
