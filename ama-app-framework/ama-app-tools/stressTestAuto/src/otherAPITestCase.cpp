/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <AppSDK.h>
#include <AppIME.h>
#include <log.h>

#include "tools.h"
#include "appSDKTestCase.h"
#include "otherAPITestCase.h"

#include <vector>
#include <map>
#include <string>

static IMEContext sIMEConText;
static std::vector<PopupID> sPopupVec;
static std::multimap<string,string> sStateMap;
static int app_x=0;
static int app_y=0;
static unsigned int app_width=0;
static unsigned int app_height=0;

void changeOfSetCallBack(int state_type,  std::string  value){
    printf("state_type:%d \n",state_type);
}

void popupEventCallBack(PopupID id, uint32_t reason){
    printf("PopupID:%d;reason:%u \n",id,reason);
}

void IMEEventCallBack(std::string msg, EMsgType msgType){
    printf("IMEEventCallBack()--> IN \n");
}

void SM_handleReqSetCallBack(uint16_t ampid_req, std::string key, std::string value){
    logVerbose(SDKTEST,"SM_handleReqSetCallBack()-->IN");
}
void SM_handleStateChangeCallBack(uint16_t ampid_req, uint16_t ampid_reply, std::string  key,  std::string  value){
    logVerbose(SDKTEST,"SM_handleStateChangeCallBack()-->IN");
}

void loadOtherAPITestCase(void){
    AMGR_regCb4StateChanged(changeOfSetCallBack);
    imeRegisterInputEvent(IMEEventCallBack);
    SM_regReply4Req(SM_handleReqSetCallBack);
    SM_regCb4StateChanged(SM_handleStateChangeCallBack);
    psInitPopup();
    createStateMapForTestCase();
    TestCase::GetInstance()->addTestCase(testCase_setByAppManager);
    TestCase::GetInstance()->addTestCase(testCase_AMGR_getState);
    TestCase::GetInstance()->addTestCase(testCase_imeOpenIME);
    TestCase::GetInstance()->addTestCase(testCase_imeCloseIME);
    TestCase::GetInstance()->addTestCase(testCase_psShowPopup);
    TestCase::GetInstance()->addTestCase(testCase_psClosePopup);
    TestCase::GetInstance()->addTestCase(testCase_GetDestinationRectangle);
    TestCase::GetInstance()->addTestCase(testCase_SetDestinationRectangle);
    TestCase::GetInstance()->addTestCase(testCase_GetSourceRectangle);
    TestCase::GetInstance()->addTestCase(testCase_SetSourceRectangle);
    TestCase::GetInstance()->addTestCase(testCase_SetBothRectangle);
    TestCase::GetInstance()->addTestCase(testCase_SetUnderLayerCenter);
    TestCase::GetInstance()->addTestCase(testCase_IsUnderLayerCenter);
    TestCase::GetInstance()->addTestCase(testCase_ShowTitlebar);
    TestCase::GetInstance()->addTestCase(testCase_HideTitlebar);
    TestCase::GetInstance()->addTestCase(testCase_IsTitlebarVisible);
    TestCase::GetInstance()->addTestCase(testCase_EnableTouch);
    TestCase::GetInstance()->addTestCase(testCase_SM_getState);
    TestCase::GetInstance()->addTestCase(testCase_SM_req2SetState);
    TestCase::GetInstance()->addTestCase(testCase_SM_updateState);
}

void loadOnlyPopupAPITestCase(void)
{
    psInitPopup();
    TestCase::GetInstance()->addTestCase(testCase_psShowPopup);
    TestCase::GetInstance()->addTestCase(testCase_psClosePopup);
}

int getRandVolueByKey(ama_setType_e key){
    switch(key){
        case E_MediaVolMixWithNavi:
        case E_MeidaVolMixWithRadar:
        case E_NaviVolMixWithPhone:
        case E_FrontRadarVol:
        case E_RearRadarVol:
        case E_IVINotificationVol:
        case E_LRseNotificationVol:
        case E_RRseNotificationVol:
        case E_IVISoftKeyVol:
        case E_LRseSoftKeyVol:
        case E_RRseSoftKeyVol:
        case E_IVIMediaVol:
        case E_LRseMediaVol:
        case E_RRseMediaVol:
        case E_SMSTTSVol:
        case E_VRTTSVol:
        case E_NaviTTSVol:
        case E_PhoneVol:
        case E_ECallVol:
            return getRand(0,30);
        case E_IVISoundBalance:
        case E_IVISoundFader:
        case E_IVISoundTreble:
        case E_LRseSoundTreble:
        case E_RRseSoundTreble:
        case E_IVISoundMid:
        case E_LRseSoundMid:
        case E_RRseSoundMid:
        case E_IVISoundBass:
        case E_LRseSoundBass:
        case E_RRseSoundBass:
            return getRand(-6,6);
        case E_IVISoundEnviroment:
        case E_LRseSoundEnviroment:
        case E_RRseSoundEnviroment:
            return getRand(0,2);
        case E_IsEnablePhoneMixNavi:
            return getRand(0,1);
        case E_FrontRadarToneType:
        case E_RearRadarToneType:
            return getRand(1,3);
            break;
        case E_IsIVIMediaMute:                     // default:0                       IVI MUTE switch      0:false mediaVol is Unmute  1:true mediaVol is mute default:0
        case E_IsLRseMediaMute:              // default:0                       LRse MUTE switch      0:false mediaVol is Unmute  1:true mediaVol is mute default:0
        case E_IsRRseMediaMute:                       // default:0                       RRse MUTE switch      0:false mediaVol is Unmute  1:true mediaVol is mute default:0
        case E_IsEnableFeedback:
        case E_IsDisableRseUseSpeaker:// default:0                       0:false - Rse can use speaker, 1:true - Rse seats can not use speaker
        case E_IsOpenKidsMode:                                 // default:0                       0:close - Disable Kids Mode , 1:Open - enable Kids Mode
        case E_IsDisableUseRse:
            return getRand(0,1);
        case E_IVIScrBrightness:
        case E_LRseScrBrightness:
        case E_RRseScrBrightness:
            return getRand(0,100);
        default:
            break;
    }
}

bool testCase_setByAppManager(void){
    logVerbose(SDKTEST,"testCase_setByAppManager()-->IN");
    ama_setType_e key = (ama_setType_e)getRand(E_MediaVolMixWithNavi,E_Key_Max);
    setByAppManager(key,getRandVolueByKey(key));

    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_AMGR_getState(void){
    logVerbose(SDKTEST,"testCase_AMGR_getState()-->IN");
    ama_stateType_e state = (ama_stateType_e)getRand(E_SpeakerWho,E_StateType_NONE);
    AMGR_getState(state);

    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_imeOpenIME(void){
    logVerbose(SDKTEST,"testCase_imeOpenIME()-->IN");
    sIMEConText.KeyboardType = (EKeyboardType)getRand(E_KEYBOARD_TYPE_NINE,E_KEYBOARD_TYPE_FULL);
    sIMEConText.LangType = (ELangType)getRand(E_LANG_TYPE_CN,E_LANG_TYPE_CHAR);
    imeOpenIME(sIMEConText);

    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_imeCloseIME(void){
    logVerbose(SDKTEST,"testCase_imeCloseIME()-->IN");
    imeCloseIME();

    delayAfterCallAPI();

    return isAppMgrExist();
}

void funcShowPopup(int popupType){
    PopupInfo pi;
    pi.Pt = (PopupType)popupType;
    pi.Value = "";
    pi.Prior = getRand(0,10);
    pi.ScreenIndex = getRand(0,2);

    PopupID popupID = psRequestPopup(pi);
    psRegisterClickEvent(popupID,popupEventCallBack);

    psShowPopup(popupID);
    sPopupVec.push_back(popupID);
}

bool testCase_psShowAllTypePopup(void){
    logVerbose(SDKTEST,"testCase_psShowPopup()-->IN");

    for(int i=0;i < 16;i++){
        funcShowPopup((PopupType)i);

        delayAfterCallAPI();
        sleep(2);
    }


    return isAppMgrExist();
}

bool testCase_psShowPopup(void){
    logVerbose(SDKTEST,"testCase_psShowPopup()-->IN");

    funcShowPopup((PopupType)getRand(0,15));

    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_psClosePopup(void){
    logVerbose(SDKTEST,"testCase_psClosePopup()-->IN");
    if(!sPopupVec.empty()){
        PopupID popupID = sPopupVec.back();
        sPopupVec.pop_back();
        psClosePopup(popupID);
    }

    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_GetDestinationRectangle(){
    logVerbose(SDKTEST,"testCase_GetDestinationRectangle()-->IN");
    GetDestinationRectangle(app_x,app_y,app_width,app_height);
    delayAfterCallAPI();

    return isAppMgrExist();
}
bool testCase_SetDestinationRectangle(){
    logVerbose(SDKTEST,"testCase_SetDestinationRectangle()-->IN");
    SetDestinationRectangle(getRand(0,1280),getRand(0,720),getRand(0,1280),getRand(0,720));
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_GetSourceRectangle(){
    logVerbose(SDKTEST,"testCase_GetSourceRectangle()-->IN");
    GetSourceRectangle(app_x,app_y,app_width,app_height);
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_SetSourceRectangle(){
    logVerbose(SDKTEST,"testCase_SetSourceRectangle()-->IN");
    SetSourceRectangle(getRand(0,1280),getRand(0,720),getRand(0,1280),getRand(0,720));
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_SetBothRectangle(){
    logVerbose(SDKTEST,"testCase_SetBothRectangle()-->IN");
    SetBothRectangle(getRand(0,1280),getRand(0,720),getRand(0,1280),getRand(0,720));
    delayAfterCallAPI();

    return isAppMgrExist();
}


bool testCase_SetUnderLayerCenter(void){
    logVerbose(SDKTEST,"testCase_SetUnderLayerCenter()-->IN");
    SetUnderLayerCenter(getRand(0,1));
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_IsUnderLayerCenter(void){
    logVerbose(SDKTEST,"testCase_IsUnderLayerCenter()-->IN");
    IsUnderLayerCenter();
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_ShowTitlebar(void){
    logVerbose(SDKTEST,"testCase_ShowTitlebar()-->IN");
    ShowTitlebar();
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_HideTitlebar(void){
    logVerbose(SDKTEST,"testCase_HideTitlebar()-->IN");
    HideTitlebar();
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_IsTitlebarVisible(void){
    logVerbose(SDKTEST,"testCase_IsTitlebarVisible()-->IN");
    IsTitlebarVisible();
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_EnableTouch(void){
    logVerbose(SDKTEST,"testCase_EnableTouch()-->IN");
    EnableTouch(getRand(0,1));
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_SM_getState(void){
    logVerbose(SDKTEST,"testCase_SM_getState()-->IN");
    int index = getRand(0,(sStateMap.size()-1));
    int i = 0;
    for(std::multimap<string,string>::iterator it=sStateMap.begin();it!=sStateMap.end();++it){
        i++;
        if(i==index){
            if(SM_isStateExist(it->first)){
                SM_getState(it->first);
            }
        }
    }
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_SM_req2SetState(void){
    logVerbose(SDKTEST,"testCase_SM_req2SetState()-->IN");
    int index = getRand(0,(sStateMap.size()-1));
    int i = 0;
    for(std::multimap<string,string>::iterator it=sStateMap.begin();it!=sStateMap.end();++it){
        i++;
        if(i==index){
            if(SM_isStateExist(it->first)){
                SM_req2SetState(it->first,it->second);
            }
        }
    }
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_SM_updateState(void){
    logVerbose(SDKTEST,"testCase_SM_updateState()-->IN");
    int index = getRand(0,(sStateMap.size()-1));
    int i = 0;
    for(std::multimap<string,string>::iterator it=sStateMap.begin();it!=sStateMap.end();++it){
        i++;
        if(i==index){
            if(SM_isStateExist(it->first)){
                SM_updateState(getRandomAMPID(),it->first,it->second);
            }
        }
    }
    delayAfterCallAPI();

    return isAppMgrExist();
}

void createStateMapForTestCase(void){
    sStateMap.insert(make_pair("syssetting_IVI_RADAR_VOL","15"));
    sStateMap.insert(make_pair("syssetting_IVI_RADAR_VOL","20"));
    sStateMap.insert(make_pair("syssetting_IVI_RADAR_VOL","30"));

    sStateMap.insert(make_pair("syssetting_RSE_RADAR_VOL","15"));
    sStateMap.insert(make_pair("syssetting_RSE_RADAR_VOL","22"));
    sStateMap.insert(make_pair("syssetting_RSE_RADAR_VOL","30"));

    sStateMap.insert(make_pair("syssetting_IVI_ECALL_VOL","15"));
    sStateMap.insert(make_pair("syssetting_IVI_ECALL_VOL","23"));
    sStateMap.insert(make_pair("syssetting_IVI_ECALL_VOL","30"));

    sStateMap.insert(make_pair("syssetting_IVI_PHONE_VOL","0"));
    sStateMap.insert(make_pair("syssetting_IVI_PHONE_VOL","10"));
    sStateMap.insert(make_pair("syssetting_IVI_PHONE_VOL","30"));

    sStateMap.insert(make_pair("syssetting_IVI_INFORMING_VOL","0"));
    sStateMap.insert(make_pair("syssetting_IVI_INFORMING_VOL","14"));
    sStateMap.insert(make_pair("syssetting_IVI_INFORMING_VOL","30"));

    sStateMap.insert(make_pair("syssetting_IVI_SOFTKEY_VOL","0"));
    sStateMap.insert(make_pair("syssetting_IVI_SOFTKEY_VOL","16"));
    sStateMap.insert(make_pair("syssetting_IVI_SOFTKEY_VOL","30"));

    sStateMap.insert(make_pair("syssetting_RSE1_SOFTKEY_VOL","0"));
    sStateMap.insert(make_pair("syssetting_RSE1_SOFTKEY_VOL","13"));
    sStateMap.insert(make_pair("syssetting_RSE1_SOFTKEY_VOL","30"));

    sStateMap.insert(make_pair("syssetting_RSE2_SOFTKEY_VOL","0"));
    sStateMap.insert(make_pair("syssetting_RSE2_SOFTKEY_VOL","17"));
    sStateMap.insert(make_pair("syssetting_RSE2_SOFTKEY_VOL","30"));

    sStateMap.insert(make_pair("syssetting_IVI_VRTTS_VOL","0"));
    sStateMap.insert(make_pair("syssetting_IVI_VRTTS_VOL","12"));
    sStateMap.insert(make_pair("syssetting_IVI_VRTTS_VOL","30"));

    sStateMap.insert(make_pair("syssetting_IVI_SMSTTS_VOL","0"));
    sStateMap.insert(make_pair("syssetting_IVI_SMSTTS_VOL","18"));
    sStateMap.insert(make_pair("syssetting_IVI_SMSTTS_VOL","30"));

    sStateMap.insert(make_pair("syssetting_IVI_NAVITTS_VOL","0"));
    sStateMap.insert(make_pair("syssetting_IVI_NAVITTS_VOL","11"));
    sStateMap.insert(make_pair("syssetting_IVI_NAVITTS_VOL","30"));

    sStateMap.insert(make_pair("syssetting_IVI_MEDIA_VOL","0"));
    sStateMap.insert(make_pair("syssetting_IVI_MEDIA_VOL","19"));
    sStateMap.insert(make_pair("syssetting_IVI_MEDIA_VOL","30"));

    sStateMap.insert(make_pair("syssetting_RSE1_MEDIA_VOL","0"));
    sStateMap.insert(make_pair("syssetting_RSE1_MEDIA_VOL","10"));
    sStateMap.insert(make_pair("syssetting_RSE1_MEDIA_VOL","30"));

    sStateMap.insert(make_pair("syssetting_RSE2_MEDIA_VOL","0"));
    sStateMap.insert(make_pair("syssetting_RSE2_MEDIA_VOL","20"));
    sStateMap.insert(make_pair("syssetting_RSE2_MEDIA_VOL","30"));

    sStateMap.insert(make_pair("syssetting_IVI_SOUND_FIELD","0"));
    sStateMap.insert(make_pair("syssetting_IVI_SOUND_FIELD","1"));
    sStateMap.insert(make_pair("syssetting_IVI_SOUND_FIELD","2"));
    sStateMap.insert(make_pair("syssetting_IVI_SOUND_FIELD","3"));

    sStateMap.insert(make_pair("syssetting_IVI_SOUND_BALANCE","-6"));
    sStateMap.insert(make_pair("syssetting_IVI_SOUND_BALANCE","-2"));
    sStateMap.insert(make_pair("syssetting_IVI_SOUND_BALANCE","6"));

    sStateMap.insert(make_pair("syssetting_IVI_SOUND_FADER","-6"));
    sStateMap.insert(make_pair("syssetting_IVI_SOUND_FADER","3"));
    sStateMap.insert(make_pair("syssetting_IVI_SOUND_FADER","6"));

    sStateMap.insert(make_pair("syssetting_IVI_AUDIO_HIGH","-6"));
    sStateMap.insert(make_pair("syssetting_IVI_AUDIO_HIGH","-4"));
    sStateMap.insert(make_pair("syssetting_IVI_AUDIO_HIGH","6"));

    sStateMap.insert(make_pair("syssetting_RSE1_AUDIO_HIGH","-6"));
    sStateMap.insert(make_pair("syssetting_RSE1_AUDIO_HIGH","2"));
    sStateMap.insert(make_pair("syssetting_RSE1_AUDIO_HIGH","6"));

    sStateMap.insert(make_pair("syssetting_RSE2_AUDIO_HIGH","-6"));
    sStateMap.insert(make_pair("syssetting_RSE2_AUDIO_HIGH","-1"));
    sStateMap.insert(make_pair("syssetting_RSE2_AUDIO_HIGH","6"));

    sStateMap.insert(make_pair("syssetting_IVI_AUDIO_MID","-6"));
    sStateMap.insert(make_pair("syssetting_IVI_AUDIO_MID","0"));
    sStateMap.insert(make_pair("syssetting_IVI_AUDIO_MID","6"));

    sStateMap.insert(make_pair("syssetting_RSE1_AUDIO_MID","-6"));
    sStateMap.insert(make_pair("syssetting_RSE1_AUDIO_MID","5"));
    sStateMap.insert(make_pair("syssetting_RSE1_AUDIO_MID","6"));

    sStateMap.insert(make_pair("syssetting_RSE2_AUDIO_MID","-6"));
    sStateMap.insert(make_pair("syssetting_RSE2_AUDIO_MID","-4"));
    sStateMap.insert(make_pair("syssetting_RSE2_AUDIO_MID","6"));

    sStateMap.insert(make_pair("syssetting_IVI_AUDIO_LOW","-6"));
    sStateMap.insert(make_pair("syssetting_IVI_AUDIO_LOW","-2"));
    sStateMap.insert(make_pair("syssetting_IVI_AUDIO_LOW","6"));

    sStateMap.insert(make_pair("syssetting_RSE1_AUDIO_LOW","-6"));
    sStateMap.insert(make_pair("syssetting_RSE1_AUDIO_LOW","-1"));
    sStateMap.insert(make_pair("syssetting_RSE1_AUDIO_LOW","6"));

    sStateMap.insert(make_pair("syssetting_RSE2_AUDIO_LOW","-6"));
    sStateMap.insert(make_pair("syssetting_RSE2_AUDIO_LOW","0"));
    sStateMap.insert(make_pair("syssetting_RSE2_AUDIO_LOW","6"));

    sStateMap.insert(make_pair("syssetting_IVI_EAX","0"));
    sStateMap.insert(make_pair("syssetting_IVI_EAX","1"));
    sStateMap.insert(make_pair("syssetting_IVI_EAX","2"));

    sStateMap.insert(make_pair("syssetting_RSE1_EAX","0"));
    sStateMap.insert(make_pair("syssetting_RSE1_EAX","1"));
    sStateMap.insert(make_pair("syssetting_RSE1_EAX","2"));

    sStateMap.insert(make_pair("syssetting_RSE2_EAX","0"));
    sStateMap.insert(make_pair("syssetting_RSE2_EAX","1"));
    sStateMap.insert(make_pair("syssetting_RSE2_EAX","2"));

    sStateMap.insert(make_pair("syssetting_IVI_PHNA_SWIHCH","0"));
    sStateMap.insert(make_pair("syssetting_IVI_PHNA_SWIHCH","1"));

    sStateMap.insert(make_pair("syssetting_NAV_MED_MEDVOL","0"));
    sStateMap.insert(make_pair("syssetting_NAV_MED_MEDVOL","2"));
    sStateMap.insert(make_pair("syssetting_NAV_MED_MEDVOL","30"));

    sStateMap.insert(make_pair("syssetting_RAD_MED_MEDVOL","0"));
    sStateMap.insert(make_pair("syssetting_RAD_MED_MEDVOL","22"));
    sStateMap.insert(make_pair("syssetting_RAD_MED_MEDVOL","30"));

    sStateMap.insert(make_pair("syssetting_PHO_NAV_NAVVOL","0"));
    sStateMap.insert(make_pair("syssetting_PHO_NAV_NAVVOL","13"));
    sStateMap.insert(make_pair("syssetting_PHO_NAV_NAVVOL","30"));

    sStateMap.insert(make_pair("syssetting_IVI_RADAR_TONE","1"));
    sStateMap.insert(make_pair("syssetting_IVI_RADAR_TONE","2"));
    sStateMap.insert(make_pair("syssetting_IVI_RADAR_TONE","3"));

    sStateMap.insert(make_pair("syssetting_RSE_RADAR_TONE","1"));
    sStateMap.insert(make_pair("syssetting_RSE_RADAR_TONE","2"));
    sStateMap.insert(make_pair("syssetting_RSE_RADAR_TONE","3"));

    sStateMap.insert(make_pair("syssetting_FAST_CHARGE_SET","1"));
    sStateMap.insert(make_pair("syssetting_FAST_CHARGE_SET","2"));
    sStateMap.insert(make_pair("syssetting_FAST_CHARGE_SET","3"));

    sStateMap.insert(make_pair("syssetting_SP_WHOSE","1"));
    sStateMap.insert(make_pair("syssetting_SP_WHOSE","2"));
    sStateMap.insert(make_pair("syssetting_SP_WHOSE","3"));

    sStateMap.insert(make_pair("syssetting_SYS_TIME","00:00:00"));
    sStateMap.insert(make_pair("syssetting_SYS_TIME","12:55:31"));
    sStateMap.insert(make_pair("syssetting_SYS_TIME","08:23:31"));

    sStateMap.insert(make_pair("syssetting_SYS_TIME_FORMAT","1"));
    sStateMap.insert(make_pair("syssetting_SYS_TIME_FORMAT","0"));

    sStateMap.insert(make_pair("syssetting_SYS_DATE","2010/12/22"));
    sStateMap.insert(make_pair("syssetting_SYS_DATE","2050/05/23"));
    sStateMap.insert(make_pair("syssetting_SYS_DATE","2022/03/12"));

    sStateMap.insert(make_pair("syssetting_SYS_DATE_FORMAT","0"));
    sStateMap.insert(make_pair("syssetting_SYS_DATE_FORMAT","1"));
    sStateMap.insert(make_pair("syssetting_SYS_DATE_FORMAT","2"));

    sStateMap.insert(make_pair("syssetting_SYS_LANG","zh_CN.utf-8"));

    sStateMap.insert(make_pair("syssetting_POL_SIG","0"));
    sStateMap.insert(make_pair("syssetting_POL_SIG","1"));

    sStateMap.insert(make_pair("syssetting_BRIGHT_MODE","0"));
    sStateMap.insert(make_pair("syssetting_BRIGHT_MODE","1"));
    sStateMap.insert(make_pair("syssetting_BRIGHT_MODE","2"));

    sStateMap.insert(make_pair("syssetting_IVI_BRIGHT_DTMODE","0"));
    sStateMap.insert(make_pair("syssetting_IVI_BRIGHT_DTMODE","5"));
    sStateMap.insert(make_pair("syssetting_IVI_BRIGHT_DTMODE","10"));

    sStateMap.insert(make_pair("syssetting_RSE1_BRIGHT_DTMODE","0"));
    sStateMap.insert(make_pair("syssetting_RSE1_BRIGHT_DTMODE","3"));
    sStateMap.insert(make_pair("syssetting_RSE1_BRIGHT_DTMODE","10"));

    sStateMap.insert(make_pair("syssetting_RSE2_BRIGHT_DTMODE","0"));
    sStateMap.insert(make_pair("syssetting_RSE2_BRIGHT_DTMODE","2"));
    sStateMap.insert(make_pair("syssetting_RSE2_BRIGHT_DTMODE","10"));

    sStateMap.insert(make_pair("syssetting_IVI_BRIGHT_NTMODE","0"));
    sStateMap.insert(make_pair("syssetting_IVI_BRIGHT_NTMODE","6"));
    sStateMap.insert(make_pair("syssetting_IVI_BRIGHT_NTMODE","10"));

    sStateMap.insert(make_pair("syssetting_RSE1_BRIGHT_NTMODE","0"));
    sStateMap.insert(make_pair("syssetting_RSE1_BRIGHT_NTMODE","1"));
    sStateMap.insert(make_pair("syssetting_RSE1_BRIGHT_NTMODE","10"));

    sStateMap.insert(make_pair("syssetting_RSE2_BRIGHT_NTMODE","0"));
    sStateMap.insert(make_pair("syssetting_RSE2_BRIGHT_NTMODE","9"));
    sStateMap.insert(make_pair("syssetting_RSE2_BRIGHT_NTMODE","10"));

    sStateMap.insert(make_pair("syssetting_IVI_BRIGHT_DTMODE","0"));
    sStateMap.insert(make_pair("syssetting_IVI_BRIGHT_DTMODE","5"));
    sStateMap.insert(make_pair("syssetting_IVI_BRIGHT_DTMODE","10"));

    sStateMap.insert(make_pair("syssetting_IVI_VIDEOSCR_RATIO","0"));
    sStateMap.insert(make_pair("syssetting_IVI_VIDEOSCR_RATIO","1"));

    sStateMap.insert(make_pair("syssetting_RSE1_VIDEOSCR_RATIO","0"));
    sStateMap.insert(make_pair("syssetting_RSE1_VIDEOSCR_RATIO","1"));

    sStateMap.insert(make_pair("syssetting_RSE2_VIDEOSCR_RATIO","0"));
    sStateMap.insert(make_pair("syssetting_RSE2_VIDEOSCR_RATIO","1"));

    sStateMap.insert(make_pair("syssetting_GPS_VALID","0"));
    sStateMap.insert(make_pair("syssetting_GPS_VALID","1"));

    sStateMap.insert(make_pair("syssetting_IVI_MUTE_SWITCH","0"));
    sStateMap.insert(make_pair("syssetting_IVI_MUTE_SWITCH","1"));

    sStateMap.insert(make_pair("syssetting_RSE1_MUTE_SWITCH","0"));
    sStateMap.insert(make_pair("syssetting_RSE1_MUTE_SWITCH","1"));

    sStateMap.insert(make_pair("syssetting_RSE2_MUTE_SWITCH","0"));
    sStateMap.insert(make_pair("syssetting_RSE2_MUTE_SWITCH","1"));

    sStateMap.insert(make_pair("syssetting_USB_IVI_STATE","0"));
    sStateMap.insert(make_pair("syssetting_USB_IVI_STATE","1"));

    sStateMap.insert(make_pair("syssetting_USB_RSE_STATE","0"));
    sStateMap.insert(make_pair("syssetting_USB_RSE_STATE","1"));

    sStateMap.insert(make_pair("syssetting_USB_IVI_PATH","/tmp/0"));
    sStateMap.insert(make_pair("syssetting_USB_RSE_PATH","/tmp/1"));
    sStateMap.insert(make_pair("syssetting_HDD_PATH","/tmp/2"));
    sStateMap.insert(make_pair("syssetting_IVI_PLAY_FILE","/tmp/3"));
    sStateMap.insert(make_pair("syssetting_RSE1_PLAY_FILE","/tmp/4"));
    sStateMap.insert(make_pair("syssetting_RSE2_PLAY_FILE","/tmp/5"));

    sStateMap.insert(make_pair("syssetting_SCRSYNC_TRSE1_FIVI","0"));
    sStateMap.insert(make_pair("syssetting_SCRSYNC_TRSE1_FIVI","1"));

    sStateMap.insert(make_pair("syssetting_SCRSYNC_TRSE2_FIVI","0"));
    sStateMap.insert(make_pair("syssetting_SCRSYNC_TRSE2_FIVI","1"));

    sStateMap.insert(make_pair("syssetting_SCRSYNC_TRSE1_FRSE2","0"));
    sStateMap.insert(make_pair("syssetting_SCRSYNC_TRSE1_FRSE2","1"));

    sStateMap.insert(make_pair("syssetting_SCRSYNC_TRSE2_FRSE1","0"));
    sStateMap.insert(make_pair("syssetting_SCRSYNC_TRSE2_FRSE1","1"));

    sStateMap.insert(make_pair("syssetting_SPEAKER_DISABLED","0"));
    sStateMap.insert(make_pair("syssetting_SPEAKER_DISABLED","1"));

    sStateMap.insert(make_pair("syssetting_KIDS_MODE","0"));
    sStateMap.insert(make_pair("syssetting_KIDS_MODE","1"));

    sStateMap.insert(make_pair("syssetting_RSE_DISABLED","0"));
    sStateMap.insert(make_pair("syssetting_RSE_DISABLED","1"));

    sStateMap.insert(make_pair("syssetting_IVI_VIDEO_PLAYING","0"));
    sStateMap.insert(make_pair("syssetting_IVI_VIDEO_PLAYING","1"));

    sStateMap.insert(make_pair("syssetting_RSE1_VIDEO_PLAYING","0"));
    sStateMap.insert(make_pair("syssetting_RSE1_VIDEO_PLAYING","1"));

    sStateMap.insert(make_pair("syssetting_RSE2_VIDEO_PLAYING","0"));
    sStateMap.insert(make_pair("syssetting_RSE2_VIDEO_PLAYING","1"));

    sStateMap.insert(make_pair("syssetting_FEEDBACK_SOUND_SWITCH","0"));
    sStateMap.insert(make_pair("syssetting_FEEDBACK_SOUND_SWITCH","1"));

    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_MEDIA","0"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_MEDIA","1"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_MEDIA","2"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_MEDIA","3"));

    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_RADIO","0"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_RADIO","1"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_RADIO","2"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_RADIO","3"));

    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_NAVI","0"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_NAVI","1"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_NAVI","2"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_NAVI","3"));

    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_SYSSETTING","0"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_SYSSETTING","1"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_SYSSETTING","2"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_SYSSETTING","3"));

    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_BT","0"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_BT","1"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_BT","2"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_BT","3"));

    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_NET","0"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_NET","1"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_NET","2"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_NET","3"));

    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_HDD","0"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_HDD","1"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_HDD","2"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_HDD","3"));

    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_SOUND","0"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_SOUND","1"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_SOUND","2"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_SOUND","3"));

    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_DISPLAY","0"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_DISPLAY","1"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_DISPLAY","2"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_DISPLAY","3"));

    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_VEH","0"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_VEH","1"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_VEH","2"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_VEH","3"));

    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_BROWSER","0"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_BROWSER","1"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_BROWSER","2"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_BROWSER","3"));

    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_CLOUD","0"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_CLOUD","1"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_CLOUD","2"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_CLOUD","3"));

    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_THIRD","0"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_THIRD","1"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_THIRD","2"));
    sStateMap.insert(make_pair("syssetting_FACTORY_RESET_THIRD","3"));

    sStateMap.insert(make_pair("aircondition_AC_SWITCH","0"));
    sStateMap.insert(make_pair("aircondition_AC_SWITCH","1"));

    sStateMap.insert(make_pair("aircondition_AC_AUTO_SWITCH","0"));
    sStateMap.insert(make_pair("aircondition_AC_AUTO_SWITCH","1"));

    sStateMap.insert(make_pair("aircondition_AC_COMPRESSOR_SWITCH","0"));
    sStateMap.insert(make_pair("aircondition_AC_COMPRESSOR_SWITCH","1"));

    sStateMap.insert(make_pair("aircondition_AC_NATURAL_SWITCH","0"));
    sStateMap.insert(make_pair("aircondition_AC_NATURAL_SWITCH","1"));

    sStateMap.insert(make_pair("aircondition_AC_SYNC_SWITCH","0"));
    sStateMap.insert(make_pair("aircondition_AC_SYNC_SWITCH","1"));

    sStateMap.insert(make_pair("aircondition_AC_DEF_SWITCH","0"));
    sStateMap.insert(make_pair("aircondition_AC_DEF_SWITCH","1"));

    sStateMap.insert(make_pair("aircondition_AC_WIND_MODE","0"));
    sStateMap.insert(make_pair("aircondition_AC_WIND_MODE","1"));
    sStateMap.insert(make_pair("aircondition_AC_WIND_MODE","2"));
    sStateMap.insert(make_pair("aircondition_AC_WIND_MODE","3"));

    sStateMap.insert(make_pair("aircondition_AC_WIND_LEVEL","0"));
    sStateMap.insert(make_pair("aircondition_AC_WIND_LEVEL","1"));
    sStateMap.insert(make_pair("aircondition_AC_WIND_LEVEL","2"));
    sStateMap.insert(make_pair("aircondition_AC_WIND_LEVEL","3"));


    sStateMap.insert(make_pair("syssetting_FEEDBACK_SOUND_SWITCH","0"));
    sStateMap.insert(make_pair("syssetting_FEEDBACK_SOUND_SWITCH","1"));

    sStateMap.insert(make_pair("syssetting_FEEDBACK_SOUND_SWITCH","0"));
    sStateMap.insert(make_pair("syssetting_FEEDBACK_SOUND_SWITCH","1"));

    sStateMap.insert(make_pair("syssetting_FEEDBACK_SOUND_SWITCH","0"));
    sStateMap.insert(make_pair("syssetting_FEEDBACK_SOUND_SWITCH","1"));

    sStateMap.insert(make_pair("syssetting_FEEDBACK_SOUND_SWITCH","0"));
    sStateMap.insert(make_pair("syssetting_FEEDBACK_SOUND_SWITCH","1"));

    sStateMap.insert(make_pair("syssetting_FEEDBACK_SOUND_SWITCH","0"));
    sStateMap.insert(make_pair("syssetting_FEEDBACK_SOUND_SWITCH","1"));
}
