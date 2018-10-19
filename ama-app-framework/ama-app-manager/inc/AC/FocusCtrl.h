/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file FocusCtrl.h
/// @brief contains class FocusController
///
/// Created by wangqi on 2016/6/16.
/// this file contains the definination of class FocusController
///

#ifndef FOCUSCTRL_H_
#define FOCUSCTRL_H_

#include <mutex>
#include <vector>
#include <map>

#include "AMGRLogContext.h"
#include "ama_types.h"
#include "ama_audioTypes.h"

using std::vector;
using std::map;

#define DEF_COMPARE_WITH_LATERCOMER         (0)
#define DEF_COMPARE_WITH_EARLYCOMER         (1)

/// @class FocusController
/// @brief this class is the abstraction of Audio module
///
/// this class receive connect request from source and
/// decide which source connect the sink
class FocusController
{
public:
    static FocusController* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static FocusController instance;
        return &instance;
    }
    ~FocusController();

    void requestAudioDeviceFocus(ama_connectID_t connectID);
    void releaseAudioDeviceFocus(ama_connectID_t connectID);
    void refreshFocusStaWhenSettingChanged();

    void debugShowFocusSta(void);

    bool isVolInfoHadFocus(ama_volumeInfo_t volInfo);
private:
    FocusController();

    void handleConCtrlForReqMic(ama_connectID_t connectID);
    void handleConCtrlForRelMic(ama_connectID_t connectID);

    void handleConCtrlForReqSpeaker(ama_connectID_t connectID);
    void handleConCtrlForRelSpeaker(ama_connectID_t connectID);

    void handleConCtrlForReqLHeadset(ama_connectID_t connectID);
    void handleConCtrlForRelLHeadset(ama_connectID_t connectID);

    void handleConCtrlForReqRHeadset(ama_connectID_t connectID);
    void handleConCtrlForRelRHeadset(ama_connectID_t connectID);

    void tellConnectionToHandleNewFocusSta(ama_connectID_t connectID,ama_focusSta_t newFocusSta);
    void printfFocusMap(std::map<ama_connectID_t,ama_focusSta_t> focusStaMap);

    void addReqIntoFocusVec(ama_connectID_t connectID,std::vector<ama_connectID_t> &focusVec);
    void removeReqFromFocusVec(ama_connectID_t connectID,std::vector<ama_connectID_t> &focusVec);
    void removeStaFromFocusMap(ama_connectID_t connectID,map<ama_connectID_t,ama_focusSta_t> &focusStaMap);
    void refreshFocusState(std::vector<ama_connectID_t> &focusVec,map<ama_connectID_t,ama_focusSta_t> &focusStaMap);
    void removeLossFocusFromVecAndMap(std::vector<ama_connectID_t> &focusVec,map<ama_connectID_t,ama_focusSta_t> &focusStaMap);
    ama_focusSta_t getFocusStaInVec(ama_connectID_t connectID,vector<ama_connectID_t> &focusVec);
    ama_focusSta_t getFocusStaCompareWithVec(ama_connectID_t connectID,vector<ama_connectID_t> &focusVec);
    ama_focusSta_t getConSta(ama_connectID_t connectID,ama_connectID_t cmpConnectID,int compareMode);

private:
    std::map<ama_connectID_t,ama_focusSta_t> mSpeakerFocusMap;
    std::map<ama_connectID_t,ama_focusSta_t> mLeftHeadsetFocusMap;
    std::map<ama_connectID_t,ama_focusSta_t> mRightHeadsetFocusMap;
    std::map<ama_connectID_t,ama_focusSta_t> mMicFocusMap;

    std::vector<ama_connectID_t> mSpeakerFocusVec;
    std::vector<ama_connectID_t> mLeftHeadsetFocusVec;
    std::vector<ama_connectID_t> mRightHeadsetFocusVec;
    std::vector<ama_connectID_t> mMicFocusVec;

    ama_connectID_t mLocalVRConID;
    ama_focusSta_t mLoaclVRFocusSta;
};

//-------------FocusTable[connectIDOfgetSta][connectIDofCompare] for mix sound between phone and navi TTS
//-------------this table define compare with the connectIDofCompare which come after the connectIDOfgetSta
static uint8_t FocusCmpWithLatercomerTable[E_FOCUS_TYPE_MAX][E_FOCUS_TYPE_MAX] =
{//------------/RADAR/-----/NOTIFI/-----/SOFTKEY/------/MEDIA/------/SMSTTS/----------/VRTTS/---------/NAVITTS/------/PHONE/----------/BICALL/----------/ECALL/--------/POWEROFF*/
/*RADAR*/   E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*NOTIFI*/  E_FOCUSLOSE  ,E_FOCUSLOSE  ,E_FOCUSLOSE  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*SOFTKEY*/ E_FOCUSLOSE  ,E_FOCUSLOSE  ,E_FOCUSLOSE  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*MEDIA*/   E_GAIN_FADE  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOCUSLOSE  ,E_LOSE_TRANSIENT,E_LOSE_TRANSIENT,E_GAIN_FADE  ,E_LOSE_TRANSIENT,E_LOSE_TRANSIENT,E_LOSE_TRANSIENT,E_LOSE_TRANSIENT,
/*SMSTTS*/  E_FOCUSLOSE  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOCUSLOSE  ,E_FOCUSLOSE     ,E_FOCUSLOSE     ,E_FOUCSGAIN  ,E_FOCUSLOSE     ,E_FOCUSLOSE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*VRTTS*/   E_FOCUSLOSE  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOCUSLOSE     ,E_FOUCSGAIN  ,E_FOCUSLOSE     ,E_FOCUSLOSE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*NAVITTS*/ E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOCUSLOSE     ,E_FOCUSLOSE     ,E_FOCUSLOSE  ,E_GAIN_FADE     ,E_GAIN_FADE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*PHONE*/   E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOCUSLOSE     ,E_FOCUSLOSE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*BICALL*/  E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*ECALL*/   E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*POWEROFF*/E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN,     E_FOUCSGAIN
};

//-------------FocusTable[connectIDOfgetSta][connectIDofCompare] for mix sound between phone and navi TTS
//-------------this table define compare with the connectIDofCompare which come before the connectIDOfgetSta
static uint8_t FocusCmpWithEarlycomerTable[E_FOCUS_TYPE_MAX][E_FOCUS_TYPE_MAX] =
{//------------/RADAR/-----/NOTIFI/-----/SOFTKEY/------/MEDIA/------/SMSTTS/-------/VRTTS/---------/NAVITTS/------/PHONE/----------/BICALL/---------/ECALL/---------/POWEROFF*/
/*RADAR*/   E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*NOTIFI*/  E_FOCUSLOSE  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*SOFTKEY*/ E_FOCUSLOSE  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*MEDIA*/   E_GAIN_FADE  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_LOSE_TRANSIENT,E_GAIN_FADE  ,E_LOSE_TRANSIENT,E_LOSE_TRANSIENT,E_LOSE_TRANSIENT,E_LOSE_TRANSIENT,
/*SMSTTS*/  E_FOCUSLOSE  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOCUSLOSE     ,E_FOUCSGAIN  ,E_FOCUSLOSE     ,E_FOCUSLOSE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*VRTTS*/   E_FOCUSLOSE  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOCUSLOSE     ,E_FOCUSLOSE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*NAVITTS*/ E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOCUSLOSE  ,E_FOCUSLOSE     ,E_FOUCSGAIN  ,E_GAIN_FADE     ,E_GAIN_FADE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*PHONE*/   E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOCUSLOSE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*BICALL*/  E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOCUSLOSE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*ECALL*/   E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*POWEROFF*/E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN,     E_FOUCSGAIN
};

//-------------FocusTableNoMix[connectIDOfgetSta][connectIDofCompare] for no mix between phone and navi TTS
//-------------this table define compare with the connectIDofCompare which come after the connectIDOfgetSta
static uint8_t FocusCmpWithLatercomerTableNoMix[E_FOCUS_TYPE_MAX][E_FOCUS_TYPE_MAX] =
{//------------/RADAR/-----/NOTIFI/-----/SOFTKEY/------/MEDIA/------/SMSTTS/----------/VRTTS/---------/NAVITTS/------/PHONE/----------/BICALL/---------/ECALL/---------/POWEROFF*/
/*RADAR*/   E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*NOTIFI*/  E_FOCUSLOSE  ,E_FOCUSLOSE  ,E_FOCUSLOSE  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*SOFTKEY*/ E_FOCUSLOSE  ,E_FOCUSLOSE  ,E_FOCUSLOSE  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*MEDIA*/   E_GAIN_FADE  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOCUSLOSE  ,E_LOSE_TRANSIENT,E_LOSE_TRANSIENT,E_GAIN_FADE  ,E_LOSE_TRANSIENT,E_LOSE_TRANSIENT,E_LOSE_TRANSIENT,E_LOSE_TRANSIENT,
/*SMSTTS*/  E_FOCUSLOSE  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOCUSLOSE  ,E_FOCUSLOSE     ,E_FOCUSLOSE     ,E_FOUCSGAIN  ,E_FOCUSLOSE     ,E_FOCUSLOSE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*VRTTS*/   E_FOCUSLOSE  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOCUSLOSE     ,E_FOUCSGAIN  ,E_FOCUSLOSE     ,E_FOCUSLOSE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*NAVITTS*/ E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOCUSLOSE     ,E_FOCUSLOSE     ,E_FOCUSLOSE  ,E_FOCUSLOSE     ,E_FOCUSLOSE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*PHONE*/   E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOCUSLOSE     ,E_FOCUSLOSE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*BICALL*/  E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*ECALL*/   E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*POWEROFF*/E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN,     E_FOUCSGAIN
};

//-------------FocusTableNoMix[connectIDOfgetSta][connectIDofCompare] for no mix between phone and navi TTS
//-------------this table define compare with the connectIDofCompare which come before the connectIDOfgetSta
static uint8_t FocusCmpWithEarlycomerTableNoMix[E_FOCUS_TYPE_MAX][E_FOCUS_TYPE_MAX] =
{//------------/RADAR/-----/NOTIFI/-----/SOFTKEY/------/MEDIA/------/SMSTTS/-------/VRTTS/---------/NAVITTS/------/PHONE/----------/BICALL/---------/ECALL/---------/POWEROFF*/
/*RADAR*/   E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*NOTIFI*/  E_FOCUSLOSE  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*SOFTKEY*/ E_FOCUSLOSE  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*MEDIA*/   E_GAIN_FADE  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_LOSE_TRANSIENT,E_GAIN_FADE  ,E_LOSE_TRANSIENT,E_LOSE_TRANSIENT,E_LOSE_TRANSIENT,E_LOSE_TRANSIENT,
/*SMSTTS*/  E_FOCUSLOSE  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOCUSLOSE     ,E_FOUCSGAIN  ,E_FOCUSLOSE     ,E_FOCUSLOSE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*VRTTS*/   E_FOCUSLOSE  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOCUSLOSE     ,E_FOCUSLOSE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*NAVITTS*/ E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOCUSLOSE  ,E_FOCUSLOSE     ,E_FOUCSGAIN  ,E_FOCUSLOSE     ,E_FOCUSLOSE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*PHONE*/   E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOCUSLOSE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*BICALL*/  E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOCUSLOSE     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*ECALL*/   E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOCUSLOSE,     E_FOUCSGAIN,
/*POWEROFF*/E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN  ,E_FOUCSGAIN     ,E_FOUCSGAIN     ,E_FOUCSGAIN,     E_FOUCSGAIN
};
#endif //FOCUSCTRL_H_
