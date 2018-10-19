/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef _CS_TASK_H
#define _CS_TASK_H

#include "TASK/TaskDispatcher.h"
#include "Task.h"
#include "AMGRLogContext.h"

#include "rpc_proxy_def.h"
#include "popup.h"
#include "IntentInfo.h"

#define TASK_INFO_MAX 64

enum CSTaskType
{
    E_CS_TASK_TYPE_KEY,
    E_CS_TASK_TYPE_SPEED_CHANGE_GALA,
    E_CS_TASK_TYPE_EOL_CHANGED,
    E_CS_TASK_INSTALL_PCL,
    E_CS_TASK_TYPE_POP_ADD,
    E_CS_TASK_TYPE_POP_SHOW,
    E_CS_TASK_TYPE_POP_DELETE,
    E_CS_TASK_TYPE_POP_RESPOND,
    E_CS_TASK_TYPE_POP_RESPOND_EX,
    E_CS_TASK_TYPE_IME_SHOW,
    E_CS_TASK_TYPE_IME_CLOSE,
    E_CS_TASK_TYPE_IME_HIDE,
    E_CS_TASK_TYPE_ENABLE_CHILDRENLOCK,
    E_CS_TASK_TYPE_DISABLE_CHILDRENLOCK,
    E_CS_TASK_TYPE_ENABLE_RSELOCK,
    E_CS_TASK_TYPE_DISABLE_RSELOCK,

    // last source tasks begin here
    E_CS_TASK_TYPE_SET_CURRENT_SOURCE, // set current source
    E_CS_TASK_TYPE_GET_CURRENT_SOURCE, // get current source
    E_CS_TASK_TYPE_SAVE_CURRENT_SOURCE, // save current source
    E_CS_TASK_TYPE_GET_LAST_SOURCE, // get last source of current power cycle
    // last source tasks end here

    E_CS_TASK_MODULE_STARTUP,
};

class CSTask : public Task
{
public:
    CSTask();
    void setKeyEvent(KeyID keyid, KeyEventID keyEvent, void * data = nullptr);
    KeyID getKeyId() const;
    KeyEventID getKeyEventId() const;
    void * getKeyData() const {
        return keyUsr_;
    }
    void setPopInfo(const PopInfo & pi);
    PopInfo getPopInfo() const;
    void setPopupId(uint32_t popupId) {
        popupId_ = popupId;
    }
    uint32_t getPopupId() {
        return popupId_;
    }
    void setAmpid(uint32_t ampid) {
        ampid_ = ampid;
    }
    uint32_t getAmpid() {
        return ampid_;
    }
    void setPopupRespondType(uint32_t popupRespondType) {
        popupRespondType_ = popupRespondType;
    }
    uint32_t getPopupRespondType() {
        return popupRespondType_;
    }
    void setPopupRespondMessage(std::string popupRespondMessage) {
        popupRespondMessage_ = popupRespondMessage;
    }
    std::string getPopupRespondMessage() {
        return popupRespondMessage_;
    }
    RpcID getRpcId() const {
        return rpcId_;
    }
    int getRpcData(unsigned char * data) const {
        memcpy(data, info_, infoLen_);
        return infoLen_;
    }
    void setRpcEvent(RpcID id, void * data, int len) {
        rpcId_   = id;
        memcpy(info_, data, len);
        infoLen_ = len;
    }
    void setLastSourceSet(const std::vector<IntentInfo>& infos) {
        lastSourceSet = infos;
    }
    void setLastSourceSet(std::vector<IntentInfo>&& infos) {
        lastSourceSet = std::move(infos);
    }
    const std::vector<IntentInfo>& getLastSourceSet() const {
        return lastSourceSet;
    }
    virtual string GetTaskTypeString()
    {
        switch(GetType())
        {
            case E_CS_TASK_TYPE_KEY:return "E_CS_TASK_TYPE_KEY";
            case E_CS_TASK_TYPE_SPEED_CHANGE_GALA:return "E_CS_TASK_TYPE_SPEED_CHANGE_GALA";
            case E_CS_TASK_TYPE_EOL_CHANGED:return "E_CS_TASK_TYPE_EOL_CHANGED";
            case E_CS_TASK_INSTALL_PCL:return "E_CS_TASK_INSTALL_PCL";
            case E_CS_TASK_TYPE_POP_ADD:return "E_CS_TASK_TYPE_POP_ADD";
            case E_CS_TASK_TYPE_POP_SHOW:return "E_CS_TASK_TYPE_POP_SHOW";
            case E_CS_TASK_TYPE_POP_DELETE:return "E_CS_TASK_TYPE_POP_DELETE";
            case E_CS_TASK_TYPE_POP_RESPOND:return "E_CS_TASK_TYPE_POP_RESPOND";
            case E_CS_TASK_TYPE_POP_RESPOND_EX:return "E_CS_TASK_TYPE_POP_RESPOND_EX";
            case E_CS_TASK_TYPE_IME_SHOW:return "E_CS_TASK_TYPE_IME_SHOW";
            case E_CS_TASK_TYPE_IME_CLOSE:return "E_CS_TASK_TYPE_IME_CLOSE";
            case E_CS_TASK_TYPE_IME_HIDE:return "E_CS_TASK_TYPE_IME_HIDE";
            case E_CS_TASK_TYPE_ENABLE_CHILDRENLOCK:return "E_CS_TASK_TYPE_ENABLE_CHILDRENLOCK";
            case E_CS_TASK_TYPE_DISABLE_CHILDRENLOCK:return "E_CS_TASK_TYPE_DISABLE_CHILDRENLOCK";
            case E_CS_TASK_TYPE_ENABLE_RSELOCK:return "E_CS_TASK_TYPE_ENABLE_RSELOCK";
            case E_CS_TASK_TYPE_DISABLE_RSELOCK:return "E_CS_TASK_TYPE_DISABLE_RSELOCK";
            case E_CS_TASK_TYPE_SET_CURRENT_SOURCE:return "E_CS_TASK_TYPE_SET_CURRENT_SOURCE";
            case E_CS_TASK_TYPE_GET_CURRENT_SOURCE:return "E_CS_TASK_TYPE_GET_CURRENT_SOURCE";
            case E_CS_TASK_TYPE_SAVE_CURRENT_SOURCE:return "E_CS_TASK_TYPE_SAVE_CURRENT_SOURCE";
            case E_CS_TASK_TYPE_GET_LAST_SOURCE:return "E_CS_TASK_TYPE_GET_LAST_SOURCE";
            case E_CS_TASK_MODULE_STARTUP:return "E_CS_TASK_MODULE_STARTUP";
            default:
                return "invalid CS task type!";
        }
    }
private:
    KeyID keyid_ ;
    KeyEventID keyEvent_;
    RpcID rpcId_;
    void * keyUsr_;
    uint32_t ampid_;
    PopInfo popInfo_;
    uint32_t popupId_;
    uint32_t popupRespondType_;
    std::string popupRespondMessage_;
    unsigned char info_[TASK_INFO_MAX];
    int infoLen_;
    std::vector<IntentInfo> lastSourceSet;
};

#endif
