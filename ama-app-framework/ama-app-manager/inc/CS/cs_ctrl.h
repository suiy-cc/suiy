/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef _HARDKEY_CONTROLLER_H
#define _HARDKEY_CONTROLLER_H

#include <memory>
#include <vector>

#include "rpc_proxy.h"
#include "cs_task.h"
#include "popup.h"
#include "LastSourceManager.h"

using namespace std;

void HardkeyFunCB(KeyID id, KeyEventID eid, void * data);
void RpcRevFunCB(RpcID id, void * data, int len);

class CSController
{
public:
	CSController();
	void doTask(std::shared_ptr<Task> task);
	static void installPCL();

private:
	static void hardkeyHandler(KeyID id, KeyEventID eid, void * data);
	static void homeKeyHandler();
    static void onSpeedGalaChanged(unsigned char speedGala);
    void onEOLChanged(EOLInfo ei);
	void setChildrenLock(bool isEnable);
	void setRSELock(bool isEnable);
	void stopAllRSEMediaApp(void);
	void startRSEHomeApp(void);
    void setCurrentSource(const std::vector<IntentInfo> &infoVector);
    void getCurrentSource(std::vector<IntentInfo> &infoVector);
	void saveCurrentSource();
    void getLastSource(std::vector<IntentInfo> &infoVector);
	void onModuleStartup();
	//std::shared_ptr<PopupManager> popupMgr_;

private:
    LastSourceManager lastSourceManager;
};

#endif
