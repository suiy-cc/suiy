/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef AMA_STATETYPES_H_
#define AMA_STATETYPES_H_
#ifdef CODE_FOR_EV
enum ama_stateType_e{
    E_SpeakerWho             = 0x00,    //default:1  The speaker who preemption  1:IVI  2:RSE1  3:RSE2
    E_ActiveStreamForHardKey = 0x01,    //default:5  sameDef as ama_streamTypeClass_e 5:MEDIA 7:VRTTS 9:Phone 10:ECALL
    //0x2~0xf  Occupied by hs7 don't use
    E_RVC_REVERSE_SATATE     = 0x10,    //0x00:RVC_OUT                                        0x01:RVC_IN
    E_USB_SIGNAL_CHANGED     = 0x11,    //see ama_USBSignal in ama_enum.h
    E_IS_IVI_MUTE            = 0x21,    //"0":unmute      "1":mute
    E_IS_LRSE_MUTE           = 0x22,    //"0":unmute      "1":mute
    E_IS_RRSE_MUTE           = 0x23,    //"0":unmute      "1":mute
    E_MODULE_EV_FCW             = 0x64, //100 0x00:Without FCW                                    0x01:With FCW
    E_MODULE_EV_AEB             = 0x65, //101 0x00:Without AEB  Switch Setting                    0x01:With AEB Switch Setting
    E_MODULE_EV_BSD             = 0x66, //102 0x00:Without BSD Switch Setting                     0x01:With BSD Switch Setting
    E_MODULE_EV_LDW             = 0x67, //103 0x00:Without LDW Sensitivity Setting                0x01:With LDW Sensitivity Setting LDW
    E_MODULE_EV_LKA             = 0x68, //104 0x00:Without LKA  Switch Setting                    0x01:With LKA Switch Setting
    E_MODULE_EV_DSM             = 0x69, //105 0x00:Without DSM Switch Setting                     0x01:With DSM Switch Setting
    E_MODULE_EV_LDWS            = 0x6a, //106 0x00:Without LDW Switch Setting                     0x01:With LDW Switch Setting
    E_MODULE_EV_TRM             = 0x6b, //107 0x00:Without Turn of Right Mirror (when reversing) Switch Setting   0x01:With Turn of Right Mirror (when reversing) Switch Setting
    E_MODULE_EV_HBA             = 0x6c, //108 0x00:Without High-beam Assist Switch Setting        0x01:With High-beam Assist Switch Setting
    E_MODULE_EV_AP              = 0x6d, //109 0x00:Without Auto-Parking                           0x01:With Auto-Parking
    E_MODULE_EV_TSR             = 0x6e, //110 0x00:Without TSR Switch Setting                     0x01:With TSR Switch Setting
    E_MODULE_EV_LANG            = 0x6f, //111 0x00:Chinese                                        0x01:English
    E_MODULE_EV_ALC             = 0x70, //112 0x00:Without Atmosphere Lamp Color Setting          0x01:With Atmosphere Lamp Color Setting
    E_MODULE_EV_ALCD            = 0x71, //113 0x00:Without Atmosphere Lamp Color Changing (follow the change of drive mode)Switch Setting    0x01:With Atmosphere Lamp Color Changing (follow the change of drive mode)Switch Setting
    E_MODULE_EV_SCM             = 0x72, //114 0x00:Without SCM Comfort Access Switch Setting      0x01:With SCM Comfort Access Switch Setting
    E_MODULE_EV_DSCU            = 0x73, //115 0x00:Without DSCU Comfort Access Switch Setting     0x01:With DSCU Comfort Access Switch Setting
    E_MODULE_EV_HUD             = 0x74, //116 0x00:Without HUD                                    0x01:With HUD
    E_MODULE_EV_DVR             = 0x75, //117 0x00:Without Car DVR                                0x01:With Car DVR
    E_MODULE_EV_EFI             = 0x76, //118 0x00:Energy Flow Information Display of 2WD         0x01:Energy Flow Information Display of 4WD
    E_MODULE_EV_MSMTD           = 0x77, //119 0x00:Motor Speed and Motor Torque Display of 2WD    0x01:Motor Speed and Motor Torque Dispay of 4WD
    E_MODULE_EV_BMFS            = 0x78, //120 0x00:MOD Button of MFS                              0x01:HUD Button of MFS
    E_MODULE_EV_ALL             = 0x79, //121 0x00:Without Atmosphere Lamp Level Setting          0x01:With Atmosphere Lamp Level Setting
    E_MODULE_EV_CURISE          = 0x7a, //122 0x00:Cruise control                                 0x01:Advanced Cruise
    E_MODULE_EV_TDMC            = 0x7b, //123 0x00: Arctic White 0x01: Flame Red 0x02: Qingchuan Blue 0x03: Platinum Gold (Default 0x00)
    E_StateType_NONE
};

#else
enum ama_stateType_e{
    E_SpeakerWho             = 0x00,    //default:1  The speaker who preemption  1:IVI  2:RSE1  3:RSE2
    E_ActiveStreamForHardKey = 0x01,    //default:5  sameDef as ama_streamTypeClass_e 5:MEDIA 7:VRTTS 9:Phone 10:ECALL
    E_MODULE_FCW             = 0x02,    //0x00:Without FCW                                    0x01:With FCW
    E_MODULE_AEB             = 0x03,    //0x00:Without AEB  Switch Setting                    0x01:With AEB Switch Setting
    E_MODULE_MoPhCon         = 0x04,    //Mobile phone interconnection 0x00:Without carplay+carlife+Welink 0x01:With carplay+carlife+Welink
    E_MODULE_LDW             = 0x05,    //0x00:Without LDW Sensitivity Setting                0x01:With LDW Sensitivity Setting LDW
    E_MODULE_LKA             = 0x06,    //0x00:Without LKA  Switch Setting                    0x01:With LKA Switch Setting
    E_MODULE_DSM             = 0x07,    //0x00:Without DSM Switch Setting                     0x01:With DSM Switch Setting
    E_MODULE_RIorAVS         = 0x08,    //0x00:RI����Ӱ��                                     0x01:AVSȫ��Ӱ��
    E_MODULE_RPE             = 0x09,    //0x00:Without Rear Passenger entertainment function  0x01:With Rear Passenger entertainment function
    E_MODULE_HBA             = 0x0A,    //0x00:Without High-beam Assist Switch Setting        0x01:With High-beam Assist Switch Setting
    E_MODULE_AP              = 0x0B,    //0x00:Without Auto-Parking                           0x01:With Auto-Parking
    E_MODULE_TBOX            = 0x0C,    //0x00:Without T-box                                  0x01:With T-box
    E_MODULE_LANG            = 0x0D,    //0x00:Chinese����                                    0x01:EnglishӢ��
    E_MODULE_WIFI            = 0x0E,    //0x00:Without Wifi                                   0x01:With Wifi
    E_MODULE_BROW            = 0x0F,    //0x00:Without Browser                                0x01:With Browser
    E_RVC_REVERSE_SATATE     = 0x10,    //0x00:RVC_OUT                                        0x01:RVC_IN
    E_USB_SIGNAL_CHANGED     = 0x11,    //see ama_USBSignal in ama_enum.h
    E_IS_IVI_MUTE            = 0x21,    //"0":unmute      "1":mute
    E_IS_LRSE_MUTE           = 0x22,    //"0":unmute      "1":mute
    E_IS_RRSE_MUTE           = 0x23,    //"0":unmute      "1":mute
	E_MODULE_DMSCfg          = 0x64,    //100 0x00:Normal                       0x01:All terrain
	E_MODULE_DVR             = 0x65,    //101 0x00:Without Car DVR              0x01:With Car DVR
	E_MODULE_AVS             = 0x66,    //102 0x00:Without AVS MOD              0x01:With AVS MOD
	E_MODULE_CURISE          = 0x67,    //103 0x00:Cruise control               0x01:Advanced Cruise
	E_MODULE_TDMC            = 0x68,    //104 0x00: Arctic White 0x01: Flame Red 0x02: Qingchuan Blue 0x03: Platinum Gold (Default 0x00)
    E_StateType_NONE
};
#endif

#endif //AMA_STATETYPES_H_
