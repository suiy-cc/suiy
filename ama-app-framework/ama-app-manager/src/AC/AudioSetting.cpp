/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AudioSetting.cpp
/// @brief contains the implementation of class AudioSetting
///
/// Created by wangqi on 2016/11/30.
/// this file contains the implementation of class AudioSetting
///

#include "AudioSetting.h"
#include "AudioCtrl.h"
#include "ama_audioTypes.h"
#include "TaskDispatcher.h"
#include "Speaker.h"
#include "LCDManager.h"
#include "ama_setTypes.h"
#include "cs_task.h"
#include "ama_stateTypes.h"
#include "Capi.h"
#include "FocusCtrl.h"
#include "AppDB.h"

#include <string>
#include <list>

using std::string;
using std::list;

bool isGainForSpeedChanged(int ABS_VehicleSpeed,int EMS_EngineSpeed)
{
    uint8_t newGainForSpeed  = 0;
    if(EMS_EngineSpeed > CRITICAL_ENGINE_SPEED){
        if((ABS_VehicleSpeed>0)&&(ABS_VehicleSpeed<MAX_VEHICLE_SPEED)){
            newGainForSpeed = gainForSpeedTable[ABS_VehicleSpeed];
        }else if(ABS_VehicleSpeed >= MAX_VEHICLE_SPEED){
            newGainForSpeed = 12;
        }
    }

    if(newGainForSpeed != gainForSpeed){
        gainForSpeed = newGainForSpeed;
        return true;
    }else{
        return false;
    }
}

AudioSetting::AudioSetting()
:mIsEnableFeedback(true)
,mABS_VehicleSpeed(0)
,mVoldbAddBySpeed(0)
,mIsMediaMuteByPower(false)
{
    logVerbose(AC,"AudioSetting::AudioSetting()-->IN");

    AppDatabaseEx db;
    if (db.connectDB("SYS_SETTING") != ama_DB_Error_e::E_DB_OK)
    {
        logDebug(AC,"connect SYS_SETTING DB failed!");
        loadVolSetFromDB(NULL);
    }
    else
    {
        logDebug(AC,"load vol value from SYS_SETTING DB");
        loadVolSetFromDB(&db);
    }

    logVerbose(AC,"AudioSetting::AudioSetting()-->OUT");
}

AudioSetting::~AudioSetting()
{
    logVerbose(AC,"AudioSetting::~AudioSetting()-->IN");
    logVerbose(AC,"AudioSetting::~AudioSetting()-->OUT");
}

bool AudioSetting::isIVIMediaMute(void)
{
    logVerbose(AC,"AudioSetting::isIVIMediaMute()-->IN");
    logVerbose(AC,"AudioSetting::isIVIMediaMute()-->OUT");

    return (mIsMediaMuteByPower||mIsMediaMuteMap[E_IVI_SPEAKER]);
}

void AudioSetting::setMediaMuteByPowerSta(bool isMediaMute)
{
    mIsMediaMuteByPower = isMediaMute;
    notifySettingChanged();
}

int AudioSetting::getVolSet(ama_connectID_t connectID,ama_focusSta_t focusSta, bool &isVolGainDown)
{
    logVerbose(AC,"AudioSetting::getVolSet()-->IN");

    logInfo(AC,"focusSta = ",ToHEX(focusSta));
	bool isTmpVolGainDown = false;
    int volSet = MUTE_VOL_STEP;
    bool isSysMediaMute = mIsMediaMuteMap[GET_DEVICETYPE_FROM_CONNECTID(connectID)];
    isSysMediaMute = mIsMediaMuteByPower || isSysMediaMute;
    ama_volumeInfo_t volumeInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NAVITTS,GET_DEVICETYPE_FROM_CONNECTID(connectID));
    bool isMixWithNavi = FocusController::GetInstance()->isVolInfoHadFocus(volumeInfo);
    volumeInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_FRONTRADAR,GET_DEVICETYPE_FROM_CONNECTID(connectID));
    bool isMixWithFrontRadar = FocusController::GetInstance()->isVolInfoHadFocus(volumeInfo);
    volumeInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_REARRADAR,GET_DEVICETYPE_FROM_CONNECTID(connectID));
    bool isMixWithRearRadar = FocusController::GetInstance()->isVolInfoHadFocus(volumeInfo);

    bool isMixWithRadar = false;
    if(isMixWithFrontRadar || isMixWithRearRadar){
        isMixWithRadar = true;
    }else{
        isMixWithRadar = false;
    }

	int naviVolSet = mVolSetMap[CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NAVITTS,E_IVI_SPEAKER)];
	int radarVolSet = mVolSetMap[CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_FRONTRADAR,E_IVI_SPEAKER)];
	int phoneVolSet = mVolSetMap[CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_PHONE,E_IVI_SPEAKER)];

    if(E_FOUCSGAIN == focusSta){
        if((E_VOLUME_TYPE_MEDIA==GET_VOLUMETYPE_FROM_CONNECTID(connectID)) && (isSysMediaMute)){
            logInfo(AC,"sys mute for media situation");
        }else if((E_VOLUME_TYPE_SOFTKEY==GET_VOLUMETYPE_FROM_CONNECTID(connectID)) && (isSysMediaMute)){
            logInfo(AC,"sys mute for softKey situation");
        }else if(E_STREAM_BACKDOOR_NOTI==GET_STREAMTYPE_FROM_CONNECTID(connectID)){
            volSet=BACKDOOR_VOLUME_STEP;
        }else if(E_STREAM_FEEDBACK_NOTI==GET_STREAMTYPE_FROM_CONNECTID(connectID)){
            volSet = mCurFeedbackToneVol;
            logInfo(AC,"get mCurFeedbackToneVol:",volSet);
        }else{
            volSet = mVolSetMap[GET_VOLUMEINFO_FROM_CONNECTID(connectID)];
            logInfo(AC,"get volstep from mVolSetMap volSet:",volSet);
        }
    }else if(E_GAIN_FADE == focusSta){
        if(E_STREAM_CARPLAY_AUDIO==GET_STREAMTYPE_FROM_CONNECTID(connectID)){
            //carplay audio don't fade by system
            volSet = mVolSetMap[GET_VOLUMEINFO_FROM_CONNECTID(connectID)];
            logInfo(AC,"get volstep from mVolSetMap volSet:",volSet);
        }else if(E_VOLUME_TYPE_MEDIA==GET_VOLUMETYPE_FROM_CONNECTID(connectID)){
            if(isSysMediaMute){
                logInfo(AC,"sys media mute situation");
            }else{
				isTmpVolGainDown = true;
                int mediaVol = mVolSetMap[GET_VOLUMEINFO_FROM_CONNECTID(connectID)];
                if(isMixWithNavi && isMixWithRadar){
                    logInfo(AC,"isMixWithNavi && isMixWithRadar");
                    volSet = (naviVolSet<=radarVolSet)?naviVolSet:radarVolSet;
                    volSet = (volSet<=mediaVol)?volSet:mediaVol;
                }else if(isMixWithNavi && (!isMixWithRadar)){
                    logInfo(AC,"isMixWithNavi && (!isMixWithRadar)");
                    volSet = (naviVolSet<=mediaVol)?naviVolSet:mediaVol;
                }else if((!isMixWithNavi) && isMixWithRadar){
                    logInfo(AC,"(!isMixWithNavi) && isMixWithRadar");
                    volSet = (radarVolSet<=mediaVol)?radarVolSet:mediaVol;
                }else{
					isTmpVolGainDown = false;
                    logError(AC,"Error mix state");
                }
            }
        }else if(E_VOLUME_TYPE_NAVITTS==GET_VOLUMETYPE_FROM_CONNECTID(connectID)){
			isTmpVolGainDown = true;
			int mediaVol = mVolSetMap[GET_VOLUMEINFO_FROM_CONNECTID(connectID)];
			volSet = (phoneVolSet<=mediaVol)?phoneVolSet:mediaVol;
        }else{
            logError(AC,"the streamClass don't hav mix situation");
        }
    }else{
        logInfo(AC,"focusSta == E_FOCUSLOSE");
    }

	isVolGainDown = isTmpVolGainDown;
    logVerbose(AC,"AudioSetting::getVolSet()-->OUT volSet=",volSet);
    return volSet;
}

int AudioSetting::getVolStep(ama_connectID_t connectID,ama_focusSta_t focusSta, bool &isVolGainDown)
{
    logVerbose(AC,"AudioSetting::getVolStep()-->IN");

    int volStep = MUTE_VOL_STEP;
    int volSet = getVolSet(connectID,focusSta, isVolGainDown);

    volStep = volSet;
    logVerbose(AC,"AudioSetting::getVolStep()-->OUT volStep=",volStep);
    return volStep;
}
int AudioSetting::getVolGain(ama_connectID_t connectID,ama_focusSta_t focusSta)
{
    logVerbose(AC,"AudioSetting::getVolGain()-->IN");
	bool isVolGainDown = false;
    int volGain = AM_MUTE_DB;

    int volStep = getVolStep(connectID,focusSta, isVolGainDown);
    volGain = AMClient::GetInstance()->getVolGainByStep(connectID,volStep);

	if (isVolGainDown){
		volGain -= 60;
	}
	
    if((volGain!=AM_MUTE_DB)&&(GET_DEVICETYPE_FROM_CONNECTID(connectID)==E_IVI_SPEAKER)
        &&(GET_FOCUSTYPE_FROM_CONNECTID(connectID)==E_FOCUS_TYPE_MEDIA)){
        //because 1db = 10 volGain,so we need *10
        volGain += (mVoldbAddBySpeed*10);

        if(volGain >= 0){
            volGain = 0;
        }
    }

    logVerbose(AC,"AudioSetting::getVolGain()-->OUT volGain=",volGain);
    return volGain;
}

int AudioSetting::getBassSet(ama_audioDeviceType_t audioDeviceID)
{
    logVerbose(AC,"AudioSetting::getBassSet()-->IN audioDeviceID=",ToHEX(audioDeviceID));

    int bassSet = mBassSetMap[audioDeviceID];
    logVerbose(AC,"AudioSetting::getBassSet()-->OUT bassSet=",bassSet);
    return bassSet;
}

int AudioSetting::getMidSet(ama_audioDeviceType_t audioDeviceID)
{
    logVerbose(AC,"AudioSetting::getMidSet()-->IN audioDeviceID=",ToHEX(audioDeviceID));

    int midSet = mMidSetMap[audioDeviceID];
    logVerbose(AC,"AudioSetting::getMidSet()-->OUT midSet=",midSet);
    return midSet;
}

int AudioSetting::getTrebleSet(ama_audioDeviceType_t audioDeviceID)
{
    logVerbose(AC,"AudioSetting::getTrebleSet()-->IN audioDeviceID=",ToHEX(audioDeviceID));

    int trebleSet = mTrebleSetMap[audioDeviceID];
    logVerbose(AC,"AudioSetting::getTrebleSet()-->OUT trebleSet=",trebleSet);
    return trebleSet;
}

int AudioSetting::getBalanceSet()
{
    logVerbose(AC,"AudioSetting::getBalanceSet()-->IN");
    logVerbose(AC,"AudioSetting::getBalanceSet()-->OUT balanceSet=",mBalanceSet);
    return mBalanceSet;
}

int AudioSetting::getFaderSet()
{
    logVerbose(AC,"AudioSetting::getFaderSet()-->IN");
    logVerbose(AC,"AudioSetting::getFaderSet()-->OUT faderSet=",mFaderSet);
    return mFaderSet;
}

int AudioSetting::getSoundEffectSet(ama_seatID_t seatID)
{
    logVerbose(AC,"AudioSetting::getTrebleSet()-->IN seatID=",ToHEX(seatID));
    int soundEffect = E_Stereo;
    if((seatID>E_SEAT_TEST)||(seatID<E_SEAT_MAX)){
        soundEffect = mSoundEnvironmentalSet[seatID];
    }else{
        logError(AC,"seatID is not vaild");
    }

    logVerbose(AC,"AudioSetting::getSoundEffectSet()-->OUT");
    return soundEffect;
}

void AudioSetting::attach(SettingObserver *pSettingObserver)
{
    logVerbose(AC,"AudioSetting::attach()-->IN");

    mObserverList.push_back(pSettingObserver);

    logVerbose(AC,"AudioSetting::attach()-->OUT");
}

void AudioSetting::detach(SettingObserver *pSettingObserver)
{
    logVerbose(AC,"AudioSetting::detach()-->IN");

    for(std::list<SettingObserver *>::iterator it = mObserverList.begin(); it != mObserverList.end(); ++it){
        if(*it == pSettingObserver){
            logVerbose(AC,"remove pSettingObserver from mObserverList");
            mObserverList.erase(it);
            logVerbose(AC,"AudioSetting::detach()-->OUT");
            return;
        }
    }

    logError(AC,"can't find pSettingObserver in mObserverList");
    logVerbose(AC,"AudioSetting::detach()-->OUT");
}

void AudioSetting::notifySettingChanged(void)
{
    logVerbose(AC,"AudioSetting::notifySettingChanged()-->IN");

    list<SettingObserver *>::iterator it;
    for(it=mObserverList.begin(); it!=mObserverList.end(); ++it){
        (*it)->OnAudioSettingChanged();
    }

    logVerbose(AC,"AudioSetting::notifySettingChanged()-->OUT");
}

void AudioSetting::debugShowAudioSetting(void)
{
    logVerbose(DBG,"AudioSetting::debugShowAudioSetting()-->IN");

    int index = 0;
    std::map<ama_volumeInfo_t,int>::iterator it;
    for(it=mVolSetMap.begin();it!=mVolSetMap.end();++it,++index){
        logDebug(DBG,"audioSetting index:",index);
        logDebug(DBG,"audioSetting volInfo:", ToHEX(it->first));
        logDebug(DBG,"audioSetting volSet:",it->second);
    }
    std::map<ama_audioDeviceType_t,bool>::iterator it_mute;
    for(it_mute=mIsMediaMuteMap.begin();it_mute!=mIsMediaMuteMap.end();++it_mute,++index){
        logDebug(DBG,"audioSetting index:",index);
        logDebug(DBG,"audioSetting audioDeviceID:", ToHEX(it_mute->first));
        logDebug(DBG,"audioSetting isMediaMute:",it_mute->second);
    }

    logVerbose(DBG,"AudioSetting::debugShowAudioSetting()-->OUT");
}

void AudioSetting::setFeedbackVol(int feedbackVol)
{
    logVerbose(AC,"AudioSetting::setFeedbackVol()-->IN feedbackVol:",feedbackVol);

    mCurFeedbackToneVol = feedbackVol;

    logVerbose(AC,"AudioSetting::setFeedbackVol()-->OUT");
}

bool AudioSetting::isVolumeTypeActive(int volumeType)
{
    logVerbose(AC,"AudioSetting::isVolumeTypeActive()-->IN");

    bool isVolumeInfoActive = true;
    ama_volumeInfo_t volInfo;
    ama_volumeInfo_t volumeInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NAVITTS,E_IVI_SPEAKER);
    bool isMixWithNavi = FocusController::GetInstance()->isVolInfoHadFocus(volumeInfo);
    volumeInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_PHONE,E_IVI_SPEAKER);
    bool isMixWithPhone = FocusController::GetInstance()->isVolInfoHadFocus(volumeInfo);
    volumeInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_FRONTRADAR,E_IVI_SPEAKER);
    bool isMixWithFrontRadar = FocusController::GetInstance()->isVolInfoHadFocus(volumeInfo);
    volumeInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_REARRADAR,E_IVI_SPEAKER);
    bool isMixWithRearRadar = FocusController::GetInstance()->isVolInfoHadFocus(volumeInfo);

    bool isMixWithRadar = false;
    if(isMixWithFrontRadar || isMixWithRearRadar){
        isMixWithRadar = true;
    }else{
        isMixWithRadar = false;
    }

    switch(volumeType){
        case E_MediaVolMixWithNavi:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_MEDIA,E_IVI_SPEAKER);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            if(isVolumeInfoActive&&isMixWithNavi){
                return true;
            }else{
                return false;
            }
        case E_MeidaVolMixWithRadar:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_MEDIA,E_IVI_SPEAKER);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            if(isVolumeInfoActive&&isMixWithRadar){
                return true;
            }else{
                return false;
            }
        case E_NaviVolMixWithPhone:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NAVITTS,E_IVI_SPEAKER);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            if(isVolumeInfoActive&&isMixWithPhone){
                return true;
            }else{
                return false;
            }
        case E_FrontRadarVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_FRONTRADAR,E_IVI_SPEAKER);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            return isVolumeInfoActive;
        case E_RearRadarVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_REARRADAR,E_IVI_SPEAKER);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            return isVolumeInfoActive;
        case E_IVINotificationVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NOTIFICATION,E_IVI_SPEAKER);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            return isVolumeInfoActive;
        case E_LRseNotificationVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NOTIFICATION,E_RSE_L_HEADSET);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            return isVolumeInfoActive;
        case E_RRseNotificationVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NOTIFICATION,E_RSE_R_HEADSET);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            return isVolumeInfoActive;
        case E_IVISoftKeyVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_SOFTKEY,E_IVI_SPEAKER);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            return isVolumeInfoActive;
        case E_LRseSoftKeyVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_SOFTKEY,E_RSE_L_HEADSET);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            return isVolumeInfoActive;
        case E_RRseSoftKeyVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_SOFTKEY,E_RSE_R_HEADSET);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            return isVolumeInfoActive;
        case E_IVIMediaVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_MEDIA,E_IVI_SPEAKER);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            return isVolumeInfoActive;
        case E_LRseMediaVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_MEDIA,E_RSE_L_HEADSET);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            return isVolumeInfoActive;
        case E_RRseMediaVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_MEDIA,E_RSE_R_HEADSET);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            return isVolumeInfoActive;
        case E_SMSTTSVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_SMSTTS,E_IVI_SPEAKER);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            return isVolumeInfoActive;
        case E_VRTTSVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_VRTTS,E_IVI_SPEAKER);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            return isVolumeInfoActive;
        case E_NaviTTSVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NAVITTS,E_IVI_SPEAKER);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            return isVolumeInfoActive;
        case E_PhoneVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_PHONE,E_IVI_SPEAKER);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            return isVolumeInfoActive;
        case E_ECallVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_ECALL,E_IVI_SPEAKER);
            isVolumeInfoActive = AMClient::GetInstance()->isVolInfoActive(volInfo);
            return isVolumeInfoActive;
        default:
            logError(AC,"volumeType is not defined!");
    }
    logVerbose(AC,"AudioSetting::isVolumeTypeActive()-->IN");
}

void AudioSetting::handleMuteStaChangedByFocusCtrl(ama_seatID_t seatID,bool isMute)
{
    logVerbose(AC,"AudioSetting::handleMuteStaChangedByFocusCtrl()-->IN seatID:",ToHEX(seatID),"isMute:",isMute);

    switch(seatID){
    case E_SEAT_IVI:
        if(isMute != mIsMediaMuteMap[E_SEAT_IVI]){
            mIsMediaMuteMap[E_SEAT_IVI] = isMute;
            BroadcastAMGRSetNotify(E_IS_IVI_MUTE,(isMute?"1":"0"));
            notifySettingChanged();
        }else{
            logInfo(AC,"Mute state wasn't changed");
        }
        break;
    case E_SEAT_RSE1:
        if(isMute != mIsMediaMuteMap[E_SEAT_RSE1]){
            mIsMediaMuteMap[E_SEAT_RSE1] = isMute;
            BroadcastAMGRSetNotify(E_IS_LRSE_MUTE,(isMute?"1":"0"));
            notifySettingChanged();
        }else{
            logInfo(AC,"Mute state wasn't changed");
        }
        break;
    case E_SEAT_RSE2:
        if(isMute != mIsMediaMuteMap[E_SEAT_RSE2]){
            mIsMediaMuteMap[E_SEAT_RSE2] = isMute;
            BroadcastAMGRSetNotify(E_IS_RRSE_MUTE,(isMute?"1":"0"));
            notifySettingChanged();
        }else{
            logInfo(AC,"Mute state wasn't changed");
        }
        break;
    default:
        logError(AC,"seatID is not defined");
        break;
    }

    logVerbose(AC,"AudioSetting::handleMuteStaChangedByFocusCtrl()-->OUT");
}

void AudioSetting::setGainForSpeedByRPC(int gainForSpeed)
{
    logVerbose(AC,"AudioSetting::setGainForSpeedByRPC()-->IN gainForSpeed:",gainForSpeed);

    if(mVoldbAddBySpeed != gainForSpeed){
        mVoldbAddBySpeed = gainForSpeed;
        notifySettingChanged();
    }

    logVerbose(AC,"AudioSetting::setGainForSpeedByRPC()-->OUT");
}

void AudioSetting::handleAudioSetting(int key,int value)
{
    logVerbose(AC,"AudioSetting::handleAudioSetting()-->IN key:",key," value:",value);
    ama_volumeInfo_t volInfo;

    //there are some functions don't work in new setByAppManger();
    //1:volume and mute relation
    //2:restore audioSetting when speaker user changed

    switch(key){
        case E_MediaVolMixWithNavi:
            if(value != mMediaVolStepMixWithNavi){
                mMediaVolStepMixWithNavi = value;
                notifySettingChanged();
            }
            break;
        case E_MeidaVolMixWithRadar:
            if(value != mMediaVolStepMixWithRadar){
                mMediaVolStepMixWithRadar = value;
                notifySettingChanged();
            }
            break;
        case E_NaviVolMixWithPhone:
            if(value != mNaviVolStepMixWithPhone){
                mNaviVolStepMixWithPhone = value;
                notifySettingChanged();
            }
            break;
        case E_NaviVolMixWithPhoneNoFB:
            if(value != mNaviVolStepMixWithPhone){
                mNaviVolStepMixWithPhone = value;
                notifySettingChanged();
            }
            break;
        case E_FrontRadarVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_FRONTRADAR,E_IVI_SPEAKER);
            if(value != mVolSetMap[volInfo]){
                mVolSetMap[volInfo] = value;
                notifySettingChanged();
            }
            break;
        case E_RearRadarVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_REARRADAR,E_IVI_SPEAKER);
            if(value != mVolSetMap[volInfo]){
                mVolSetMap[volInfo] = value;
                notifySettingChanged();
            }
            break;
        case E_IVINotificationVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NOTIFICATION,E_IVI_SPEAKER);
            if(value != mVolSetMap[volInfo]){
                mVolSetMap[volInfo] = value;
                notifySettingChanged();
            }
            break;
        case E_LRseNotificationVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NOTIFICATION,E_RSE_L_HEADSET);
            if(value != mVolSetMap[volInfo]){
                mVolSetMap[volInfo] = value;
                notifySettingChanged();
            }
            break;
        case E_RRseNotificationVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NOTIFICATION,E_RSE_R_HEADSET);
            if(value != mVolSetMap[volInfo]){
                mVolSetMap[volInfo] = value;
                notifySettingChanged();
            }
            break;
        case E_IVISoftKeyVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_SOFTKEY,E_IVI_SPEAKER);
            if(value != mVolSetMap[volInfo]){
                mVolSetMap[volInfo] = value;
                notifySettingChanged();
            }
            break;
        case E_LRseSoftKeyVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_SOFTKEY,E_RSE_L_HEADSET);
            if(value != mVolSetMap[volInfo]){
                mVolSetMap[volInfo] = value;
                notifySettingChanged();
            }
            break;
        case E_RRseSoftKeyVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_SOFTKEY,E_RSE_R_HEADSET);
            if(value != mVolSetMap[volInfo]){
                mVolSetMap[volInfo] = value;
                notifySettingChanged();
            }
            break;
        case E_IVIMediaVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_MEDIA,E_IVI_SPEAKER);
            if(value != mVolSetMap[volInfo]){
                mVolSetMap[volInfo] = value;
                notifySettingChanged();
            }
            break;
        case E_LRseMediaVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_MEDIA,E_RSE_L_HEADSET);
            if(value != mVolSetMap[volInfo]){
                mVolSetMap[volInfo] = value;
                notifySettingChanged();
            }
            break;
        case E_RRseMediaVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_MEDIA,E_RSE_R_HEADSET);
            if(value != mVolSetMap[volInfo]){
                mVolSetMap[volInfo] = value;
                notifySettingChanged();
            }
            break;
        case E_SMSTTSVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_SMSTTS,E_IVI_SPEAKER);
            if(value != mVolSetMap[volInfo]){
                mVolSetMap[volInfo] = value;
                notifySettingChanged();
            }
            break;
        case E_VRTTSVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_VRTTS,E_IVI_SPEAKER);
            if(value != mVolSetMap[volInfo]){
                mVolSetMap[volInfo] = value;
                notifySettingChanged();
            }
            break;
        case E_NaviTTSVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NAVITTS,E_IVI_SPEAKER);
            if(value != mVolSetMap[volInfo]){
                mVolSetMap[volInfo] = value;
                notifySettingChanged();
            }
            break;
        case E_PhoneVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_PHONE,E_IVI_SPEAKER);
            if(value != mVolSetMap[volInfo]){
                mVolSetMap[volInfo] = value;
                notifySettingChanged();
            }
            break;
        case E_ECallVol:
            volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_ECALL,E_IVI_SPEAKER);
            if(value != mVolSetMap[volInfo]){
                mVolSetMap[volInfo] = value;
                notifySettingChanged();
            }
            break;
        case E_FeedbackVol:
            if(value != mCurFeedbackToneVol){
                setFeedbackVol(value);
            }
            break;

        case E_IVISoundBalance:
            if(value != mBalanceSet){
                mBalanceSet = value;
                Amplifier::GetInstance()->setAMPWhenSettingChanged();
            }
            break;
        case E_IVISoundFader:
            if(value != mFaderSet){
                mFaderSet = value;
                Amplifier::GetInstance()->setAMPWhenSettingChanged();
            }
            break;

        case E_IVISoundTreble:
            if(value != mTrebleSetMap[E_IVI_SPEAKER]){
                mTrebleSetMap[E_IVI_SPEAKER] = value;
                notifySettingChanged();
            }
            break;
        case E_LRseSoundTreble:
            if(value != mTrebleSetMap[E_RSE_L_HEADSET]){
                mTrebleSetMap[E_RSE_L_HEADSET] = value;
                notifySettingChanged();
            }
            break;
        case E_RRseSoundTreble:
            if(value != mTrebleSetMap[E_RSE_R_HEADSET]){
                mTrebleSetMap[E_RSE_R_HEADSET] = value;
                notifySettingChanged();
            }
            break;
        case E_IVISoundMid:
            if(value != mMidSetMap[E_IVI_SPEAKER]){
                mMidSetMap[E_IVI_SPEAKER] = value;
                notifySettingChanged();
            }
            break;
        case E_LRseSoundMid:
            if(value != mMidSetMap[E_RSE_L_HEADSET]){
                mMidSetMap[E_RSE_L_HEADSET] = value;
                notifySettingChanged();
            }
            break;
        case E_RRseSoundMid:
            if(value != mMidSetMap[E_RSE_R_HEADSET]){
                mMidSetMap[E_RSE_R_HEADSET] = value;
                notifySettingChanged();
            }
            break;
        case E_IVISoundBass:
            if(value != mBassSetMap[E_IVI_SPEAKER]){
                mBassSetMap[E_IVI_SPEAKER] = value;
                notifySettingChanged();
            }
            break;
        case E_LRseSoundBass:
            if(value != mBassSetMap[E_RSE_L_HEADSET]){
                mBassSetMap[E_RSE_L_HEADSET] = value;
                notifySettingChanged();
            }
            break;
        case E_RRseSoundBass:
            if(value != mBassSetMap[E_RSE_R_HEADSET]){
                mBassSetMap[E_RSE_R_HEADSET] = value;
                notifySettingChanged();
            }
            break;

        case E_IVISoundEnviroment:
            if(value != mSoundEnvironmentalSet[E_SEAT_IVI]){
                mSoundEnvironmentalSet[E_SEAT_IVI] = value;
                Amplifier::GetInstance()->setAMPWhenSettingChanged();
            }
            break;
        case E_LRseSoundEnviroment:
            if(value != mSoundEnvironmentalSet[E_SEAT_RSE1]){
                mSoundEnvironmentalSet[E_SEAT_RSE1] = value;
                Amplifier::GetInstance()->setAMPWhenSettingChanged();
            }
            break;
        case E_RRseSoundEnviroment:
            if(value != mSoundEnvironmentalSet[E_SEAT_RSE2]){
                mSoundEnvironmentalSet[E_SEAT_RSE2] = value;
                Amplifier::GetInstance()->setAMPWhenSettingChanged();
            }
            break;
        case E_IsEnablePhoneMixNavi:
            if(value){
                mIsPhoneAndNaviMix = true;
            }else{
                mIsPhoneAndNaviMix = false;
            }
            FocusController::GetInstance()->refreshFocusStaWhenSettingChanged();
            break;
        case E_FrontRadarToneType:
            break;
        case E_RearRadarToneType:
            break;

        case E_IsIVIMediaMute:
            if(value){
                mIsMediaMuteMap[E_IVI_SPEAKER] = true;
                notifySettingChanged();
            }else{
                mIsMediaMuteMap[E_IVI_SPEAKER] = false;
                notifySettingChanged();
            }
            break;                      // default:0                       IVI MUTE switch      0:false mediaVol is Unmute  1:true mediaVol is mute default:0
        case E_IsLRseMediaMute:
            if(value){
                mIsMediaMuteMap[E_RSE_L_HEADSET] = true;
                notifySettingChanged();
            }else{
                mIsMediaMuteMap[E_RSE_L_HEADSET] = false;
                notifySettingChanged();
            }
            break;              // default:0                       LRse MUTE switch      0:false mediaVol is Unmute  1:true mediaVol is mute default:0
        case E_IsRRseMediaMute:
            if(value){
                mIsMediaMuteMap[E_RSE_R_HEADSET] = true;
                notifySettingChanged();
            }else{
                mIsMediaMuteMap[E_RSE_R_HEADSET] = false;
                notifySettingChanged();
            }
            break;                          // default:0                       RRse MUTE switch      0:false mediaVol is Unmute  1:true mediaVol is mute default:0
        case E_IsEnableFeedback:
            if(value){
                mIsEnableFeedback = true;
            }else{
                mIsEnableFeedback = false;
            }
            break;
        case E_IsDisableRseUseSpeaker:
            if(value){
                mIsEnableRSEUseSpeaker = false;
            }else{
                mIsEnableRSEUseSpeaker = true;
            }
            Speaker::GetInstance()->isEnableRSEUseSpeakerStaChanged();
            break;// default:0                       0:false - Rse can use speaker, 1:true - Rse seats can not use speaker
        case E_IsOpenKidsMode:
            if(value){
                sendTaskToCS(E_CS_TASK_TYPE_ENABLE_CHILDRENLOCK);
            }else{
                sendTaskToCS(E_CS_TASK_TYPE_DISABLE_CHILDRENLOCK);
            }
            break;                                  // default:0                       0:close - Disable Kids Mode , 1:Open - enable Kids Mode
        case E_IsDisableUseRse:
            if(value){
                sendTaskToCS(E_CS_TASK_TYPE_ENABLE_RSELOCK);
            }else{
                sendTaskToCS(E_CS_TASK_TYPE_DISABLE_RSELOCK);
            }
            break;                          // default:0                       0:false - User can use Rse Screen , 1:true - User can not use Rse Screen
        case E_IVIScrBrightness:
            LCDManager::GetInstance()->SetLCDBrightness(value,E_SEAT_IVI);
            break;              // default:50                      IVI Adjust screen brightness mode 0~100
        case E_LRseScrBrightness:
            LCDManager::GetInstance()->SetLCDBrightness(value,E_SEAT_RSE1);
            break;                  // default:50                      LRse Adjust screen brightness mode 0~100
        case E_RRseScrBrightness:
            LCDManager::GetInstance()->SetLCDBrightness(value,E_SEAT_RSE2);
            break;
        default:
            logError(AC,"we don't handle the key here");
            break;
    }

    logVerbose(AC,"AudioSetting::handleAudioSetting()-->OUT");
}

void AudioSetting::sendTaskToCS(int taskType)
{
    logVerbose(AC,"AudioSetting::sendTaskToCS()-->IN");

    auto csTask = std::make_shared<CSTask>();
    csTask->SetType((CSTaskType)taskType);
    TaskDispatcher::GetInstance()->Dispatch(csTask);

    logVerbose(AC,"AudioSetting::sendTaskToCS()-->IN");
}

void AudioSetting::loadVolSetFromDB(void* pDBInput)
{
    DomainLog chatter(AC, LOG_VBOS, formatText("AudioSetting::"+string(__FUNCTION__)+"()"));

    ama_volumeInfo_t volInfo;
    if (pDBInput == NULL)
    {
        logVerbose(AC, "input db pointer is null, and set vol info to def");

        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_FRONTRADAR,E_IVI_SPEAKER);      // syssetting_IVI_RADAR_VOL     20
        mVolSetMap[volInfo] = 20;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_REARRADAR,E_IVI_SPEAKER);       // syssetting_RSE_RADAR_VOL     20
        mVolSetMap[volInfo] = 20;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NOTIFICATION,E_IVI_SPEAKER);    // syssetting_IVI_INFORMING_VOL 10
        mVolSetMap[volInfo] = 10;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NOTIFICATION,E_RSE_L_HEADSET);  // 
        mVolSetMap[volInfo] = 10;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NOTIFICATION,E_RSE_R_HEADSET);  // 
        mVolSetMap[volInfo] = 10;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_SOFTKEY,E_IVI_SPEAKER);         // syssetting_IVI_SOFTKEY_VOL   10
        mVolSetMap[volInfo] = 10;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_SOFTKEY,E_RSE_L_HEADSET);       // syssetting_RSE1_SOFTKEY_VOL  10
        mVolSetMap[volInfo] = 10;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_SOFTKEY,E_RSE_R_HEADSET);       // syssetting_RSE2_SOFTKEY_VOL  10
        mVolSetMap[volInfo] = 10;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_MEDIA,E_IVI_SPEAKER);           // syssetting_IVI_MEDIA_VOL     10 
        mVolSetMap[volInfo] = 10;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_MEDIA,E_RSE_L_HEADSET);         // syssetting_RSE1_MEDIA_VOL    10
        mVolSetMap[volInfo] = 10;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_MEDIA,E_RSE_R_HEADSET);         // syssetting_RSE2_MEDIA_VOL    10
        mVolSetMap[volInfo] = 10;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_SMSTTS,E_IVI_SPEAKER);          // syssetting_IVI_SMSTTS_VOL    15
        mVolSetMap[volInfo] = 15;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_VRTTS,E_IVI_SPEAKER);           // syssetting_IVI_VRTTS_VOL     15
        mVolSetMap[volInfo] = 15;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NAVITTS,E_IVI_SPEAKER);         // syssetting_IVI_NAVITTS_VOL   15
        mVolSetMap[volInfo] = 15;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_PHONE,E_IVI_SPEAKER);           // syssetting_IVI_PHONE_VOL     15
        mVolSetMap[volInfo] = 15;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_ECALL,E_IVI_SPEAKER);           // syssetting_IVI_ECALL_VOL     20
        mVolSetMap[volInfo] = 20;
    }
    else
    {
        logVerbose(AC, "get vol info from db");
        int nValue = 10;
        string strKey = "";
        AppDatabaseEx* pDB = (AppDatabaseEx *)pDBInput;

        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_FRONTRADAR,E_IVI_SPEAKER);      // syssetting_IVI_RADAR_VOL     20
        strKey = "syssetting_IVI_RADAR_VOL";
        if (pDB->getValueOfKey(strKey, nValue) != ama_DB_Error_e::E_DB_OK)
            nValue = 20;
        mVolSetMap[volInfo] = nValue;

        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_REARRADAR,E_IVI_SPEAKER);       // syssetting_RSE_RADAR_VOL     20
        strKey = "syssetting_RSE_RADAR_VOL";
        if (pDB->getValueOfKey(strKey, nValue) != ama_DB_Error_e::E_DB_OK)
            nValue = 20;
        mVolSetMap[volInfo] = nValue;

        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NOTIFICATION,E_IVI_SPEAKER);    // syssetting_IVI_INFORMING_VOL 10
        strKey = "syssetting_IVI_INFORMING_VOL";
        if (pDB->getValueOfKey(strKey, nValue) != ama_DB_Error_e::E_DB_OK)
            nValue = 10;
        mVolSetMap[volInfo] = nValue;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NOTIFICATION,E_RSE_L_HEADSET);  // 
        mVolSetMap[volInfo] = nValue;
        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NOTIFICATION,E_RSE_R_HEADSET);  // 
        mVolSetMap[volInfo] = nValue;

        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_SOFTKEY,E_IVI_SPEAKER);         // syssetting_IVI_SOFTKEY_VOL   10
        strKey = "syssetting_IVI_SOFTKEY_VOL";
        if (pDB->getValueOfKey(strKey, nValue) != ama_DB_Error_e::E_DB_OK)
            nValue = 10;
        mVolSetMap[volInfo] = nValue;

        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_SOFTKEY,E_RSE_L_HEADSET);       // syssetting_RSE1_SOFTKEY_VOL  10
        strKey = "syssetting_RSE1_SOFTKEY_VOL";
        if (pDB->getValueOfKey(strKey, nValue) != ama_DB_Error_e::E_DB_OK)
            nValue = 10;
        mVolSetMap[volInfo] = nValue;

        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_SOFTKEY,E_RSE_R_HEADSET);       // syssetting_RSE2_SOFTKEY_VOL  10
        strKey = "syssetting_RSE2_SOFTKEY_VOL";
        if (pDB->getValueOfKey(strKey, nValue) != ama_DB_Error_e::E_DB_OK)
            nValue = 10;
        mVolSetMap[volInfo] = nValue;

        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_MEDIA,E_IVI_SPEAKER);           // syssetting_IVI_MEDIA_VOL     10 
        strKey = "syssetting_IVI_MEDIA_VOL";
        if (pDB->getValueOfKey(strKey, nValue) != ama_DB_Error_e::E_DB_OK)
            nValue = 10;
        mVolSetMap[volInfo] = nValue;

        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_MEDIA,E_RSE_L_HEADSET);         // syssetting_RSE1_MEDIA_VOL    10
        strKey = "syssetting_RSE1_MEDIA_VOL";
        if (pDB->getValueOfKey(strKey, nValue) != ama_DB_Error_e::E_DB_OK)
            nValue = 10;
        mVolSetMap[volInfo] = nValue;

        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_MEDIA,E_RSE_R_HEADSET);         // syssetting_RSE2_MEDIA_VOL    10
        strKey = "syssetting_RSE2_MEDIA_VOL";
        if (pDB->getValueOfKey(strKey, nValue) != ama_DB_Error_e::E_DB_OK)
            nValue = 10;
        mVolSetMap[volInfo] = nValue;

        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_SMSTTS,E_IVI_SPEAKER);          // syssetting_IVI_SMSTTS_VOL    15
        strKey = "syssetting_IVI_SMSTTS_VOL";
        if (pDB->getValueOfKey(strKey, nValue) != ama_DB_Error_e::E_DB_OK)
            nValue = 15;
        mVolSetMap[volInfo] = nValue;

        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_VRTTS,E_IVI_SPEAKER);           // syssetting_IVI_VRTTS_VOL     15
        strKey = "syssetting_IVI_VRTTS_VOL";
        if (pDB->getValueOfKey(strKey, nValue) != ama_DB_Error_e::E_DB_OK)
            nValue = 15;
        mVolSetMap[volInfo] = nValue;

        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_NAVITTS,E_IVI_SPEAKER);         // syssetting_IVI_NAVITTS_VOL   15
        strKey = "syssetting_IVI_NAVITTS_VOL";
        if (pDB->getValueOfKey(strKey, nValue) != ama_DB_Error_e::E_DB_OK)
            nValue = 15;
        mVolSetMap[volInfo] = nValue;

        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_PHONE,E_IVI_SPEAKER);           // syssetting_IVI_PHONE_VOL     15
        strKey = "syssetting_IVI_PHONE_VOL";
        if (pDB->getValueOfKey(strKey, nValue) != ama_DB_Error_e::E_DB_OK)
            nValue = 15;
        mVolSetMap[volInfo] = nValue;

        volInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_ECALL,E_IVI_SPEAKER);           // syssetting_IVI_ECALL_VOL     20
        strKey = "syssetting_IVI_ECALL_VOL";
        if (pDB->getValueOfKey(strKey, nValue) != ama_DB_Error_e::E_DB_OK)
            nValue = 20;
        mVolSetMap[volInfo] = nValue;

    }
}
