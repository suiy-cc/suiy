/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "rpc_proxy_impl.h"

#include <thread>
#include <iostream>
#include <cstring>
#include <map>
#include <sstream>
#include <iomanip>
#include <string>

#include "log.h"

LOG_DECLARE_CONTEXT(RPCW);
// internal key map external key list
static std::map<unsigned char, int> keyNumMap = {
	{0x01, E_KEY_HOME},
	//{, },
};
static std::map<unsigned char, int> keyStatusMap = {
	{0x00, KEY_EVENT_DOWN},
	{0x01, KEY_EVENT_UP},
	{0x02, KEY_EVENT_LONG_3s},
	{0x03, KEY_EVENT_LONG_7s},
};

static std::map<unsigned char, int> wheelKeyNumMap = {
	{0x00, KEY_WHEEL_PHONE_ANSWER},
	{0x01, KEY_WHEEL_PHONE_REJECT},
	{0x02, KEY_WHEEL_PHONE_MUTE},
	{0x03, KEY_WHEEL_VOICE_RECOGNITION},
	{0x04, KEY_WHEEL_HUD_MODE},
	//{, },
};
static std::map<unsigned char, int> wheelKeyStatusMap = {
	{0x00, KEY_EVENT_DOWN},
	{0x01, KEY_EVENT_UP},
	{0x02, KEY_EVENT_1P5S_OUT},
	{0x03, KEY_EVENT_OUT_OFF},
};

RpcProxyImpl::RpcProxyImpl()
{
	librpc_ref(&rpcHandle_);
	librpc_when_data(rpcHandle_, parseCallBack, (void *)this);
}

RpcProxyImpl::~RpcProxyImpl()
{
	librpc_unref(&rpcHandle_);
}

librpc_rx_t RpcProxyImpl::GetRPCValue(uint16_t key)
{
	DomainVerboseLog chatter(RPCW, formatText(__STR_FUNCTION__+"(%#x)", key));

	librpc_tx_t txInfo;
	librpc_rx_t rxInfo;

	// fill argument
	txInfo.control = 0x00;
	txInfo.opcode  = key;
	txInfo.datalen = LIBRPC_TXDATA_MAX;
	memset(txInfo.data, 0x00, LIBRPC_TXDATA_MAX);
	int rtv = librpc_info(rpcHandle_, &txInfo, &rxInfo);
    if(rtv < 0)
    {
        logError(RPCW, LOG_HEAD, "librpc_info's return value is ", rtv, " !");
    }

	return rxInfo;
}

Time RpcProxyImpl::getSystemTime()
{
	FUNCTION_SCOPE_LOG_C(RPCW);

	librpc_rx_t rxInfo = GetRPCValue(0x0508);

	// process return data
    Time sysTime;
	memcpy(&sysTime, rxInfo.data, sizeof(sysTime));

	return sysTime;
}

SysInfo RpcProxyImpl::getSystemInfo()
{
    FUNCTION_SCOPE_LOG_C(RPCW);
	librpc_rx_t rxInfo = GetRPCValue(0x0509);

	// process return data
    SysInfo si;
	memcpy(&si, rxInfo.data, sizeof(SysInfo));

	return si;
}

uint32_t RpcProxyImpl::getVehicleSpeed()
{
    FUNCTION_SCOPE_LOG_C(RPCW);
	librpc_rx_t rxInfo = GetRPCValue(0x050C);

	// process return data
    uint32_t vehicleSpeed = 0;
	memcpy(&vehicleSpeed, rxInfo.data, sizeof(vehicleSpeed));

	return vehicleSpeed;
}

#ifdef CODE_FOR_EV
static EOLInfo RX2EOL_forEV(const librpc_rx_t& rxInfo)
{
	FUNCTION_SCOPE_LOG_C(RPCW);
	EOLInfo ei = {0};
	memcpy(&ei, rxInfo.data, sizeof(EOLInfo));

	logInfoF(RPCW, "SMCtrl.cpp::sm_eolGet_forEV \
		FCW:%d,AEB:%d,BSD:%d,LDW:%d,LKA:%d,DSM:%d,LDWS:%d,TRM:%d,HBA:%d,AP:%d,TSR:%d,LANG:%d,ALC:%d,ALCD:%d,SCM:%d,DSCU:%d,HUD:%d,DVR:%d,EFI:%d,MSMTD:%d,BMFS:%d,ALL:%d,CURISE:%d,TDMC:%d",\
		(uint8_t)ei.FCW,(uint8_t)ei.AEB,(uint8_t)ei.BSD,(uint8_t)ei.LDW,(uint8_t)ei.LKA,(uint8_t)ei.DSM,(uint8_t)ei.LDWS,\
		(uint8_t)ei.TRM,(uint8_t)ei.HBA,(uint8_t)ei.AP,(uint8_t)ei.TSR,(uint8_t)ei.LANG,(uint8_t)ei.ALC,(uint8_t)ei.ALCD,\
		(uint8_t)ei.SCM,(uint8_t)ei.DSCU,(uint8_t)ei.HUD,(uint8_t)ei.DVR,(uint8_t)ei.EFI,(uint8_t)ei.MSMTD,(uint8_t)ei.BMFS,(uint8_t)ei.ALL,(uint8_t)ei.CURISE,(uint8_t)ei.TDMC);

	#if 0
	if(    ((uint8_t)ei.FCW == 0)
		&& ((uint8_t)ei.AEB == 0)
		&& ((uint8_t)ei.BSD == 0)
		&& ((uint8_t)ei.LDW == 0)
		&& ((uint8_t)ei.LKA == 0)
		&& ((uint8_t)ei.DSM == 0)
		&& ((uint8_t)ei.LDWS == 0)
		&& ((uint8_t)ei.TRM == 0)
		&& ((uint8_t)ei.HBA == 0)
		&& ((uint8_t)ei.AP == 0)
		&& ((uint8_t)ei.TSR == 0)
		&& ((uint8_t)ei.LANG == 0)
		&& ((uint8_t)ei.ALC == 0)
		&& ((uint8_t)ei.ALCD == 0)
		&& ((uint8_t)ei.SCM == 0)
		&& ((uint8_t)ei.DSCU == 0)
		&& ((uint8_t)ei.HUD == 0)
		&& ((uint8_t)ei.DVR == 0)
		&& ((uint8_t)ei.EFI == 0)
		&& ((uint8_t)ei.MSMTD == 0)
		&& ((uint8_t)ei.BMFS == 0)
		&& ((uint8_t)ei.ALL == 0)    )
	#endif
	//High configuration  all1 & default
	//Low configuration  all0
	#if 0
	if(    \
		((((uint8_t)ei.FCW) & ((uint8_t)ei.AEB) & ((uint8_t)ei.BSD)  & ((uint8_t)ei.LDW) & ((uint8_t)ei.LKA)\
		& ((uint8_t)ei.DSM) & ((uint8_t)ei.LDWS)& ((uint8_t)ei.TRM)  & ((uint8_t)ei.HBA) & ((uint8_t)ei.AP)\
		& ((uint8_t)ei.TSR) & ((uint8_t)ei.ALC) & ((uint8_t)ei.ALCD) & ((uint8_t)ei.SCM) & ((uint8_t)ei.DSCU)\
		& ((uint8_t)ei.DVR) & ((uint8_t)ei.EFI) & ((uint8_t)ei.MSMTD)& ((uint8_t)ei.ALL) & ((uint8_t)ei.CURISE)) == 0)\
		&& \
		((((uint8_t)ei.FCW) | ((uint8_t)ei.AEB) | ((uint8_t)ei.BSD)  | ((uint8_t)ei.LDW) | ((uint8_t)ei.LKA)\
		| ((uint8_t)ei.DSM) | ((uint8_t)ei.LDWS)| ((uint8_t)ei.TRM)  | ((uint8_t)ei.HBA) | ((uint8_t)ei.AP)\
		| ((uint8_t)ei.TSR) | ((uint8_t)ei.ALC) | ((uint8_t)ei.ALCD) | ((uint8_t)ei.SCM) | ((uint8_t)ei.DSCU)\
		| ((uint8_t)ei.DVR) | ((uint8_t)ei.EFI) | ((uint8_t)ei.MSMTD)| ((uint8_t)ei.ALL) | ((uint8_t)ei.CURISE)) != 0)\
	  )
	{
   		ei.FCW =ei.AEB =ei.BSD =ei.LDW =ei.LKA =ei.DSM =ei.LDWS =ei.TRM =ei.HBA =ei.AP =ei.TSR =ei.ALC =ei.ALCD =ei.SCM =ei.DSCU =ei.HUD =ei.DVR =ei.EFI =ei.MSMTD =ei.ALL =ei.CURISE = 1;
		//ei.LANG=0;
		ei.BMFS = 0;
	}

	logVerboseF(RPCW, "SMCtrl.cpp::sm_eolGet_forEV \
		FCW:%d,AEB:%d,BSD:%d,LDW:%d,LKA:%d,DSM:%d,LDWS:%d,TRM:%d,HBA:%d,AP:%d,TSR:%d,LANG:%d,ALC:%d,ALCD:%d,SCM:%d,DSCU:%d,HUD:%d,DVR:%d,EFI:%d,MSMTD:%d,BMFS:%d,ALL:%d,CURISE:%d,TDMC:%d-->>OUT",\
		(uint8_t)ei.FCW,(uint8_t)ei.AEB,(uint8_t)ei.BSD,(uint8_t)ei.LDW,(uint8_t)ei.LKA,(uint8_t)ei.DSM,(uint8_t)ei.LDWS,\
		(uint8_t)ei.TRM,(uint8_t)ei.HBA,(uint8_t)ei.AP,(uint8_t)ei.TSR,(uint8_t)ei.LANG,(uint8_t)ei.ALC,(uint8_t)ei.ALCD,\
		(uint8_t)ei.SCM,(uint8_t)ei.DSCU,(uint8_t)ei.HUD,(uint8_t)ei.DVR,(uint8_t)ei.EFI,(uint8_t)ei.MSMTD,(uint8_t)ei.BMFS,(uint8_t)ei.ALL,(uint8_t)ei.CURISE,(uint8_t)ei.TDMC);
    #endif
	return ei;
}
#endif

#ifndef CODE_FOR_EV
static EOLInfo RX2EOL_forHS7(const librpc_rx_t& rxInfo)
{
    FUNCTION_SCOPE_LOG_C(RPCW);
#ifdef CODE_FOR_HS5
	EOLInfo_HS5 ei_hs5 = {0};
	memcpy(&ei_hs5, rxInfo.data, sizeof(EOLInfo_HS5));
	logInfoF(RPCW, "SMCtrl.cpp::sm_eolGet_forHS5\
		FCW:%d,AEB:%d,BSD:%d,LDW:%d,LKA:%d,DSM:%d,LDWS:%d,TRM:%d,HBA:%d,AP:%d,TSR:%d,LANG:%d,ALC:%d,ALCD:%d,SCM:%d,DSCU:%d,HUD:%d,DVR:%d,ALL:%d,RI_AVS:%d",\
		(uint8_t)ei_hs5.FCW,(uint8_t)ei_hs5.AEB,(uint8_t)ei_hs5.BSD,(uint8_t)ei_hs5.LDW,(uint8_t)ei_hs5.LKA,(uint8_t)ei_hs5.DSM,(uint8_t)ei_hs5.LDWS,\
		(uint8_t)ei_hs5.TRM,(uint8_t)ei_hs5.HBA,(uint8_t)ei_hs5.AP,(uint8_t)ei_hs5.TSR,(uint8_t)ei_hs5.LANG,(uint8_t)ei_hs5.ALC,(uint8_t)ei_hs5.ALCD,\
		(uint8_t)ei_hs5.SCM,(uint8_t)ei_hs5.DSCU,(uint8_t)ei_hs5.HUD,(uint8_t)ei_hs5.DVR,(uint8_t)ei_hs5.ALL,(uint8_t)ei_hs5.RI_AVS);
	EOLInfo ei = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	//ei.FCW=ei_hs5.FCW;
	//ei.AEB=ei_hs5.AEB;
	//ei.LDW=ei_hs5.LDW;
	//ei.LKA=ei_hs5.LKA;
	//ei.DSM=ei_hs5.DSM;
	//ei.HBA=ei_hs5.HBA;
	//ei.AP=ei_hs5.AP;
	ei.LANG=ei_hs5.LANG;
	//ei.DVR=ei_hs5.DVR;
	ei.RI_AVS=ei_hs5.RI_AVS;
#else
    EOLInfo ei = {0};
    memcpy(&ei, rxInfo.data, sizeof(EOLInfo));
#endif

    logInfoF(RPCW, "SMCtrl.cpp::sm_eolGet_forHS7\
		fcw:%d aeb:%d mpi:%d ldw:%d lka:%d dsm:%d ri_avs:%d rpe:%d hba:%d ap:%d tbox:%d lang:%d wifi:%d brow:%d \
		DMSCfg:%d DVR:%d AVS:%d CURISE:%d TDMC:%d",\
		(uint8_t)ei.FCW,(uint8_t)ei.AEB,(uint8_t)ei.MPI,(uint8_t)ei.LDW,(uint8_t)ei.LKA,(uint8_t)ei.DSM,\
		(uint8_t)ei.RI_AVS,(uint8_t)ei.RPE,(uint8_t)ei.HBA,(uint8_t)ei.AP,(uint8_t)ei.TBOX,(uint8_t)ei.LANG,\
		(uint8_t)ei.WIFI,(uint8_t)ei.BROW,(uint8_t)ei.DMSCfg,(uint8_t)ei.DVR,(uint8_t)ei.AVS,(uint8_t)ei.CURISE,(uint8_t)ei.TDMC);
#if 0
    if(    ((uint8_t)ei.FCW == 0)
		&& ((uint8_t)ei.AEB == 0)
		&& ((uint8_t)ei.MPI == 0)
		&& ((uint8_t)ei.LDW == 0)
		&& ((uint8_t)ei.LKA == 0)
		&& ((uint8_t)ei.DSM == 0)
		&& ((uint8_t)ei.RI_AVS == 0)
		&& ((uint8_t)ei.RPE == 0)
		&& ((uint8_t)ei.HBA == 0)
		&& ((uint8_t)ei.AP == 0)
		&& ((uint8_t)ei.TBOX == 0)
		&& ((uint8_t)ei.WIFI == 0)
		&& ((uint8_t)ei.BROW == 0)    )
#endif
#if 0
    {
        ei.FCW=ei.AEB=ei.MPI=ei.LDW=ei.LKA=ei.DSM=ei.RPE=ei.HBA=ei.AP=ei.TBOX=ei.WIFI=ei.BROW = 1;
        ei.LANG=0;
        //ei.RI_AVS=ei.RI_AVS; // AVM/RVC setting is depend on EOL, instead of constant value.
    }
    logVerboseF(RPCW, "SMCtrl.cpp::sm_eolGet_forHS7\
		fcw:%d aeb:%d mpi:%d ldw:%d lka:%d dsm:%d ri_avs:%d rpe:%d hba:%d ap:%d tbox:%d lang:%d wifi:%d brow:%d-->>OUT",\
		(uint8_t)ei.FCW,(uint8_t)ei.AEB,(uint8_t)ei.MPI,(uint8_t)ei.LDW,(uint8_t)ei.LKA,(uint8_t)ei.DSM,(uint8_t)ei.RI_AVS,(uint8_t)ei.RPE,(uint8_t)ei.HBA,(uint8_t)ei.AP,(uint8_t)ei.TBOX,(uint8_t)ei.LANG,(uint8_t)ei.WIFI,(uint8_t)ei.BROW);
#endif
    return ei;
}
#endif

static EOLInfo RX2EOL(const librpc_rx_t &rxInfo)
{
    FUNCTION_SCOPE_LOG_C(RPCW);

#ifdef CODE_FOR_EV
    return RX2EOL_forEV(rxInfo);
#else
    return RX2EOL_forHS7(rxInfo);
#endif
}

EOLInfo RpcProxyImpl::getEOLInfo()
{
    FUNCTION_SCOPE_LOG_C(RPCW);

#if defined(CODE_FOR_EV) || defined(CODE_FOR_HS5)
#define E_OPCODE_EOL 0x0C20
#else
#define E_OPCODE_EOL 0x0C13
#endif // CODE_FOR_EV  CODE_FOR_HS5

	librpc_rx_t rxInfo = GetRPCValue(E_OPCODE_EOL);

	return RX2EOL(rxInfo);
}

void RpcProxyImpl::parseCallBack(librpc_rx_t * rxbuf, void * user)
{
	logVerbose(RPCW,"RpcProxyImpl::parseCallBack IN");
	RpcProxyImpl * rp = static_cast<RpcProxyImpl *>(user);
	CBData * cb = &rp->callbackData_;
	memcpy(cb, rxbuf->data, rxbuf->datalen);
    // print rpc message
	std::ostringstream os;
	os << "<Control :  " << std::setiosflags(std::ios::uppercase) << std::setfill ('0') << std::hex << std::setw(2)
				<< (int)rxbuf->control << ">  "
		<< "<OpCode  :  "<< std::setiosflags(std::ios::uppercase) << std::setfill ('0') << std::hex << std::setw(4)
		  		<< (int)rxbuf->opcode  << ">  "
		<< "<DataLen :  "<< std::setiosflags(std::ios::uppercase) << std::setfill ('0') << std::hex << std::setw(4)
				<< (int)rxbuf->datalen << ">  ";
	logDebug(RPCW, os.str());
	os.str("");
	logDebug(RPCW, "RPC Message Data : ");
	for (int i = 0; i < rxbuf->datalen; ++i) {
		os << std::setiosflags(std::ios::uppercase) << std::setfill ('0') << std::hex << std::setw(2)
			<< (int)rxbuf->data[i] << " ";
	}
	logDebug(RPCW, "Data : ", os.str());
	switch (rxbuf->opcode) {
		case CB_OPCODE_KEY:	// Key Number
		{
			unsigned char kn = cb->KeyNum.KeyNumber;
			unsigned char ks = cb->KeyNum.KeyStatus;
			os.str("");
			os << "Key : "<< std::setiosflags(std::ios::uppercase) << std::setfill ('0') << std::hex << std::setw(2)
				<< (int)kn << " "
			   << "Value : "<< std::setiosflags(std::ios::uppercase) << std::setfill ('0') << std::hex << std::setw(2)
			    << (int)ks;
			logDebug(RPCW, os.str());
			// key down event handle
			if (ks == KEY_STAT_ON) {
				if (rp->keyDownFun_ != nullptr) {
					rp->keyDownFun_((KeyID)convertKeyNum(kn), (KeyEventID)convertkeyStatus(ks), nullptr);
				}
			}
			// normal key event handle
			if (filterKeyEvent(rp, kn, ks) && rp->keyCBFun_ != nullptr) {
				rp->keyCBFun_((KeyID)convertKeyNum(kn), (KeyEventID)convertkeyStatus(ks), nullptr);
			}
			break;
		}
		case CB_OPCODE_WHEEL_KEY_NUM:
		{
			unsigned char kn = cb->KeyNum.KeyNumber;
			unsigned char ks = cb->KeyNum.KeyStatus;
			unsigned int ekn = wheelKeyNumMap[kn];
			unsigned char eks = wheelKeyStatusMap[ks];
			logDebug(RPCW, "KN : ", ekn & rp->keyIds_);
			logDebug(RPCW, "KS : ", eks & rp->keyEvents_);
			if ((ekn & rp->keyIds_) && (eks & rp->keyEvents_) && rp->keyCBFun_ != nullptr) {
				rp->keyCBFun_((KeyID)ekn, (KeyEventID)eks, nullptr);
				logDebug(RPCW, "register wheel key : ", ekn, " status : ", eks);
			}
			logDebug(RPCW, "wheel key : ", ekn, " status : ", eks);
			break;
		}
		case CB_OPCODE_WHEEL_KEY_VOICE:
		{
			int voice = *((unsigned char *)(rxbuf->data));	// rang : (0 - 24) default : (12)
			if ((KEY_WHEEL_VOL_UP_DOWN & rp->keyIds_) && (rp->keyCBFun_ != nullptr)) {
				rp->keyCBFun_((KeyID)KEY_WHEEL_VOL_UP_DOWN, KEY_EVENT_DOWN, &voice);
				logDebug(RPCW, "register wheel key voice : ", (int)voice);
			}
			logDebug(RPCW, "wheel key voice : ", (int)voice);
			break;
		}
		case CB_OPCODE_SPEED_CHANGE:
		{
			logDebug(RPCW, "Enter speed change");
            unsigned char speed = *((unsigned char *)(rxbuf->data));
			if ((RPC_SPEED_CHANGE & rp->rpcIds_) && (rp->rpcRevFun_ != nullptr)) {
				rp->rpcRevFun_(RPC_SPEED_CHANGE, &speed, sizeof(speed));
				logDebug(RPCW, "register speed change : ", speed);
			}
			break;
		}
		case CB_OPCODE_PWR_LIMIT_SIG:
		{
			logDebug(RPCW, "Enter pwr limit sig");
			int sig = *((unsigned char *)(rxbuf->data));
			if ((RPC_PWR_LIMIT_SIG & rp->rpcIds_) && (rp->rpcRevFun_ != nullptr)) {
				rp->rpcRevFun_(RPC_PWR_LIMIT_SIG, &sig, sizeof(sig));
				logDebug(RPCW, "register pwr limit sig : ", (int)sig);
			}
			break;
		}
		case CB_OPCODE_SMALL_LIGHT_SIG:
		{
			logDebug(RPCW, "Enter small light sig");
			unsigned char sig = rxbuf->data[0];
			if ((RPC_SMALL_LIGHT_SIG & rp->rpcIds_) && (rp->rpcRevFun_ != nullptr)) {
				rp->rpcRevFun_(RPC_SMALL_LIGHT_SIG, &sig, sizeof(sig));
				logInfoF(RPCW, "register small light sig : %d", sig);
			}
			break;
		}
        case CB_OPCODE_EOL_CHANGED:
        {
            logDebug(RPCW, "EOL setting has changed");
            EOLInfo ei = RX2EOL(*rxbuf);
            if ((RPC_EOL_CHANGED & rp->rpcIds_) && (rp->rpcRevFun_ != nullptr)) {
                rp->rpcRevFun_(RPC_EOL_CHANGED, &ei, sizeof(ei));
            }
            break;
        }
	}
	logVerbose(RPCW, "RpcProxyImpl::getVehicleSpeed OUT");
}

int RpcProxyImpl::convertKeyNum(unsigned char kn)
{
	return keyNumMap[kn];
}

int RpcProxyImpl::convertkeyStatus(unsigned char ks)
{
	return keyStatusMap[ks];
}

bool RpcProxyImpl::filterKeyEvent(RpcProxyImpl * rp, unsigned char kn, unsigned char ks)
{
	unsigned char ekn = keyNumMap[kn];
	unsigned char eks = keyStatusMap[ks];
	if (ekn & rp->keyIds_) {
		if (eks & rp->keyEvents_)
			return true;
	}
	return false;
}

bool RpcProxyImpl::registerKeyEvent(KeyID id, KeyEventID eid, KeyFunCB fcb)
{
	keyIds_ |= id;
	keyEvents_ |= eid;
	keyCBFun_ = fcb;
	return true;
}

bool RpcProxyImpl::registerKeyEventOnDown(KeyFunCB fcb)
{
	keyDownFun_ = fcb;
	return true;
}

void RpcProxyImpl::setSystemTime(const Time & t)
{
	logVerbose(RPCW, "RpcProxyImpl::setSystemTime IN");
	librpc_tx_t txInfo;
	// fill send info
	txInfo.control = 0x00;
	txInfo.opcode  = 0x0501;
	txInfo.datalen = 0x0019;
	memset(txInfo.data, 0x00, 25);
	memcpy(txInfo.data, &t, sizeof(t));
	librpc_send(rpcHandle_, &txInfo);
	logVerbose(RPCW, "RpcProxyImpl::setSystemTime OUT");
}

void RpcProxyImpl::setGpsInfo(const GpsInfo & gi)
{
	logVerbose(RPCW, "RpcProxyImpl::setGpsInfo IN");
	librpc_tx_t txInfo;
	librpc_rx_t rxInfo;
	// fill send info
	txInfo.control = 0x00;
	//txInfo.opcode  = 0x050C;
	txInfo.datalen = 0x0019;
	memset(txInfo.data, 0x00, 25);
	librpc_info(rpcHandle_, &txInfo, &rxInfo);
	// process return data
	//memcpy(&vehicleSpeed, rxInfo.data, sizeof(vehicleSpeed));
	logVerbose(RPCW, "RpcProxyImpl::setGpsInfo OUT");
}

void RpcProxyImpl::setAmpMode(uint8_t mode)
{
	logVerbose(RPCW, "RpcProxyImpl::setAmpMode IN mode:",mode);
	librpc_tx_t txInfo;
	// fill send info
	txInfo.control = 0x00;
	txInfo.opcode  = 0x0513;
	txInfo.datalen = 0x0019;
	memset(txInfo.data, 0x00, 25);
	txInfo.data[0] = mode;
	librpc_send(rpcHandle_, &txInfo);
	//Amplifier need 400ms to changed mode
	usleep(400*1000);
	logVerbose(RPCW, "RpcProxyImpl::setAmpMode OUT");
}

void RpcProxyImpl::setLinKeyWorkSta(bool isEnable)
{
	logVerbose(RPCW, "RpcProxyImpl::setLinKeyWorkSta IN isEnable:",isEnable);
	librpc_tx_t txInfo;
	// fill send info
	txInfo.control = 0x00;
	txInfo.opcode  = 0x0530;
	txInfo.datalen = 0x0019;
	memset(txInfo.data, 0x00, 25);
	if(isEnable){
		txInfo.data[0] = 0x01;
	}else{
		txInfo.data[0] = 0x00;
	}
	librpc_send(rpcHandle_, &txInfo);
	logVerbose(RPCW, "RpcProxyImpl::setLinKeyWorkSta OUT");
}

bool RpcProxyImpl::registerRpcEvent(RpcID id, RpcFunCB rf)
{
	rpcIds_ |= id;
	rpcRevFun_ = rf;
}
