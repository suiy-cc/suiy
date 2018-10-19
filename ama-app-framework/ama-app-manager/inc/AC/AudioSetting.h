/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AudioSetting.h
/// @brief contains class AudioSetting
///
/// Created by wangqi on 2016/11/30.
/// this file contains the definination of class AudioSetting
///

#ifndef AUDIOSETTING_H_
#define AUDIOSETTING_H_

#include <mutex>
#include <vector>
#include <map>
#include <list>
#include <string>

#include "AMGRLogContext.h"
#include "ama_types.h"
#include "ama_audioTypes.h"
#include "Amplifier.h"

using std::vector;
using std::map;
using std::string;
using std::list;

class SettingObserver;

enum soundEnvironmental_Set_e{
    E_CenterPoint = 0x00,
    E_Stereo = 0x01,
    E_Stereo_driver = 0x02,
    E_Stereo_rear = 0x03
};

#define MIN_MEDIA_VOL_WHEN_PWRON                        (10)
#define MAX_MEDIA_VOL_WHEN_PWRON                        (20)
#define MIN_MEDIA_VOL_WHEN_PWRON_STR                    "10"

//speed change volume algorithm
#define CRITICAL_ENGINE_SPEED                           (100)
#define MAX_VEHICLE_SPEED                               (206)

#define BACKDOOR_VOLUME_STEP                            (20)

static uint8_t gainForSpeed = 0;

static int gainForSpeedTable[MAX_VEHICLE_SPEED] =
{//|  0|  1|  2|  3|  4|  5|  6|  7|  8|  9| 10| 11| 12| 13| 14| 15| 16| 17| 18| 19| 20| 21| 22| 23| 24| 25| 26| 27| 28| 29| 30| 31| 32| 33| 34| 35| 36| 37| 38| 39|
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
 //| 40| 41| 42| 43| 44| 45| 46| 47| 48| 49| 50| 51| 52| 53| 54| 55| 56| 57| 58| 59| 60| 61| 62| 63| 64| 65| 66| 67| 68| 69| 70| 71| 72| 73| 74| 75| 76| 77| 78| 79|
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,
 //| 80| 81| 82| 83| 84| 85| 86| 87| 88| 89| 90| 91| 92| 93| 94| 95| 96| 97| 98| 99|100|101|102|103|104|105|106|107|108|109|110|111|112|113|114|115|116|117|118|119|
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,
 //|120|121|122|123|124|125|126|127|128|129|130|131|132|133|134|135|136|137|138|139|140|141|142|143|144|145|146|147|148|149|150|151|152|153|154|155|156|157|158|159|
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
 //|160|161|162|163|164|165|166|167|168|169|170|171|172|173|174|175|176|177|178|179|180|181|182|183|184|185|186|187|188|189|190|191|192|193|194|195|196|197|198|199|
      8,  8,  8,  9,  9,  9,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11,
 //|200|201|202|203|204|205|
     11, 11, 11, 11, 11, 11,
};

bool isGainForSpeedChanged(int ABS_VehicleSpeed,int EMS_EngineSpeed);

/// @class AudioSetting
/// @brief this class is the abstraction of AudioSetting
class AudioSetting
{
public:
    static AudioSetting* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static AudioSetting instance;
        return &instance;
    }
    ~AudioSetting();

    void handleAudioSetting(int key,int value);

    bool isIVIMediaMute(void);

    int getVolSet(ama_connectID_t connectID,ama_focusSta_t focusSta, bool &isVolGainDown);
    int getVolStep(ama_connectID_t connectID,ama_focusSta_t focusSta, bool &isVolGainDown);
    int getVolGain(ama_connectID_t connectID,ama_focusSta_t focusSta);

    int getBassSet(ama_audioDeviceType_t audioDeviceID);
    int getMidSet(ama_audioDeviceType_t audioDeviceID);
    int getTrebleSet(ama_audioDeviceType_t audioDeviceID);

    bool isPhoneAndNaviMix(void){
        return mIsPhoneAndNaviMix;
    }
    bool isEnableRSEUseSpeaker(void){
        return mIsEnableRSEUseSpeaker;
    }
    int getBalanceSet();
    int getFaderSet();
    int getSoundEffectSet(ama_seatID_t seatID);

    void attach(SettingObserver *pSettingObserver);
    void detach(SettingObserver *pSettingObserver);

    void setGainForSpeedByRPC(int gainForSpeed);

    void setFeedbackVol(int feedbackVol);
    bool isVolumeTypeActive(int volumeType);
    
    //this function is no longer used
    void handleMuteStaChangedByFocusCtrl(ama_seatID_t seatID,bool isMute);

    void debugShowAudioSetting(void);

    void setMediaMuteByPowerSta(bool isMediaMute);
private:
    AudioSetting();

    void notifySettingChanged(void);

    void sendTaskToCS(int taskType);

    void loadVolSetFromDB(void* pDBInput);

    std::map<ama_volumeInfo_t,int> mVolSetMap;
    std::map<ama_audioDeviceType_t,bool> mIsMediaMuteMap;
    bool mIsMediaMuteByPower;
    std::map<ama_audioDeviceType_t,int> mBassSetMap;
    std::map<ama_audioDeviceType_t,int> mMidSetMap;
    std::map<ama_audioDeviceType_t,int> mTrebleSetMap;
    int mBalanceSet;
    int mFaderSet;
    std::map<ama_seatID_t,int> mSoundEnvironmentalSet;
    bool mIsPhoneAndNaviMix;
    bool mIsEnableRSEUseSpeaker;
    bool mIsEnableFeedback;
    int mMediaVolStepMixWithRadar;
    int mMediaVolStepMixWithNavi;
    int mNaviVolStepMixWithPhone;
    int mABS_VehicleSpeed;
    int mVoldbAddBySpeed;

    int mCurFeedbackToneVol;

    std::list<SettingObserver *> mObserverList;
};

class SettingObserver
{
public:
    SettingObserver(){
        AudioSetting::GetInstance()->attach(this);
    }
    ~SettingObserver(){
        AudioSetting::GetInstance()->detach(this);
    }

    virtual void OnAudioSettingChanged(void)=0;
};

#endif //AUDIOSETTING_H_
