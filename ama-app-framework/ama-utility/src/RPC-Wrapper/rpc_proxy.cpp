/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include <iostream>
#include <cstring>

#include "rpc_proxy.h"
#include "rpc_proxy_impl.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

RpcProxy::RpcProxy()
{
    rpcProxyImpl_ = new RpcProxyImpl();
}

RpcProxy::~RpcProxy()
{
	delete rpcProxyImpl_;
}

Time RpcProxy::getSystemTime()
{
	return rpcProxyImpl_->getSystemTime();
}

SysInfo RpcProxy::getSystemInfo()
{
    return rpcProxyImpl_->getSystemInfo();
}

EOLInfo RpcProxy::getEOLInfo()
{
    return rpcProxyImpl_->getEOLInfo();
}

uint32_t RpcProxy::getVehicleSpeed()
{
    return rpcProxyImpl_->getVehicleSpeed();
}

bool RpcProxy::registerKeyEvent(KeyID id, KeyEventID eid, KeyFunCB fcb)
{
	return rpcProxyImpl_->registerKeyEvent(id, eid, fcb);
}

bool RpcProxy::registerKeyEventOnDown(KeyFunCB fcb)
{
	return rpcProxyImpl_->registerKeyEventOnDown(fcb);
}

void RpcProxy::setSystemTime(const Time & t)
{
	rpcProxyImpl_->setSystemTime(t);
}

void RpcProxy::setGpsInfo(const GpsInfo & gi)
{
    rpcProxyImpl_->setGpsInfo(gi);
}

void RpcProxy::setAmpMode(uint8_t mode)
{
    rpcProxyImpl_->setAmpMode(mode);
}

void RpcProxy::setLinKeyWorkSta(bool isEnable)
{
    rpcProxyImpl_->setLinKeyWorkSta(isEnable);
}

bool RpcProxy::registerRpcEvent(RpcID id, RpcFunCB rf)
{
	rpcProxyImpl_->registerRpcEvent(id, rf);
}
