/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ISCtrl.h
/// @brief contains class ISController
///
/// Created by wangqi on 2016/11/21.
/// this file contains the definination of class ISController
///

#ifndef ISCTRL_H_
#define ISCTRL_H_

#include <mutex>

#include "ISTask.h"

#include "ama_audioTypes.h"
#include "ama_types.h"
#include "PlaySoundThread.h"
#include "RadarSoundThread.h"

#define IS_APPID            0xFF        //informingSound appid = 0xff
#define IVI_IS_AMPID        (AMPID_APP(E_SEAT_IVI,IS_APPID))
#define LRSE_IS_AMPID       (AMPID_APP(E_SEAT_RSE1,IS_APPID))
#define RRSE_IS_AMPID       (AMPID_APP(E_SEAT_RSE2,IS_APPID))
#define GET_COM_CONNECTID(connectID)               ((connectID | ((IS_APPID) << 16)) & (0xF0FFFFFF))

using std::shared_ptr;
using std::string;
using std::map;

/// @class ISControllerkDispatcher. and
/// handle the actuall work about InformingSound
class ISController
{
public:
    static ISController* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static ISController instance;
        return &instance;
    }
    ~ISController();

    void doTask(shared_ptr<Task> task);

private:
    ISController();

    void createAllPlaySoundThread(void);
    void stopAllPlaySoundThread(void);
    ama_connectID_t setAmpidForConnectID(ama_connectID_t connectID);
    shared_ptr<PlaySoundThread> findPlaySoundThreadFromMapByConnectID(ama_connectID_t connectID);

    void requestAudioFocus(ama_connectID_t connectID);
    void releaseAudioFocus(ama_connectID_t connectID);

    void OnGainAudioFocus(ama_connectID_t connectID);
    void OnLossAudioFocus(ama_connectID_t connectID);

    void reqFeedbackSound(int volumeType,int feedbackVol);
    ama_audioDeviceType_t getDeviceType(int volumeType);
private:
    std::map<ama_connectID_t,shared_ptr<PlaySoundThread>> mAllPlaySoundThreadMap;
    shared_ptr<RadarSoundThread> mpRadarSoundThread;
};

#endif //ISCTRL_H_
