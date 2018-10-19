/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ConnectCtrl.cpp
/// @brief contains the implementation of class FocusController
///
/// Created by wangqi on 2016/6/16.
/// this file contains the implementation of class FocusController
///

#include "FocusCtrl.h"
#include "AudioCtrl.h"
#include "AudioSetting.h"


FocusController::FocusController()
:mLocalVRConID(0x0)
,mLoaclVRFocusSta(E_LOSE_TRANSIENT)
{
    logVerbose(AC,"FocusController::FocusController()-->IN");
    logVerbose(AC,"FocusController::FocusController()-->OUT");
}

FocusController::~FocusController()
{
    logVerbose(AC,"FocusController::~FocusController()-->IN");
    logVerbose(AC,"FocusController::~FocusController()-->OUT");
}

void FocusController::requestAudioDeviceFocus(ama_connectID_t connectID)
{
    logVerbose(AC,"FocusController::requestAudioDeviceFocus()-->IN connectID=", ToHEX(connectID));

    switch(GET_DEVICETYPE_FROM_CONNECTID(connectID)){
    case E_IVI_MIC:
            handleConCtrlForReqMic(connectID);
            break;
    case E_IVI_SPEAKER:
            handleConCtrlForReqSpeaker(connectID);
            break;
    case E_RSE_L_HEADSET:
            handleConCtrlForReqLHeadset(connectID);
            break;
    case E_RSE_R_HEADSET:
            handleConCtrlForReqRHeadset(connectID);
            break;
    default:
            logError(AC,"audioDevice is not defined ");
            break;
    }

    logVerbose(AC,"FocusController::requestAudioDeviceFocus()-->OUT");
}

void FocusController::releaseAudioDeviceFocus(ama_connectID_t connectID)
{
    logVerbose(AC,"FocusController::releaseAudioDeviceFocus()-->IN connectID=", ToHEX(connectID));

    switch(GET_DEVICETYPE_FROM_CONNECTID(connectID)){
    case E_IVI_MIC:
            handleConCtrlForRelMic(connectID);
            break;
    case E_IVI_SPEAKER:
            handleConCtrlForRelSpeaker(connectID);
            break;
    case E_RSE_L_HEADSET:
            handleConCtrlForRelLHeadset(connectID);
            break;
    case E_RSE_R_HEADSET:
            handleConCtrlForRelRHeadset(connectID);
            break;
    default:
            logError(AC,"audioDevice is not defined ");
            break;
    }

    logVerbose(AC,"FocusController::releaseAudioDeviceFocus()-->OUT");
}

//we need refresh focuse state when focus table changed(mIsPhoneAndNaviMix changed!)
//it's only change focus state of speaker
void FocusController::refreshFocusStaWhenSettingChanged()
{
    logVerbose(AC,"FocusController::releaseAudioDeviceFocus()-->IN");

    refreshFocusState(mSpeakerFocusVec,mSpeakerFocusMap);

    printfFocusMap(mSpeakerFocusMap);

    //exec disconnect the connection first
    for(std::map<ama_connectID_t,ama_focusSta_t>::iterator it=mSpeakerFocusMap.begin();
        it!=mSpeakerFocusMap.end();++it){
        if(!IS_ALREADY_HAVE_FOCUS(it->second)){
            tellConnectionToHandleNewFocusSta(it->first, it->second);
        }
    }

    //then exec connect the connection
    for(std::map<ama_connectID_t,ama_focusSta_t>::iterator it=mSpeakerFocusMap.begin();
        it!=mSpeakerFocusMap.end();++it){
        if(IS_ALREADY_HAVE_FOCUS(it->second)){
            tellConnectionToHandleNewFocusSta(it->first, it->second);
        }
    }

    //we remove the request which focusSta==E_FOCUSLOSE from vector and map
    //we do it after tellConnectionToHandleNewFocusSta
    removeLossFocusFromVecAndMap(mSpeakerFocusVec,mSpeakerFocusMap);

    logVerbose(AC,"FocusController::releaseAudioDeviceFocus()-->OUT");
}

void FocusController::tellConnectionToHandleNewFocusSta(ama_connectID_t connectID,ama_focusSta_t newFocusSta)
{
    logVerbose(AC,"FocusController::tellConnectionToHandleNewFocusSta()-->IN connectID=", ToHEX(connectID),"focusSta=", ToHEX(newFocusSta));

    auto pConnection = AudioController::GetInstance()->findConnectionFromMapByConnectID(connectID);
    if(pConnection){
        pConnection->handleNewFocusState(newFocusSta);
    }else{
        logError(AC,"this connection is't exist");
    }

    logVerbose(AC,"FocusController::tellConnectionToHandleNewFocusSta()-->OUT");
}

void FocusController::handleConCtrlForReqSpeaker(ama_connectID_t connectID)
{
    logVerbose(AC,"FocusController::handleConCtrlForReqSpeaker()-->IN connectID=", ToHEX(connectID));

    logInfo(ADBG,"before request audio focus");
    printfFocusMap(mSpeakerFocusMap);

    if(E_FOCUSLOSE == getFocusStaCompareWithVec(connectID,mSpeakerFocusVec)){
        tellConnectionToHandleNewFocusSta(connectID, E_FOCUSLOSE);
        logInfo(AC,"get focusSta==E_FOCUSLOSE,we don't need add connectID in vector");
        return;
    }

    //new focus control code
    addReqIntoFocusVec(connectID,mSpeakerFocusVec);
    refreshFocusState(mSpeakerFocusVec,mSpeakerFocusMap);

    logInfo(ADBG,"after request audio focus");
    printfFocusMap(mSpeakerFocusMap);

    //exec disconnect the connection first
    for(std::map<ama_connectID_t,ama_focusSta_t>::iterator it=mSpeakerFocusMap.begin();
        it!=mSpeakerFocusMap.end();++it){
        if(!IS_ALREADY_HAVE_FOCUS(it->second)){
            tellConnectionToHandleNewFocusSta(it->first, it->second);
        }
    }

    //then exec connect the connection
    for(std::map<ama_connectID_t,ama_focusSta_t>::iterator it=mSpeakerFocusMap.begin();
        it!=mSpeakerFocusMap.end();++it){
        if(IS_ALREADY_HAVE_FOCUS(it->second)){
            tellConnectionToHandleNewFocusSta(it->first, it->second);
        }
    }

    //we remove the request which focusSta==E_FOCUSLOSE from vector and map
    //we do it after tellConnectionToHandleNewFocusSta
    removeLossFocusFromVecAndMap(mSpeakerFocusVec,mSpeakerFocusMap);

    logVerbose(AC,"FocusController::handleConCtrlForReqSpeaker()-->OUT");
}

void FocusController::handleConCtrlForReqLHeadset(ama_connectID_t connectID)
{
    logVerbose(AC,"FocusController::handleConCtrlForReqLHeadset()-->IN connectID=", ToHEX(connectID));

    logInfo(ADBG,"before request audio focus");
    printfFocusMap(mLeftHeadsetFocusMap);

    if(E_FOCUSLOSE == getFocusStaCompareWithVec(connectID,mLeftHeadsetFocusVec)){
        tellConnectionToHandleNewFocusSta(connectID, E_FOCUSLOSE);
        logInfo(AC,"get focusSta==E_FOCUSLOSE,we don't need add connectID in vector");
        return;
    }

    //new focus control code
    addReqIntoFocusVec(connectID,mLeftHeadsetFocusVec);
    refreshFocusState(mLeftHeadsetFocusVec,mLeftHeadsetFocusMap);

    logInfo(ADBG,"after request audio focus");
    printfFocusMap(mLeftHeadsetFocusMap);

    //exec disconnect the connection first
    for(std::map<ama_connectID_t,ama_focusSta_t>::iterator it=mLeftHeadsetFocusMap.begin();
        it!=mLeftHeadsetFocusMap.end();++it){
        if(!IS_ALREADY_HAVE_FOCUS(it->second)){
            tellConnectionToHandleNewFocusSta(it->first, it->second);
        }
    }

    //then exec connect the connection
    for(std::map<ama_connectID_t,ama_focusSta_t>::iterator it=mLeftHeadsetFocusMap.begin();
        it!=mLeftHeadsetFocusMap.end();++it){
        if(IS_ALREADY_HAVE_FOCUS(it->second)){
            tellConnectionToHandleNewFocusSta(it->first, it->second);
        }
    }

    //we remove the request which focusSta==E_FOCUSLOSE from vector and map
    //we do it after tellConnectionToHandleNewFocusSta
    removeLossFocusFromVecAndMap(mLeftHeadsetFocusVec,mLeftHeadsetFocusMap);

    logVerbose(AC,"FocusController::handleConCtrlForReqLHeadset()-->OUT");
}
void FocusController::handleConCtrlForRelLHeadset(ama_connectID_t connectID)
{
    logVerbose(AC,"FocusController::handleConCtrlForRelLHeadset()-->IN connectID=", ToHEX(connectID));

    //new focus control code
    removeReqFromFocusVec(connectID,mLeftHeadsetFocusVec);
    removeStaFromFocusMap(connectID,mLeftHeadsetFocusMap);
    tellConnectionToHandleNewFocusSta(connectID, E_FOCUSLOSE);
    refreshFocusState(mLeftHeadsetFocusVec,mLeftHeadsetFocusMap);

    printfFocusMap(mLeftHeadsetFocusMap);

    //exec disconnect the connection first
    for(std::map<ama_connectID_t,ama_focusSta_t>::iterator it=mLeftHeadsetFocusMap.begin();
        it!=mLeftHeadsetFocusMap.end();++it){
        if(!IS_ALREADY_HAVE_FOCUS(it->second)){
            tellConnectionToHandleNewFocusSta(it->first, it->second);
        }
    }

    //then exec connect the connection
    for(std::map<ama_connectID_t,ama_focusSta_t>::iterator it=mLeftHeadsetFocusMap.begin();
        it!=mLeftHeadsetFocusMap.end();++it){
        if(IS_ALREADY_HAVE_FOCUS(it->second)){
            tellConnectionToHandleNewFocusSta(it->first, it->second);
        }
    }

    //we remove the request which focusSta==E_FOCUSLOSE from vector and map
    //we do it after tellConnectionToHandleNewFocusSta
    removeLossFocusFromVecAndMap(mLeftHeadsetFocusVec,mLeftHeadsetFocusMap);

    logVerbose(AC,"FocusController::handleConCtrlForRelLHeadset()-->OUT");
}

void FocusController::handleConCtrlForReqRHeadset(ama_connectID_t connectID)
{
    logVerbose(AC,"FocusController::handleConCtrlForReqRHeadset()-->IN connectID=", ToHEX(connectID));

    logInfo(ADBG,"before request audio focus");
    printfFocusMap(mRightHeadsetFocusMap);

    if(E_FOCUSLOSE == getFocusStaCompareWithVec(connectID,mRightHeadsetFocusVec)){
        tellConnectionToHandleNewFocusSta(connectID, E_FOCUSLOSE);
        logInfo(AC,"get focusSta==E_FOCUSLOSE,we don't need add connectID in vector");
        return;
    }

    //new focus control code
    addReqIntoFocusVec(connectID,mRightHeadsetFocusVec);
    refreshFocusState(mRightHeadsetFocusVec,mRightHeadsetFocusMap);

    logInfo(ADBG,"after request audio focus");
    printfFocusMap(mRightHeadsetFocusMap);

    //exec disconnect the connection first
    for(std::map<ama_connectID_t,ama_focusSta_t>::iterator it=mRightHeadsetFocusMap.begin();
        it!=mRightHeadsetFocusMap.end();++it){
        if(!IS_ALREADY_HAVE_FOCUS(it->second)){
            tellConnectionToHandleNewFocusSta(it->first, it->second);
        }
    }

    //then exec connect the connection
    for(std::map<ama_connectID_t,ama_focusSta_t>::iterator it=mRightHeadsetFocusMap.begin();
        it!=mRightHeadsetFocusMap.end();++it){
        if(IS_ALREADY_HAVE_FOCUS(it->second)){
            tellConnectionToHandleNewFocusSta(it->first, it->second);
        }
    }

    //we remove the request which focusSta==E_FOCUSLOSE from vector and map
    //we do it after tellConnectionToHandleNewFocusSta
    removeLossFocusFromVecAndMap(mRightHeadsetFocusVec,mRightHeadsetFocusMap);

    logVerbose(AC,"FocusController::handleConCtrlForReqRHeadset()-->OUT");
}
void FocusController::handleConCtrlForRelRHeadset(ama_connectID_t connectID)
{
    logVerbose(AC,"FocusController::handleConCtrlForRelRHeadset()-->IN connectID=", ToHEX(connectID));

    //new focus control code
    removeReqFromFocusVec(connectID,mRightHeadsetFocusVec);
    removeStaFromFocusMap(connectID,mRightHeadsetFocusMap);
    tellConnectionToHandleNewFocusSta(connectID, E_FOCUSLOSE);
    refreshFocusState(mRightHeadsetFocusVec,mRightHeadsetFocusMap);

    printfFocusMap(mRightHeadsetFocusMap);

    //exec disconnect the connection first
    for(std::map<ama_connectID_t,ama_focusSta_t>::iterator it=mRightHeadsetFocusMap.begin();
        it!=mRightHeadsetFocusMap.end();++it){
        if(!IS_ALREADY_HAVE_FOCUS(it->second)){
            tellConnectionToHandleNewFocusSta(it->first, it->second);
        }
    }

    //then exec connect the connection
    for(std::map<ama_connectID_t,ama_focusSta_t>::iterator it=mRightHeadsetFocusMap.begin();
        it!=mRightHeadsetFocusMap.end();++it){
        if(IS_ALREADY_HAVE_FOCUS(it->second)){
            tellConnectionToHandleNewFocusSta(it->first, it->second);
        }
    }

    //we remove the request which focusSta==E_FOCUSLOSE from vector and map
    //we do it after tellConnectionToHandleNewFocusSta
    removeLossFocusFromVecAndMap(mRightHeadsetFocusVec,mRightHeadsetFocusMap);

    logVerbose(AC,"FocusController::handleConCtrlForRelRHeadset()-->OUT");
}

void FocusController::handleConCtrlForRelSpeaker(ama_connectID_t connectID)
{
    logVerbose(AC,"FocusController::handleConCtrlForRelSpeaker()-->IN connectID=", ToHEX(connectID));

    //new focus control code
    removeReqFromFocusVec(connectID,mSpeakerFocusVec);
    removeStaFromFocusMap(connectID,mSpeakerFocusMap);
    tellConnectionToHandleNewFocusSta(connectID, E_FOCUSLOSE);
    refreshFocusState(mSpeakerFocusVec,mSpeakerFocusMap);

    printfFocusMap(mSpeakerFocusMap);

    //exec disconnect the connection first
    for(std::map<ama_connectID_t,ama_focusSta_t>::iterator it=mSpeakerFocusMap.begin();
        it!=mSpeakerFocusMap.end();++it){
        if(!IS_ALREADY_HAVE_FOCUS(it->second)){
            tellConnectionToHandleNewFocusSta(it->first, it->second);
        }
    }

    //then exec connect the connection
    for(std::map<ama_connectID_t,ama_focusSta_t>::iterator it=mSpeakerFocusMap.begin();
        it!=mSpeakerFocusMap.end();++it){
        if(IS_ALREADY_HAVE_FOCUS(it->second)){
            tellConnectionToHandleNewFocusSta(it->first, it->second);
        }
    }

    //we remove the request which focusSta==E_FOCUSLOSE from vector and map
    //we do it after tellConnectionToHandleNewFocusSta
    removeLossFocusFromVecAndMap(mSpeakerFocusVec,mSpeakerFocusMap);

    logVerbose(AC,"FocusController::handleConCtrlForRelSpeaker()-->OUT");
}

void FocusController::addReqIntoFocusVec(ama_connectID_t connectID,vector<ama_connectID_t> &focusVec)
{
    logVerbose(AC,"FocusController::addReqIntoFocusVec()-->IN connectID=", ToHEX(connectID));

    for(std::vector<ama_connectID_t> ::iterator it = focusVec.begin(); it != focusVec.end(); ++it){
        if(*it == connectID){
            logVerbose(AC,"connectID already in focusVec");
            focusVec.erase(it);
            focusVec.push_back(connectID);
            return;
        }
    }

    logVerbose(AC,"not found connectID in focusVec");
    focusVec.push_back(connectID);

    if(focusVec.size()>0){
        logInfo(AC,"focusVec.size()>0");
        for(std::vector<ama_connectID_t> ::iterator it = focusVec.begin(); it != focusVec.end(); ++it){
            logInfo(AC,"connectID=", ToHEX(*it));
        }
    }else{
        logInfo(AC,"focusVec.size()=0");
    }

    logVerbose(AC,"FocusController::addReqIntoFocusVec()-->OUT");
}

void FocusController::removeReqFromFocusVec(ama_connectID_t connectID,std::vector<ama_connectID_t> &focusVec)
{
    logVerbose(AC,"FocusController::removeReqFromFocusVec()-->IN connectID=", ToHEX(connectID));

    for(std::vector<ama_connectID_t>::iterator it = focusVec.begin(); it != focusVec.end(); ++it){
        if(*it == connectID){
            logVerbose(AC,"remove connectID from focusVec");
            focusVec.erase(it);
            return;
        }
    }

    logError(AC,"can't found connectID in focusVec");

    logVerbose(AC,"FocusController::removeReqFromFocusVec()-->OUT");
}

void FocusController::removeStaFromFocusMap(ama_connectID_t connectID,map<ama_connectID_t,ama_focusSta_t> &focusStaMap)
{
    logVerbose(AC,"FocusController::removeStaFromFocusMap()-->IN connectID=", ToHEX(connectID));

    std::map<ama_connectID_t,ama_focusSta_t>::iterator it=focusStaMap.find(connectID);
    if(it!=focusStaMap.end()){
        focusStaMap.erase(it);
        logDebug(AC,"remove connectID from focusStaMap");
        logVerbose(AC,"FocusController::removeStaFromFocusMap()-->OUT");
        return;
    }

    logWarn(AC,"can't find connectID in focusStaMap");
    logVerbose(AC,"FocusController::removeStaFromFocusMap()-->OUT");
}

void FocusController::removeLossFocusFromVecAndMap(std::vector<ama_connectID_t> &focusVec,map<ama_connectID_t,ama_focusSta_t> &focusStaMap)
{
    logVerbose(AC,"FocusController::removeLossFocusFromVecAndMap()-->IN");

    ama_connectID_t connectID;
    ama_focusSta_t focusSta;
    for(std::map<ama_connectID_t,ama_focusSta_t>::iterator it=focusStaMap.begin();it!=focusStaMap.end();){\
        connectID = it->first;
        focusSta = it->second;
        logInfo(AC,"connectID:",ToHEX(connectID));
        logInfo(AC,"focusSta:",ToHEX(focusSta));
        if(E_FOCUSLOSE==focusSta){
            logInfo(AC,"remove connectID from focusMap :",ToHEX(connectID));
            removeReqFromFocusVec(connectID,focusVec);
            it = focusStaMap.erase(it);
        }else{
            ++it;
        }
    }

    logVerbose(AC,"FocusController::removeLossFocusFromVecAndMap()-->OUT");
}

void FocusController::refreshFocusState(std::vector<ama_connectID_t> &focusVec,map<ama_connectID_t,ama_focusSta_t> &focusStaMap)
{
    logVerbose(AC,"FocusController::refreshFocusState()-->IN ");

    ama_connectID_t connectID;
    ama_focusSta_t conSta;
    for(std::vector<ama_connectID_t> ::reverse_iterator it=focusVec.rbegin(); it!=focusVec.rend();++it){
        connectID = *it;
        conSta = getFocusStaInVec(connectID,focusVec);
        focusStaMap[connectID] = conSta;
        logInfo(AC,"connectID =",ToHEX(connectID));
        logInfo(AC,"conSta =", ToHEX(conSta));
    }

    logVerbose(AC,"FocusController::refreshFocusState()-->OUT");
}

ama_focusSta_t FocusController::getFocusStaInVec(ama_connectID_t connectID,vector<ama_connectID_t> &focusVec)
{
    logVerbose(AC,"FocusController::getFocusStaInVec()-->IN ");

    ama_focusSta_t conStaOfCompare;
    //we set lastConSta = E_FOUCSGAIN at first,then to check if it's a lower conSta
    ama_focusSta_t lastConSta = E_FOUCSGAIN;
    //because we reverse traversal,so we set compareMode = DEF_COMPARE_WITH_LATERCOMER
    int compareMode = DEF_COMPARE_WITH_LATERCOMER;
    for(std::vector<ama_connectID_t> ::reverse_iterator it = focusVec.rbegin(); it != focusVec.rend(); ++it){
        if(connectID == (*it)){
            //do nothing when connectID compare with itself
            //only change compare mode
            compareMode = DEF_COMPARE_WITH_EARLYCOMER;
        }else{
            //get focus state by focusTable
            //conStaOfCompare = FocusTable[GET_FOCUSTYPE_FROM_CONNECTID(connectID)][GET_FOCUSTYPE_FROM_CONNECTID(*it)];
            conStaOfCompare = getConSta(connectID,*it,compareMode);
            //if conStaOfCompare is E_FOCUSLOSE,
            if(E_FOCUSLOSE == conStaOfCompare){
                lastConSta = E_FOCUSLOSE;
                return lastConSta;
            }else if(conStaOfCompare < lastConSta){
                lastConSta = conStaOfCompare;
            }
        }
    }

    return lastConSta;
    logVerbose(AC,"FocusController::getFocusStaInVec()-->OUT");
}

ama_focusSta_t FocusController::getFocusStaCompareWithVec(ama_connectID_t connectID,vector<ama_connectID_t> &focusVec)
{
    logVerbose(AC,"FocusController::getFocusStaCompareWithVec()-->IN ");

    ama_focusSta_t conStaOfCompare;
    //we set lastConSta = E_FOUCSGAIN at first,then to check if it's a lower conSta
    ama_focusSta_t lastConSta = E_FOUCSGAIN;
    int compareMode = DEF_COMPARE_WITH_EARLYCOMER;
    for(std::vector<ama_connectID_t> ::reverse_iterator it = focusVec.rbegin(); it != focusVec.rend(); ++it){
        if(connectID == (*it)){
            logError(AC,"we use this function before we set connectID in vector,so we wouldn't compare with itself!");
        }else{
            conStaOfCompare = getConSta(connectID,*it,DEF_COMPARE_WITH_EARLYCOMER);
            //if conStaOfCompare is E_FOCUSLOSE,
            if(E_FOCUSLOSE == conStaOfCompare){
                lastConSta = E_FOCUSLOSE;
                return lastConSta;
            }else if(conStaOfCompare < lastConSta){
                lastConSta = conStaOfCompare;
            }
        }
    }

    logVerbose(AC,"FocusController::getFocusStaCompareWithVec()-->OUT");
    return lastConSta;
}

ama_focusSta_t FocusController::getConSta(ama_connectID_t connectID,ama_connectID_t cmpConnectID,int compareMode)
{
    //logVerbose(AC,"FocusController::getFocusStaInVec()-->IN");

    ama_focusType_t focusType = GET_FOCUSTYPE_FROM_CONNECTID(connectID);
    ama_focusType_t cmpFocusType = GET_FOCUSTYPE_FROM_CONNECTID(cmpConnectID);
    if((focusType>=E_FOCUS_TYPE_MAX)||(cmpFocusType>=E_FOCUS_TYPE_MAX)){
        logError(AC,"focusType >= E_FOCUS_TYPE_MAX!!!");
        return E_FOCUSLOSE;
    }

    if(AudioSetting::GetInstance()->isPhoneAndNaviMix()){
        if(DEF_COMPARE_WITH_LATERCOMER==compareMode){
            return FocusCmpWithLatercomerTable[focusType][cmpFocusType];
        }else{
            return FocusCmpWithEarlycomerTable[focusType][cmpFocusType];
        }
    }else{
        if(DEF_COMPARE_WITH_LATERCOMER==compareMode){
            return FocusCmpWithLatercomerTableNoMix[focusType][cmpFocusType];
        }else{
            return FocusCmpWithEarlycomerTableNoMix[focusType][cmpFocusType];
        }
    }

    //logVerbose(AC,"FocusController::getFocusStaInVec()-->OUT");
}

void FocusController::handleConCtrlForReqMic(ama_connectID_t connectID)
{
    logVerbose(AC,"FocusController::handleConCtrlForReqMic()-->IN");

    if(E_STREAM_LOCAL_VR_MIC==GET_STREAMTYPE_FROM_CONNECTID(connectID)){
        mLocalVRConID = connectID;
    }else{
        mMicFocusMap[connectID] = E_FOUCSGAIN;
    }

    if(mLocalVRConID!=0x0){
        if(!mMicFocusMap.empty()){
            mLoaclVRFocusSta = E_LOSE_TRANSIENT;
        }else{
            mLoaclVRFocusSta = E_FOUCSGAIN;
        }
        //if have a local vr stream ,dale it first
        logInfo(AC,"connectID:",mLocalVRConID," focusSta:",ToHEX(mLoaclVRFocusSta));
        tellConnectionToHandleNewFocusSta(mLocalVRConID, mLoaclVRFocusSta);
    }

    //then exec all connection
    for(std::map<ama_connectID_t,ama_focusSta_t>::iterator it=mMicFocusMap.begin();it!=mMicFocusMap.end();++it){
        tellConnectionToHandleNewFocusSta(it->first, it->second);
        logInfo(AC,"connectID:",it->first," focusSta:",ToHEX(it->second));
    }

    logVerbose(AC,"FocusController::handleConCtrlForReqMic()-->OUT");
}

void FocusController::handleConCtrlForRelMic(ama_connectID_t connectID)
{
    logVerbose(AC,"FocusController::handleConCtrlForRelMic()-->IN");

    if(connectID==mLocalVRConID){
        mLocalVRConID = 0x0;
        mLoaclVRFocusSta = E_FOCUSLOSE;
        tellConnectionToHandleNewFocusSta(connectID, E_FOCUSLOSE);
    }else{
        std::map<ama_connectID_t,ama_focusSta_t>::iterator it;
        it = mMicFocusMap.find(connectID);
        if(it != mMicFocusMap.end()){
            logInfo(AC,"delete connectID from mSpeakerFocusMap");
            mMicFocusMap.erase(it);
            tellConnectionToHandleNewFocusSta(connectID, E_FOCUSLOSE);
        }else{
            logError(AC,"can't find connectID in mSpeakerFocusMap");
            return;
        }

        //update local VR Focus State
        if(mLocalVRConID!=0x0){
            if(!mMicFocusMap.empty()){
                mLoaclVRFocusSta = E_LOSE_TRANSIENT;
            }else{
                mLoaclVRFocusSta = E_FOUCSGAIN;
            }
            //if have a local vr stream ,handle it first
            tellConnectionToHandleNewFocusSta(mLocalVRConID, mLoaclVRFocusSta);
        }
    }

    logVerbose(AC,"FocusController::handleConCtrlForRelMic()-->OUT");
}

bool FocusController::isVolInfoHadFocus(ama_volumeInfo_t volInfo)
{
    logVerbose(AC,"FocusController::isVolInfoHadFocus()-->IN volInfo:",ToHEX(volInfo));

    if(GET_DEVICETYPE_FROM_CONNECTID(volInfo) == E_IVI_SPEAKER){
        for(std::vector<ama_connectID_t> ::iterator it = mSpeakerFocusVec.begin(); it != mSpeakerFocusVec.end(); ++it){
            if((GET_VOLUMEINFO_FROM_CONNECTID((*it)))==volInfo){
                logVerbose(AC,"FocusController::isVolInfoHadFocus()-->OUT isExist == true");
                return true;
            }
        }
        logVerbose(AC,"FocusController::isVolInfoHadFocus()-->OUT isExist == false");
        return false;
    }else{
        logWarn(AC,"only speaker have mix situation");
        return false;
    }
}

void FocusController::debugShowFocusSta(void)
{
    logVerbose(DBG,"FocusController::debugShowFocusSta()-->IN");

    int index = 0;
    std::map<ama_connectID_t,ama_focusSta_t>::iterator it;

    logDebug(DBG,"show speaker focus sta-->>");
    index = 0;
    for(it=mSpeakerFocusMap.begin();it!=mSpeakerFocusMap.end();++it,++index){
        logDebug(DBG,"speakerFocus index:",index);
        logDebug(DBG,"speakerFocus connectID:", ToHEX(it->first));
        logDebug(DBG,"speakerFocus focusState:", ToHEX(it->second));
    }

    logDebug(DBG,"show left headset focus sta-->>");
    index = 0;
    for(it=mLeftHeadsetFocusMap.begin();it!=mLeftHeadsetFocusMap.end();++it,++index){
        logDebug(DBG,"LheadsetFocus index:",index);
        logDebug(DBG,"LheadsetFocus connectID:", ToHEX(it->first));
        logDebug(DBG,"LheadsetFocus focusState:", ToHEX(it->second));
    }

    logDebug(DBG,"show right headset focus sta-->>");
    index = 0;
    for(it=mRightHeadsetFocusMap.begin();it!=mRightHeadsetFocusMap.end();++it,++index){
        logDebug(DBG,"RheadsetFocus index:",index);
        logDebug(DBG,"RheadsetFocus connectID:", ToHEX(it->first));
        logDebug(DBG,"RheadsetFocus focusState:", ToHEX(it->second));
    }

    logDebug(DBG,"show mic focus sta-->>");
    index = 0;
    for(it=mMicFocusMap.begin();it!=mMicFocusMap.end();++it,++index){
        logDebug(DBG,"micFocus index:",index);
        logDebug(DBG,"micFocus connectID:", ToHEX(it->first));
        logDebug(DBG,"micFocus focusState:", ToHEX(it->second));
    }

    logVerbose(DBG,"FocusController::debugShowFocusSta()-->OUT");
}

void FocusController::printfFocusMap(std::map<ama_connectID_t,ama_focusSta_t> focusStaMap)
{
    logVerbose(DBG,"FocusController::printfFocusMap()-->IN");

    int index = 0;
    std::map<ama_connectID_t,ama_focusSta_t>::iterator it;

    index = 0;
    for(it=focusStaMap.begin();it!=focusStaMap.end();++it,++index){
        logInfo(DBG,"focus index:",index);
        logInfo(DBG,"focus connectID:", ToHEX(it->first));
        logInfo(DBG,"focus focusState:", ToHEX(it->second));
        logInfoF(ADBG,"<index[%d]>connectID[0x%x]:foucsSta[0x%x]",index,it->first,it->second);
    }

    logVerbose(DBG,"FocusController::printfFocusMap()-->OUT");
}
