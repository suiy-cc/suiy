/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef _RPC_PROXY_DEF_H_
#define _RPC_PROXY_DEF_H_

// ---------------------- Key ---------------------
/**
 * \brief  Key ID define
 */
enum KeyID
{
	KEY_NONE          = 0x0000,
	KEY_PWR           = 0x0001,
	E_KEY_HOME          = 0x0002,
	KEY_VOLUME        = 0x0004,
	E_KEY_MUTE          = 0x0008,
	KEY_VR            = 0x0010,

	KEY_WHEEL_VOL_UP_DOWN  = 0x0020,
	KEY_WHEEL_PHONE_ANSWER = 0x0040,
	KEY_WHEEL_PHONE_REJECT = 0x0080,
	KEY_WHEEL_PHONE_MUTE   = 0x0100,
	KEY_WHEEL_VOICE_RECOGNITION = 0x0200,
	KEY_WHEEL_HUD_MODE     = 0x0400,

	// ...
	KEY_ALL           = 0x00FF,
};

enum RpcID
{
	RPC_SPEED_CHANGE    = 0x0001,
	RPC_PWR_LIMIT_SIG   = 0x0002,
	RPC_SMALL_LIGHT_SIG = 0x0004,
    RPC_EOL_CHANGED     = 0x0008,

    RPC_ALL             = 0xFFFF,
};
/**
 * \brief  Key Event ID define
 */
enum KeyEventID
{
	KEY_EVENT_NONE    = 0x00,
	KEY_EVENT_UP      = 0x01,
	KEY_EVENT_DOWN    = 0x02,
	KEY_EVENT_LONG_3s = 0x04,
	KEY_EVENT_LONG_7s = 0x08,

    KEY_EVENT_1P5S_OUT = 0x10,
	KEY_EVENT_OUT_OFF  = 0x20,
	// ...
	KEY_EVENT_ALL     = 0xFF,
};

/**
 * \brief  Key callback function
 */
typedef void (*KeyFunCB)(KeyID id, KeyEventID eid, void * data);

typedef void (*RpcFunCB)(RpcID id, void * data, int len);

// ---------------------- Common ------------------

struct Time
{
	unsigned short Year;
	unsigned char Month;
	unsigned char Day;
	unsigned char Hour;
	unsigned char Minute;
	unsigned char Second;
} __attribute__((packed, aligned(1)));

struct SysInfo
{
	char Version[4];
	char Date[8];
} __attribute__((packed, aligned(1)));

#if 0
ev-----------------------------------------------------------------------------
0x0C20	7	FCW前碰撞预警开关设置	0x00:Without FCW       0x01:With FCW         (默认值0x01)
		8	AEB先进紧急制动开关设置	0x00:Without AEB  Switch Setting         0x01:With AEB Switch Setting      (默认值0x01)
		9	BSD盲区探测开关设置	0x00:Without BSD Switch Setting         0x01:With BSD Switch Setting    (默认值0x01)
		10	LDW警报机制设置	0x00:Without LDW Sensitivity Setting     0x01:With LDW Sensitivity Setting LDW        (默认值0x01)
		11	LKA车道保持辅助开关设置	0x00:Without LKA  Switch Setting     0x01:With LKA Switch Setting    (默认值0x01)
		12	DSM驾驶员疲劳监测开关设置	0x00:Without DSM Switch Setting   0x01:With DSM Switch Setting     (默认值0x01)
		13	LDW车道偏离 LDW 开关设置	0x00:Without LDW Switch Setting     0x01:With LDW Switch Setting  (默认值0x01)
		14	倒车时右后视镜下降开关设置	0x00:Without Turn of Right Mirror (when reversing) Switch Setting  0x01:With Turn of Right Mirror (when reversing) Switch Setting(默认值0x01)
		15	远光辅助开关设置	0x00:Without High-beam Assist Switch Setting 0x01:With High-beam Assist Switch Setting   (默认值0x01)
		16	自动泊车设置	0x00:Without Auto-Parking    0x01:With Auto-Parking   (默认值0x01)
		17	超速报警 TSR 开关设置	0x00:Without TSR Switch Setting    0x01:With TSR Switch Setting    (默认值0x01)
		18	语言设置	0x00:Chinese中文   0x01:English英文     (默认值0x00)
		19	氛围灯颜色设置	0x00:Without Atmosphere Lamp Color Setting   0x01:With Atmosphere Lamp Color Setting       (默认值0x01)
		20	氛围灯颜色跟随驾驶模式切换开关设置	0x00:Without Atmosphere Lamp Color Changing (follow the change of drive mode)Switch Setting 0x01:With Atmosphere Lamp Color Changing (follow the change of drive mode)Switch Setting  (默认值0x01)    "
		21	转向柱自动退让开关设置	0x00:Without SCM Comfort Access Switch Setting 0x01:With SCM Comfort Access Switch Setting    (默认值0x01)
		22	座椅自动退让开关设置	0x00:Without DSCU Comfort Access Switch Setting 0x01:With DSCU Comfort Access Switch Setting  (默认值0x01)
		23	HUD	0x00:Without HUD  0x01:With HUD (默认值0x01)
		24	行车记录仪	0x00:Without Car DVR     0x01:With Car DVR              (默认值0x01)
		25	两驱/四驱模式能量流信息显示	0x00:Energy Flow Information Display of 2WD    0x01:Energy Flow Information Display of 4WD    (默认值0x01)
		26	一组/二组电机转速及电机扭矩显示	0x00:Motor Speed and Motor Torque Display of 2WD   0x01:Motor Speed and Motor Torque Dispay of 4WD   (默认值0x01)
		27	多功能方向盘 MOD/HUD 按键功能	预留     (默认值0x00)   delete
		28	氛围灯亮度设置	0x00:Without Atmosphere Lamp Level Setting   0x01:With Atmosphere Lamp Level Setting  (默认值0x01)
		29	巡航系统	0x00:定速巡航  0x01:高级巡航  (默认值0x01)
		30  3D 车模颜色  0x00:北极白 0x01:烈焰红  0x02:青川蓝 0x03:铂光金 (默认值0x00)
ev-----------------------------------------------------------------------------
hs7----------------------------------------------------------------------------
0x0C13	7	FCW前碰撞预警开关设置	0x00:Without FCW  0x01:With FCW     (默认值0x00)
		8	AEB先进紧急制动开关设置	0x00:Without AEB  Switch Setting    0x01:With AEB Switch Setting      (默认值0x00)
		9	手机互联设置(carplay+carlife+Welink)	0x00:Without carplay+carlife+Welink     0x01:With carplay+carlife+Welink           (默认值0x00)
		10	LDW警报机制设置	0x00:Without LDW Sensitivity Setting            0x01:With LDW Sensitivity Setting LDW               (默认值0x00)
		11	LKA车道保持辅助开关设置	0x00:Without LKA  Switch Setting       0x01:With LKA Switch Setting      (默认值0x00)
		12	DSM驾驶员疲劳监测开关设置	0x00:Without DSM Switch Setting          0x01:With DSM Switch Setting    (默认值0x00)
		13	RI倒车影像/AVS全景影像设置	0x00:RI倒车影像     0x01:AVS全景影像          (默认值0x00)
		14	后排娱乐功能设置	0x00:Without Rear Passenger entertainment function   0x01:With Rear Passenger entertainment function    (默认值0x00)
		15	远光辅助开关设置	0x00:Without High-beam Assist Switch Setting    0x01:With High-beam Assist Switch Setting               (默认值0x00)
		16	自动泊车设置	0x00:Without Auto-Parking  0x01:With Auto-Parking                  (默认值0x00)
		17	T-box设置	0x00:Without T-box        0x01:With T-box       (默认值0x00)
		18	语言设置	0x00:Chinese中文  0x01:English英文         (默认值0x00)
		19	WIFI设置	0x00:Without Wifi       0x01:With Wifi             (默认值0x00)
		20	浏览器设置	0x00:Without Browser        0x01:With Browser      (默认值0x00)
		21	DMSCfg	0x00:Normal 普通    0x01:All terrain 全地形                                                                                                                                                                                                                                                                     (默认值0x00)    
        22	DVR	0x00:Without Car DVR    0x01:With Car DVR 行车记录仪                                                                                                                                                                                                                                                                 (默认值0x00)    
        23	AVS MOD	0x00:Without AVS MOD   0x01:With AVS MOD 全景影像控制器 MOD 功能                                                                                                                                                                                                                                                                 (默认值0x00)    
        24	巡航系统	0x00:CC 定速巡航   0x01:SACC 高级巡航                                                                                                                                                                                                                                                                    (默认值0x00)   
        25	3D 车模颜色	0x00:冰川白 0x01:岩浆棕 0x02:墨玉黑 0x03:荣耀蓝 0x04:流沙金  (默认值0x00)
hs7----------------------------------------------------------------------------
hs5-----------------------------------------------------------------------------
0x0C20	7	FCW前碰撞预警开关设置	0x00:Without FCW       0x01:With FCW         (默认值0x01)
		8	AEB先进紧急制动开关设置	0x00:Without AEB  Switch Setting         0x01:With AEB Switch Setting      (默认值0x01)
		9	BSD盲区探测开关设置	0x00:Without BSD Switch Setting         0x01:With BSD Switch Setting    (默认值0x01)
		10	LDW警报机制设置	0x00:Without LDW Sensitivity Setting     0x01:With LDW Sensitivity Setting LDW        (默认值0x01)
		11	LKA车道保持辅助开关设置	0x00:Without LKA  Switch Setting     0x01:With LKA Switch Setting    (默认值0x01)
		12	DSM驾驶员疲劳监测开关设置	0x00:Without DSM Switch Setting   0x01:With DSM Switch Setting     (默认值0x01)
		13	LDW车道偏离 LDW 开关设置	0x00:Without LDW Switch Setting     0x01:With LDW Switch Setting  (默认值0x01)
		14	倒车时右后视镜下降开关设置	0x00:Without Turn of Right Mirror (when reversing) Switch Setting  0x01:With Turn of Right Mirror (when reversing) Switch Setting(默认值0x01)
		15	远光辅助开关设置	0x00:Without High-beam Assist Switch Setting 0x01:With High-beam Assist Switch Setting   (默认值0x01)
		16	自动泊车设置	0x00:Without Auto-Parking    0x01:With Auto-Parking   (默认值0x01)
		17	超速报警 TSR 开关设置	0x00:Without TSR Switch Setting    0x01:With TSR Switch Setting    (默认值0x01)
		18	语言设置	0x00:Chinese中文   0x01:English英文     (默认值0x00)
		19	氛围灯颜色设置	0x00:Without Atmosphere Lamp Color Setting   0x01:With Atmosphere Lamp Color Setting       (默认值0x01)
		20	氛围灯颜色跟随驾驶模式切换开关设置	0x00:Without Atmosphere Lamp Color Changing (follow the change of drive mode)Switch Setting 0x01:With Atmosphere Lamp Color Changing (follow the change of drive mode)Switch Setting  (默认值0x01)    "
		21	转向柱自动退让开关设置	0x00:Without SCM Comfort Access Switch Setting 0x01:With SCM Comfort Access Switch Setting    (默认值0x01)
		22	座椅自动退让开关设置	0x00:Without DSCU Comfort Access Switch Setting 0x01:With DSCU Comfort Access Switch Setting  (默认值0x01)
		23	HUD	0x00:Without HUD  0x01:With HUD (默认值0x01)
		24	行车记录仪	0x00:Without Car DVR     0x01:With Car DVR              (默认值0x01)
		25	氛围灯亮度设置	0x00:Without Atmosphere Lamp Level Setting   0x01:With Atmosphere Lamp Level Setting  (默认值0x01)
		26	RI倒车影像/AVS全景影像设置	0x00:RI倒车影像     0x01:AVS全景影像          (默认值0x00)
hs5-----------------------------------------------------------------------------
#endif
#ifdef CODE_FOR_EV
struct EOLInfo
{
	unsigned char FCW;
	unsigned char AEB;
	unsigned char BSD;
	unsigned char LDW;
	unsigned char LKA;
	unsigned char DSM;
	unsigned char LDWS;
	unsigned char TRM;
	unsigned char HBA;//High-beam Assist Switch Setting
	unsigned char AP;//Auto-Parking
	unsigned char TSR;
	unsigned char LANG;
	unsigned char ALC;
	unsigned char ALCD;
	unsigned char SCM;
	unsigned char DSCU;
	unsigned char HUD;
	unsigned char DVR;
	unsigned char EFI;
	unsigned char MSMTD;
	unsigned char BMFS;
	unsigned char ALL;
	unsigned char CURISE;
	unsigned char TDMC;
} __attribute__((packed, aligned(1)));
#else // CODE_FOR_EV
struct EOLInfo
{
    unsigned char FCW;
    unsigned char AEB;
    unsigned char MPI;//carplay+carlife+Welink
    unsigned char LDW;
    unsigned char LKA;
    unsigned char DSM;
    unsigned char RI_AVS;
    unsigned char RPE;//Rear Passenger entertainment
    unsigned char HBA;//High-beam Assist Switch Setting
    unsigned char AP;//Auto-Parking
    unsigned char TBOX;
    unsigned char LANG;
    unsigned char WIFI;
    unsigned char BROW;
	unsigned char DMSCfg;
	unsigned char DVR;
	unsigned char AVS;
	unsigned char CURISE;
	unsigned char TDMC;
} __attribute__((packed, aligned(1)));
#endif // CODE_FOR_EV

#ifdef CODE_FOR_HS5
struct EOLInfo_HS5
{
	unsigned char FCW;
	unsigned char AEB;
	unsigned char BSD;
	unsigned char LDW;
	unsigned char LKA;
	unsigned char DSM;
	unsigned char LDWS;
	unsigned char TRM;
	unsigned char HBA;//High-beam Assist Switch Setting
	unsigned char AP;//Auto-Parking
	unsigned char TSR;
	unsigned char LANG;
	unsigned char ALC;
	unsigned char ALCD;
	unsigned char SCM;
	unsigned char DSCU;
	unsigned char HUD;
	unsigned char DVR;
	unsigned char ALL;
	unsigned char RI_AVS;
} __attribute__((packed, aligned(1)));
#endif // CODE_FOR_HS5

struct GpsInfo
{
	uint32_t x;
	uint32_t y;
};

#endif // _RPC_PROXY_DEF_H_
