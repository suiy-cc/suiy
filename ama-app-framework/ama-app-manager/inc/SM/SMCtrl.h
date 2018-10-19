/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef SM_CONTROLLER_H
#define SM_CONTROLLER_H

#include <memory>

#include "ActiveApp.h"
#include "SMTask.h"
#include "AMGRLogContext.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "Capi.h"
#include "KeyValuePairObserver.h"

using std::shared_ptr;
class SMValue{
public:
    SMValue():value(),requester(INVALID_AMPID),replyerItemID(""){};
    SMValue(v0_1::org::neusoft::AppMgr::value_ampid valueAmpid)
    : value(valueAmpid.getValue())
	, replyerItemID(valueAmpid.getItemid_reply())
    , requester(valueAmpid.getAmpid_req()){};
    SMValue(std::string _value,std::string _replyerItemID, unsigned int _requester, unsigned int _replyer)
    : value(_value)
	, replyerItemID(_replyerItemID)
    , requester(_requester){};

public:
    std::string value;
    unsigned int requester;
	std::string replyerItemID;
};

typedef KeyValuePairObserver<std::string, SMValue> SMValueObserver;
extern SMValueObserver sSMKeyValuePairObserver;
::v0_1::org::neusoft::AppMgr::value_ampid SMV2VA(const SMValue& value);
::v0_1::org::neusoft::AppMgr::key_value SMVO2KV(const SMValueObserver& kvMap);

// useful key define
#define SYS_LANG "syssetting_SYS_LANG"

class Context;

class SMController
{
public:
    SMController();
    ~SMController();

    void doTask(shared_ptr<Task> task);

protected:
	void OnQuerySMSnapshot(shared_ptr<SMTask> task);
    void OnBroadcastSetRequest(shared_ptr<SMTask> task);
    void OnBroadcastSetReply(shared_ptr<SMTask> task);
	void OnRequestBroadcast(shared_ptr<SMTask> task);
private:

};

static std::list<std::string> sAppItemPrefixList{
		"com.hs7.tbox",
		"com.hs7.bt",
		"com.hs7.syssetting",
		"com.hs7.autopark",
		"com.hs7.aircondition",
		"com.hs7.vr",
		"com.hs7.help",
		"com.hs7.factory",
		"com.hs7.engineering",
		"com.hs7.media",
		"com.hs7.titlebar",
		"com.hs7.flappybird",
		"com.hs7.welink",
		"com.hs7.ime",
		"com.hs7.sync",
		"com.hs7.popup",
		"com.hs7.home",
		"com.hs7.radio",
		"com.hs7.animation-player",
		"com.hs7.vehsetting",
		"com.hs7.cloud",
		"com.hs7.camera",
		"com.hs7.browser",
		"com.hs7.navi",
		"com.hs7.radar",
		"com.hs7.diag",
};

#endif // SM_CONTROLLER_H
