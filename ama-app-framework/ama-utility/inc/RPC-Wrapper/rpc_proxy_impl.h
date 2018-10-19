/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef _RPC_RPOXY_IMPL_H_
#define _RPC_RPOXY_IMPL_H_

#include "rpc_proxy.h"

extern "C"
{
#include "librpc.h"
}

enum CB_OPCODE
{
	CB_OPCODE_KEY = 0x050A,
	CB_OPCODE_WHEEL_KEY_VOICE = 0x0511,
	CB_OPCODE_WHEEL_KEY_NUM   = 0x0512,
	CB_OPCODE_SPEED_CHANGE    = 0x0515,
	CB_OPCODE_PWR_LIMIT_SIG   = 0x0516,
	CB_OPCODE_SMALL_LIGHT_SIG = 0x091E,
    CB_OPCODE_EOL_CHANGED     = 0x0C22,
};

enum KEY_VAL
{
	// center control panel <KEY>
	KEY_VAL_PANEL_HOME    = 0x01,

	// steering wheel <KEY>
	// KEY_VAL_WHEEL_REJECT  =
};

enum KEY_STAT
{
	KEY_STAT_ON      = 0x00,    // key down
	KEY_STAT_OFF     = 0x01,    // key up
	KEY_STAT_3s_OUT  = 0x02,
	KEY_STAT_7s_OUT  = 0x03,
	KET_STAT_OUT_OFF = 0x04,
};

/**
 * \brief  callback return data type
 */
typedef union _CBData
{
	struct {
		unsigned char KeyNumber;
		unsigned char KeyStatus;
	} __attribute__((packed, aligned(1))) KeyNum;

	struct {
		unsigned short Year;
		unsigned char Month;
		unsigned char Day;
		unsigned char Hour;
		unsigned char Minute;
		unsigned char Second;
		unsigned char Result;
		unsigned char Reserved[17];
	} __attribute__((packed, aligned(1))) Time;

} __attribute__((packed, aligned(1))) CBData;

/**
 * \brief  RPC Proxy implementation
 */
class RpcProxyImpl
{
public:
	RpcProxyImpl();
	~RpcProxyImpl();

    librpc_rx_t GetRPCValue(uint16_t key);

	Time getSystemTime();
	SysInfo getSystemInfo();
	uint32_t getVehicleSpeed();
    EOLInfo getEOLInfo();

	void setSystemTime(const Time & t);
	void setGpsInfo(const GpsInfo & gi);
	void setAmpMode(uint8_t mode);
	void setLinKeyWorkSta(bool isEnable);
	// user callback function type
	// Notice : user's memory is released by user
    // void registerCB(UserCBFunc func, int flag, void * user = nullptr);
	bool registerKeyEvent(KeyID id, KeyEventID eid, KeyFunCB fcb);
	bool registerKeyEventOnDown(KeyFunCB fcb);
	bool registerRpcEvent(RpcID id, RpcFunCB rf);

private:
	static void parseCallBack(librpc_rx_t * rxbuf, void * user);
	static int convertKeyNum(unsigned char kn);
	static int convertkeyStatus(unsigned char ks);
	static bool filterKeyEvent(RpcProxyImpl * rp, unsigned char kn, unsigned char ks);

	librpc_t *  rpcHandle_;
	CBData      callbackData_;	// callback return data structure
	// key data structure
    unsigned int keyIds_    = 0x0000;
    unsigned int keyEvents_ = 0x0000;
    unsigned int rpcIds_    = 0x0000;
    KeyFunCB keyCBFun_   = nullptr;
    KeyFunCB keyDownFun_ = nullptr;
    RpcFunCB rpcRevFun_  = nullptr;
};

#endif
