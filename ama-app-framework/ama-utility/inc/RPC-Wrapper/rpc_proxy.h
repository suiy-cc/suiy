/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef _RPC_PROXY_H_
#define _RPC_PROXY_H_

#include <stdint.h>
extern "C"
{
#include "librpc.h"
}
#include "rpc_proxy_def.h"

class RpcProxyImpl;

class RpcProxy
{
public:
	RpcProxy();
	~RpcProxy();

	RpcProxy(const RpcProxy & rhs) = delete;
	RpcProxy & operator=(const RpcProxy & rhs) = delete;

	Time getSystemTime();
	void setSystemTime(const Time & t);
	SysInfo getSystemInfo();
	uint32_t getVehicleSpeed();
	EOLInfo getEOLInfo();

	void setGpsInfo(const GpsInfo & gi);
	void setAmpMode(uint8_t mode);
	void setLinKeyWorkSta(bool isEnable);
	// user callback function type
	// Notice : user's memory is released by user
	// void registerCB(UserCBFunc func, int flag, void * user = nullptr);
	bool registerKeyEvent(KeyID id, KeyEventID eid, KeyFunCB fcb);

	bool registerKeyEventOnDown(KeyFunCB fcd);

	bool registerRpcEvent(RpcID id, RpcFunCB rf);

private:
	RpcProxyImpl * rpcProxyImpl_;
};

#endif
