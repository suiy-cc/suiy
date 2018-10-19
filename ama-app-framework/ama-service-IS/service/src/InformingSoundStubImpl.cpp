/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "InformingSoundStubImpl.h"
#include "log.h"
#include "logDef.h"
#include "ISTask.h"

std::shared_ptr<InformingSoundStubImpl> sCommonAPIServer;

void commonAPIServerInit()
{
    // DomainVerboseLog chatter(CAPI, "commonAPIServerInit()");

    CommonAPI::Runtime::setProperty("LogContext", "ct1");
    CommonAPI::Runtime::setProperty("LibraryBase", "InformingSound");
    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
    sCommonAPIServer = std::make_shared<InformingSoundStubImpl>();
    while (true)
    {
        // logDebug(CAPI, "Initializing CAPI server...");
        std::string domain = "";
        std::string instance = "";
        std::string connection = "";
        bool successfullyRegistered = runtime->registerService(domain, instance, sCommonAPIServer, connection);

        if(successfullyRegistered)
            break;

        const int retryTime = 10;
        // logWarn(CAPI,"Initialize CAPI server failed, trying again in ", retryTime, " ms.");
        std::this_thread::sleep_for(std::chrono::milliseconds(retryTime));
    }
    // logDebug(CAPI,"Successfully Registered CAPI server!");
}

InformingSoundStubImpl::InformingSoundStubImpl()
{
    // logVerbose(CAPI,"InformingSoundStubImpl::",__FUNCTION__,"-->>IN");
    // logVerbose(CAPI,"InformingSoundStubImpl::",__FUNCTION__,"-->>OUT");
}

InformingSoundStubImpl::~InformingSoundStubImpl()
{
    // logVerbose(CAPI,"InformingSoundStubImpl::",__FUNCTION__,"-->>IN");
    // logVerbose(CAPI,"InformingSoundStubImpl::",__FUNCTION__,"-->>OUT");
}

void InformingSoundStubImpl::reqToPlayInformingSound(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _connectID, bool _isPlay)
{
    logVerbose(CAPI,"InformingSoundStubImpl::",__FUNCTION__,"-->>IN _connectID=", ToHEX(_connectID));

    auto pTask = std::make_shared<ISTask>();
    if(E_STREAM_AVM_ALERT==GET_STREAMTYPE_FROM_CONNECTID(_connectID)){
        pTask->SetType(REQ_AVMALERT_SOUND);
        pTask->setIsPlay(_isPlay);
    }else{
        pTask->SetType(REQ_INFORMING_SOUND);
    }
    pTask->setConnectID(_connectID);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(CAPI,"InformingSoundStubImpl::",__FUNCTION__,"-->>OUT");
}

void InformingSoundStubImpl::reqToPlaytRadarSound(const std::shared_ptr<CommonAPI::ClientId> _client, uint8_t _soundType, uint16_t _highestLevelChannel)
{
    logVerbose(CAPI,"InformingSoundStubImpl::",__FUNCTION__,"-->>IN");

    auto pTask = std::make_shared<ISTask>();
    pTask->SetType(REQ_PLAYRADAR_SOUND);
    pTask->setRadarSoundType(_soundType);
    pTask->setRadarChannel(_highestLevelChannel);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(CAPI,"InformingSoundStubImpl::",__FUNCTION__,"-->>OUT");
}

void InformingSoundStubImpl::reqToPlayFeedbackSound(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _volumeType, uint32_t _volume)
{
    logVerbose(CAPI,"InformingSoundStubImpl::",__FUNCTION__,"-->>IN");

    auto pTask = std::make_shared<ISTask>();
    pTask->SetType(REQ_FEEDBACKTONE_SOUND);
    pTask->setVolumeType(_volumeType);
    pTask->setFeedbackVol(_volume);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(CAPI,"InformingSoundStubImpl::",__FUNCTION__,"-->>OUT");
}
