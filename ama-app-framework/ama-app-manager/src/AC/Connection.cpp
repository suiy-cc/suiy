/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Connection.cpp
/// @brief contains the implementation of class Connection
///
/// Created by wangqi on 2016/11/30.
/// this file contains the implementation of class Connection
///

#include "Connection.h"
#include "FocusCtrl.h"
#include "AMClient.h"
#include "AudioSetting.h"
#include "AudioCtrl.h"
#include "ConCtrl.h"

Connection::Connection(ama_connectID_t connectID)
{
    logVerbose(AC,"Connection::Connection()-->IN");

    mConnectID = connectID;

    //we need set default focusSta=unknow,because we must tell App focus state first time
    //if we see default focusSta=discon,when ConCtrl set foucs state = discon,there will be a bug
    mFocusSta = E_FOCUSUNKNOW;
    mIsStreamMute = false;

    logVerbose(AC,"Connection::Connection()-->OUT");
}

Connection::~Connection()
{
    logVerbose(AC,"Connection::~Connection()-->IN");
    logVerbose(AC,"Connection::~Connection()-->OUT");
}

void Connection::requestAudioDeviceFocus()
{
    logVerbose(AC,"Connection::requestAudioDeviceFocus()-->IN");

    if(IS_ALREADY_HAVE_FOCUS(mFocusSta)){
        logWarn(AC,"connection already have get foucs");
    }else{
        FocusController::GetInstance()->requestAudioDeviceFocus(mConnectID);
    }

    logVerbose(AC,"Connection::requestAudioDeviceFocus()-->OUT");
}
void Connection::releaseAudioDeviceFocus()
{
    logVerbose(AC,"Connection::releaseAudioDeviceFocus()-->IN");

    //it can abandon focus anytime
    FocusController::GetInstance()->releaseAudioDeviceFocus(mConnectID);

    logVerbose(AC,"Connection::releaseAudioDeviceFocus()-->OUT");
}

void Connection::setStreamMute(bool isMute)
{
    logVerbose(AC,"Connection::setStreamMute()-->IN isMute=",isMute);

    mIsStreamMute = isMute;
    setVolume();

    logVerbose(AC,"Connection::setStreamMute()-->OUT");
}

void Connection::handleNewFocusState(ama_focusSta_t newFocusSta)
{
    logVerbose(AC,"Connection::handleNewFocusState()-->IN newFocusSta=", ToHEX(newFocusSta));

    int ret;
    logInfo(AC,"oldFocusState=", ToHEX(mFocusSta));

    if(newFocusSta == E_FOCUSUNKNOW){
        logError(AC,"ConnectCtrl return a unknow focus state!");
        logVerbose(AC,"Connection::handleNewFocusState()-->OUT");
        return;
    }

    auto pStream = AudioController::GetInstance()->findStreamFromMapByStreamID(GET_STREAMID_FROM_CONNECTID(mConnectID));
    if(pStream){
        if(mFocusSta != newFocusSta){
            logInfo(AC,"focus state changed");
            ama_focusSta_t oldFocusSta = mFocusSta;
            //we record newFocus state,we must do it here (before setvolume!)
            //because we will use mFocusSta when set volume
            mFocusSta = newFocusSta;
            logInfo(AC,"old focus state:",ToHEX(oldFocusSta));
            logInfo(AC,"new focus state:",ToHEX(newFocusSta));
            //we need handle with all situations here,there are 9 situations
            if(     ((oldFocusSta==E_FOCUSUNKNOW)&&(newFocusSta==E_GAIN_FADE))      //E_FOCUSUNKNOW->E_GAIN_FADE
                ||  ((oldFocusSta==E_FOCUSUNKNOW)&&(newFocusSta==E_FOUCSGAIN))    //E_FOCUSUNKNOW->E_FOUCSGAIN
                ||  ((oldFocusSta==E_FOCUSLOSE)&&(newFocusSta==E_GAIN_FADE))       //E_FOCUSLOSE->E_GAIN_FADE
                ||  ((oldFocusSta==E_FOCUSLOSE)&&(newFocusSta==E_FOUCSGAIN))     //E_FOCUSLOSE->E_FOUCSGAIN
                ||  ((oldFocusSta==E_LOSE_TRANSIENT)&&(newFocusSta==E_GAIN_FADE))   //E_LOSE_TRANSIENT->E_GAIN_FADE
                ||  ((oldFocusSta==E_LOSE_TRANSIENT)&&(newFocusSta==E_FOUCSGAIN)) //E_LOSE_TRANSIENT->E_FOUCSGAIN
            ){
                //try to connect them when we got audioFocus
                requestConnect();
                setVolume();

                logDebug(AC,"notify to get audio focus");
                pStream->notifyFocusStateChanged(mConnectID,convert2AppFoucsSta(oldFocusSta),convert2AppFoucsSta(newFocusSta));
            }else if(((oldFocusSta==E_GAIN_FADE)&&(newFocusSta==E_FOCUSLOSE))      //E_GAIN_FADE->E_FOCUSLOSE
                ||  ((oldFocusSta==E_FOUCSGAIN)&&(newFocusSta==E_FOCUSLOSE))     //E_FOUCSGAIN->E_FOCUSLOSE
                ||  ((oldFocusSta==E_GAIN_FADE)&&(newFocusSta==E_LOSE_TRANSIENT))     //E_GAIN_FADE->E_LOSE_TRANSIENT
                ||  ((oldFocusSta==E_FOUCSGAIN)&&(newFocusSta==E_LOSE_TRANSIENT))     //E_FOUCSGAIN->E_LOSE_TRANSIENT
            ){
                //try to disconnect them when connection loss audio focus
                requestDisconnect();

                logDebug(AC,"notify to loss audio focus");
                pStream->notifyFocusStateChanged(mConnectID,convert2AppFoucsSta(oldFocusSta),convert2AppFoucsSta(newFocusSta));
            }else if(((oldFocusSta==E_GAIN_FADE)&&(newFocusSta==E_FOUCSGAIN))      //E_GAIN_FADE->E_FOUCSGAIN
                 ||  ((oldFocusSta==E_FOUCSGAIN)&&(newFocusSta==E_GAIN_FADE))     //E_FOUCSGAIN->E_GAIN_FADE
            ){
                setVolume();
            }else if(((oldFocusSta==E_FOCUSUNKNOW)&&(newFocusSta==E_FOCUSLOSE))         //E_FOCUSUNKNOW->E_FOCUSLOSE
                ||  ((oldFocusSta==E_FOCUSUNKNOW)&&(newFocusSta==E_LOSE_TRANSIENT))      //E_FOCUSUNKNOW->E_GAIN_FADE
                ||  ((oldFocusSta==E_FOCUSLOSE)&&(newFocusSta==E_LOSE_TRANSIENT))      //E_FOCUSLOSE->E_GAIN_FADE
                ||  ((oldFocusSta==E_LOSE_TRANSIENT)&&(newFocusSta==E_FOCUSLOSE))      //E_LOSE_TRANSIENT->E_FOCUSLOSE
            ){
                logDebug(AC,"notify to loss audio focus");
                pStream->notifyFocusStateChanged(mConnectID,convert2AppFoucsSta(oldFocusSta),convert2AppFoucsSta(newFocusSta));
            }
        }else{
            logDebug(AC,"focus state is not changed");
        }

        if(newFocusSta==E_FOCUSLOSE){
            logDebug(AC,"Connection::handleNewFocusState()-->delete connection when newFocusSta==E_FOCUSLOSE");
            logDebug(AC,"remove connection from AudioCtrl&Stream map!");
            AudioController::GetInstance()->delConnectionFromMap(mConnectID);
            pStream->removeConnectionFromMap(mConnectID);
        }
    }else{//if pStream==NULL
        logError(AC,"stream is not exist now!");
        return;
    }

    logVerbose(AC,"Connection::handleNewFocusState()-->OUT");
}

ama_focusSta_t Connection::convert2AppFoucsSta(ama_focusSta_t foucsSta)
{
    logVerbose(AC,"Connection::convert2AppFoucsSta()-->IN foucsSta=", ToHEX(foucsSta));

    ama_focusSta_t appFocusSta = foucsSta;

    if(appFocusSta < E_FOCUSLOSE){
        appFocusSta = E_FOUCS_LOSE;
    }

    if(appFocusSta > E_GAIN_FADE){
        appFocusSta = E_FOUCS_GAIN;
    }

    logVerbose(AC,"Connection::convert2AppFoucsSta()-->OUT");
    return appFocusSta;
}

void Connection::OnAudioSettingChanged()
{
    logVerbose(AC,"Connection::OnAudioSettingChanged()-->IN mConnectID=", ToHEX(mConnectID));

    setVolume();

    logVerbose(AC,"Connection::OnAudioSettingChanged()-->OUT");
}

void Connection::setVolume(void)
{
    logVerbose(AC,"Connection::setVolume()-->IN");

    //set volume here
    if(mIsStreamMute){
        requestSetVolGain(AM_MUTE_DB);
    }else{
        int volGainGetted = AudioSetting::GetInstance()->getVolGain(mConnectID,mFocusSta);
        requestSetVolGain(volGainGetted);
    }

    logVerbose(AC,"Connection::setVolume()-->OUT");
}

void Connection::execConnectWhenSourceCreated(void)
{
    logVerbose(AC,"Connection::execConnectWhenSourceCreated()-->>IN connectID=", ToHEX(mConnectID));

    if(IS_ALREADY_HAVE_FOCUS(mFocusSta)){
        logDebug(AC,"connection need to be connect");
        //try to connect
        ConnectController::GetInstance()->tryToConnectAndSetVolWhenAMSourceCreated(mConnectID);
    }else{
        logDebug(AC,"connection havn't get audio Focus");
    }

    logVerbose(AC,"Connection::execConnectWhenSourceCreated()-->>OUT");
}

void Connection::execDisconnectWhenSourceRemoved(void)
{
    logVerbose(AC,"Connection::execDisconnectWhenSourceRemoved()-->>IN connectID=", ToHEX(mConnectID));

    if(IS_ALREADY_HAVE_FOCUS(mFocusSta)){
        logDebug(AC,"connection need to handle the situation that source removed");
        //try to disconnect
        requestDisconnect();
    }else{
        logDebug(AC,"connection need to handle the situation ,because connection don't have focus");
    }

    logVerbose(AC,"Connection::execDisconnectWhenSourceRemoved()-->>OUT");
}

void Connection::requestConnect(void)
{
    logVerbose(AC,"Connection::requestConnect()-->IN");

    ConnectController::GetInstance()->requestConnect(mConnectID);

    logVerbose(AC,"Connection::requestConnect()-->OUT");
}

void Connection::requestDisconnect(void)
{
    logVerbose(AC,"Connection::disconnect()-->IN");

    ConnectController::GetInstance()->requestDisconnect(mConnectID);

    logVerbose(AC,"Connection::disconnect()-->OUT");
}

void Connection::requestSetVolGain(int gain)
{
    logVerbose(AC,"Connection::requestSetVolGain()-->IN gain:",gain);

    ConnectController::GetInstance()->requestSetVolGain(mConnectID,gain);

    logVerbose(AC,"Connection::requestSetVolGain()-->OUT");
}

void Connection::duckAudio(int durations_ms,int duckVolume)
{
    logVerbose(AC,"Connection::duckAudio()-->IN");

    if(E_FOCUS_TYPE_MEDIA==GET_FOCUSTYPE_FROM_CONNECTID(mConnectID)){
        ConnectController::GetInstance()->requestDuckAudio(mConnectID,durations_ms,duckVolume);
    }

    logVerbose(AC,"Connection::duckAudio()-->OUT");
}
void Connection::unduckAudio(int durations_ms)
{
    logVerbose(AC,"Connection::unduckAudio()-->IN");

    if(E_FOCUS_TYPE_MEDIA==GET_FOCUSTYPE_FROM_CONNECTID(mConnectID)){
        ConnectController::GetInstance()->requestUnduckAudio(mConnectID,durations_ms);
    }

    logVerbose(AC,"Connection::unduckAudio()-->OUT");
}
