/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Amplifier.h
/// @brief contains class Amplifier
///
/// Created by wangqi on 2017/2/6.
/// this file contains the definination of class Amplifier
///

#ifndef AMPLIFIER_H_
#define AMPLIFIER_H_

#include <mutex>
#include <list>

#include "AMGRLogContext.h"
#include "ama_types.h"
#include "ama_audioTypes.h"
#include "AMClient.h"

#define UNSETTED_DATA           0xff
#define NO_MEDIA_CONNECTID      0xffffffff

using std::list;

enum ama_ampSourceSta_e{
    E_MEDIA_STATE = 0x00,
    E_TUNER_STATE = 0x01,
    E_PHONE_STATE = 0x02,
    E_MAX_SOURCE_STATE = 0x3,
};

/// @class Amplifier
/// @brief this class is the abstraction of Audio module
///
/// this class is a client of Amplifier.
class Amplifier:
    public ConnectStaChangedObserver
{
public:
    static Amplifier* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static Amplifier instance;
        return &instance;
    }
    virtual ~Amplifier();

    void OnConnectStaChanged(ama_connectID_t connectID,bool isConnect);

    void setAMPWhenSettingChanged();

    void setAMPMuteState(bool isMute);

    void initDefaultMode(void);
private:
    Amplifier();

    void setAMPByCurSourceState();
    void setAMPForMediaSrcSta();
    void setAMPForTunerSrcSta();
    void setAMPForPhoneSrcSta();

    void setCallMode_HQEV();//0,0,5
    void setNormalMode_HQEV();//set,set,1
    void setCallMode();//0,0,7
    void setStereoMode();//set,set,1
    void setStereoDriverMode();//set,set,2
    void setStereoRearMode();//set,set,3
    void setCenterPointMode();//set,set,5

    int mCurBalanceSet;
    int mCurFaderSet;
    ama_ampMode_t mCurAMPMode;
    ama_ampSourceSta_e mCurAmpSrcSta;
    bool mIsMute;

    std::list<ama_connectID_t> mCurConList;
};

// new Amplifier set Table(HS5)
/*******************************************************************************
seat&StreamType         soundEffect     balance     fader       AMP-Mode
Call(BT/B/I/E-Call)     ever            0           0           TELEPHONE

tuner                   stereo          audioSet    audioSet    STEREO_ALL
tuner                   stereo-driver   audioSet    audioSet    STEREO_DRIVER
tuner                   stereo-rear     audioSet    audioSet    STEREO_REAR
tuner                   centerPoint     audioSet    audioSet    STEREO_ALL

media(but tuner)        stereo          audioSet    audioSet    STEREO_ALL
media(but tuner)        stereo-driver   audioSet    audioSet    STEREO_DRIVER
media(but tuner)        stereo-rear     audioSet    audioSet    STEREO_REAR
media(but tuner)        centerPoint     audioSet    audioSet    CENTER_POINT

ever                    do nothing      do nothing  do nothing
*******************************************************************************/
// new Amplifier set Table(HS7)
/*******************************************************************************
seat&StreamType         soundEffect     balance     fader       AMP-Mode
Call(BT/B/I/E-Call)     ever            0           0           TELEPHONE

tuner                   stereo          audioSet    audioSet    STEREO_ALL
tuner                   stereo-driver   audioSet    audioSet    STEREO_DRIVER
tuner                   stereo-rear     audioSet    audioSet    STEREO_REAR
tuner                   centerPoint     audioSet    audioSet    STEREO_ALL

media(but tuner)        stereo          audioSet    audioSet    STEREO_ALL
media(but tuner)        stereo-driver   audioSet    audioSet    STEREO_DRIVER
media(but tuner)        stereo-rear     audioSet    audioSet    STEREO_REAR
media(but tuner)        centerPoint     audioSet    audioSet    CENTER_POINT

ever                    do nothing      do nothing  do nothing
*******************************************************************************/
// new Amplifier set Table(HQEV)
/*******************************************************************************
seat&StreamType         soundEffect     balance     fader       AMP-Mode
Call(BT/B/I/E-Call)     ever            0           0           5

ever(but call)          ever            audioSet    audioSet    1
*******************************************************************************/

#endif//AMPLIFIER_H_
