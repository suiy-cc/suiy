/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef AMA_AUDIOTYPES_H_
#define AMA_AUDIOTYPES_H_

#include <stdint.h>

typedef uint16_t ama_AMPID_t;

typedef uint8_t ama_focusType_t;
typedef uint8_t ama_volumeType_t;
typedef uint8_t ama_audioDeviceType_t;

typedef uint16_t ama_streamType_t;

typedef uint32_t ama_streamID_t;
typedef uint32_t ama_connectID_t;

typedef uint32_t ama_connectInfo_t;//seat+streamType+deviceType
typedef uint8_t ama_volumeInfo_t;//volumeType+deviceType

typedef uint8_t ama_focusSta_t;
typedef uint8_t ama_ampMode_t;
typedef uint16_t ama_radarChannel_t;
typedef uint8_t ama_radarSoundType_t;

#define MAX_STREAM_TYPE         (0xFC)
#define MAX_STREAM_INDEX        (0x3F)
#define MUTE_VOL_STEP           (-1)
#define MAX_VOL_STEP            (30)
#define VIDEO_SHOW_STATE        (0x1000ff00)

enum ama_focusState_e{
    E_FOCUSUNKNOW = 0x00,
    E_FOCUSLOSE = 0x01,
    E_LOSE_TRANSIENT = 0x02,
    E_GAIN_FADE = 0x10,
    E_FOUCSGAIN = 0x11
};

enum ama_app_focusState_e{
    E_FOUCS_LOSE = 0x01,
    E_FOUCS_LOSE_TRANSIENT = 0x02,
    E_FOUCS_GAIN = 0x10
};

enum ama_audioDevice_e{
    E_IVI_MIC = 0x0,

	E_IVI_SPEAKER = 0x1,
	E_RSE_L_HEADSET = 0x2,
	E_RSE_R_HEADSET = 0x3,
    E_INVALID_DEVICE = 0xf
};

enum ama_radarChannel_e{
    E_NO_CHANNEL = 0x0000,
    E_FL_CHANNEL = 0x0001,
    E_FR_CHANNEL = 0x0010,
    E_RL_CHANNEL = 0x0100,
    E_RR_CHANNEL = 0x1000
};

enum ama_radarSoundType_e{
    E_NO_TONE = 0x00,
    E_CONTINUOUS_TONE = 0x01,
    E_HIGH_FREQ_TONE = 0x02,
    E_LOW_FREQ_TONE = 0x04,
    E_NORMAL_TONE = 0x05,
    E_RADAR_ALERT_TONE = 0x06
};

enum ama_streamType_e{
    //focusType = RADAR volumeType=frontRadar
    E_STREAM_FL_RADAR = 0,//0x00           //The audio Stream for radar Front Left
    E_STREAM_FR_RADAR,//0x04           //The audio Stream for radar Front Right
    //focusType = RADAR volumeType=RearRadar
    E_STREAM_RL_RADAR,//0x08           //The audio Stream for radar Rear Left
    E_STREAM_RR_RADAR,//0x0c           //The audio Stream for radar Rear Right
    //focusType = RADAR volumeType=NOTIFICATION
    E_STREAM_RADAR_ALERT,//0x10           //The audio Stream for radar alert
    //focusType = NOTIFICATION  volumeType=NOTIFICATION
    E_STREAM_NOTIFICATION,//0x14       //the audio Stream for notifications
    E_STREAM_BACKDOOR_NOTI,//0x18      //the audio Stream for RR backdoor notifications
    E_STREAM_FEEDBACK_NOTI,//0x1c      //the audio Stream for feedbackTone
    E_STREAM_AVM_ALERT,//0x20         //the audio Stream for feedbackTone
    E_STREAM_CARPLAY_ALT,//0x24
    //focusType = SOFTKEY volumeType=SOFTKEY
    E_STREAM_SOFTKEY,//0x28          //the audio Stream for soft key
    //focusType = MEDIA volumeType=MEDIA
    E_STREAM_TUNER,//0x2c           //the audio Stream for TUNER
    E_STREAM_MUSIC,//0x30           //the audio Stream for (EMMC,USB) music playback
    E_STREAM_VIDEO_AUDIO,//0x34     //the audio Stream for sound of video playback
    E_STREAM_BT_AUDIO,//0x38        //the audio Stream for sound of bt music
    E_STREAM_PHONELINK_AUDIO,//0x3c //the audio Stream for sound of phonelink
    E_STREAM_3THAPP_AUDIO,//0x40    //the audio Stream for sound of 3th app
    E_STREAM_FM,//0x44             //the audio stream for fm tuner
    E_STREAM_AM,//0x48             //the audio stream for am tuner
    E_STREAM_IPOD_AUDIO,//0x4c     //the audio stream for IPOD
    E_STREAM_CARPLAY_AUDIO,//0x50  //the audio stream for carplay
    E_STREAM_CARLIFE_AUDIO,//0x54  //the audio stream for carlife
    E_STREAM_FM_SEC,//0x58        //the audio stream for fm second channel
    //focusType = SMSTTS volumeType=SMSTTS
    E_STREAM_SMS_TTS,//0x5c         //the audio Stream for TTS of SMS
    //focusType = VRTTS volumeType=VRTTS
    E_STREAM_VR_TTS,//0x60          //the audio Stream for TTS of VR
    E_STREAM_CARPLAY_SIRI,//0x64          //the audio Stream for siri of carplay
    //focusType = NAVITTS volumeType=NAVITTS
    E_STREAM_NAVI_TTS,//0x68        //the audio Stream for TTS of navi
    //focusType = Phone volumeType=Phone
    E_STREAM_CALL_RING,//0x6c       //the audio Stream of call ring
    E_STREAM_BT_CALL,//0x70         //the audio Stream for bt call
    E_STREAM_CARPLAY_CALL,//0x74
    //focusType = B&I-CALL volumeType=Phone
    E_STREAM_B_CALL,//0x78          //The audio Stream for B_Call
    E_STREAM_I_CALL,//0x7c          //The audio Stream for I_Call
    //focusType = E-Call volumeType=E-Call
    E_STREAM_E_CALL,//0x80          //The audio Stream for E_Call
    //focusType = PowerOff volumeType=unknown
    E_STREAM_POWER_OFF,//0x84       //It's not a audio stream,it is control audio state by PowerOff

    //focusType = MIC volumeType=unknown
    E_STREAM_PHONE_MIC,//0x88       //the audio Stream for phone mic
    E_STREAM_VR_MIC,//0x8c          //the audio Stream for VR mic
    E_STREAM_B_CALL_MIC,//0x90      //the audio Stream for B_CALL mic
    E_STREAM_I_CALL_MIC,//0x94      //the audio Stream for I_CALL mic
    E_STREAM_E_CALL_MIC,//0x98      //the audio Stream for E_CALL mic
    E_STREAM_LOCAL_VR_MIC//0x9c     //the audio Stream for E_CALL mic
};

enum ama_streamType_Ex_e{
    //focusType = RADAR volumeType=frontRadar
    E_STREAM_EX_FL_RADAR = 0x0001,           //The audio Stream for radar Front Left
    E_STREAM_EX_FR_RADAR = 0x0002,           //The audio Stream for radar Front Right
    //focusType = RADAR volumeType=RearRadar
    E_STREAM_EX_RL_RADAR = 0x0011,           //The audio Stream for radar Rear Left
    E_STREAM_EX_RR_RADAR = 0x0012,           //The audio Stream for radar Rear Right
    //focusType = RADAR volumeType=NOTIFICATION
    E_STREAM_EX_RADAR_ALERT = 0x0003,           //The audio Stream for radar alert
    //focusType = NOTIFICATION  volumeType=NOTIFICATION
    E_STREAM_EX_NOTIFICATION = 0x0121,       //the audio Stream for notifications
    E_STREAM_EX_BACKDOOR_NOTI = 0x0122,      //the audio Stream for RR backdoor notifications
    E_STREAM_EX_FEEDBACK_NOTI = 0x0123,      //the audio Stream for feedbackTone
    E_STREAM_EX_AVM_ALERT = 0x0124,         //the audio Stream for feedbackTone
    E_STREAM_EX_CARPLAY_ALT = 0x0145,       //the audio Stream for alt of carplay
    //focusType = SOFTKEY volumeType=SOFTKEY
    E_STREAM_EX_SOFTKEY = 0x0231,          //the audio Stream for soft key
    //focusType = MEDIA volumeType=MEDIA
    E_STREAM_EX_TUNER = 0x0341,           //the audio Stream for TUNER
    E_STREAM_EX_MUSIC = 0x0342,           //the audio Stream for (EMMC,USB) music playback
    E_STREAM_EX_VIDEO_AUDIO = 0x0343,     //the audio Stream for sound of video playback
    E_STREAM_EX_BT_AUDIO = 0x0344,        //the audio Stream for sound of bt music
    E_STREAM_EX_PHONELINK_AUDIO = 0x0345, //the audio Stream for sound of phonelink
    E_STREAM_EX_3THAPP_AUDIO = 0x0346,    //the audio Stream for sound of 3th app
    E_STREAM_EX_FM = 0x0347,             //the audio stream for fm tuner
    E_STREAM_EX_AM = 0x0348,             //the audio stream for am tuner
    E_STREAM_EX_IPOD_AUDIO = 0x0349,     //the audio stream for IPOD
    E_STREAM_EX_CARPLAY_AUDIO = 0x034A,  //the audio stream for carplay
    E_STREAM_EX_CARLIFE_AUDIO = 0x034B,  //the audio stream for carlife
    E_STREAM_EX_FM_SEC = 0x034C,         //the audio stream for fm second channel
    //focusType = SMSTTS volumeType=SMSTTS
    E_STREAM_EX_SMS_TTS = 0x0451,         //the audio Stream for TTS of SMS
    //focusType = VRTTS volumeType=VRTTS
    E_STREAM_EX_VR_TTS = 0x0561,          //the audio Stream for TTS of VR
    E_STREAM_EX_CARPLAY_SIRI = 0x0562,      //the audio Stream for siri of carplay
    //focusType = NAVITTS volumeType=NAVITTS
    E_STREAM_EX_NAVI_TTS = 0x0671,        //the audio Stream for TTS of navi
    //focusType = Phone volumeType=Phone
    E_STREAM_EX_CALL_RING = 0x0781,       //the audio Stream of call ring
    E_STREAM_EX_BT_CALL = 0x0782,         //the audio Stream for bt call
    E_STREAM_EX_CARPLAY_CALL = 0x0783,    //the audio Stream for call of Carplay
    //focusType = B&I-CALL volumeType=Phone
    E_STREAM_EX_B_CALL = 0x0881,          //The audio Stream for B_Call
    E_STREAM_EX_I_CALL = 0x0882,          //The audio Stream for I_Call
    //focusType = E-Call volumeType=E-Call
    E_STREAM_EX_E_CALL = 0x0991,          //The audio Stream for E_Call
    //focusType = PowerOff volumeType=unknown
    E_STREAM_EX_POWER_OFF  = 0x0AF1,       //It's not a audio stream,it is control audio state by PowerOff

    //focusType = MIC volumeType=unknown
    E_STREAM_EX_PHONE_MIC = 0x0EF1,       //the audio Stream for phone mic
    E_STREAM_EX_VR_MIC = 0x0EF2,          //the audio Stream for VR mic
    E_STREAM_EX_B_CALL_MIC = 0x0EF3,      //the audio Stream for B_CALL mic
    E_STREAM_EX_I_CALL_MIC = 0x0EF4,      //the audio Stream for I_CALL mic
    E_STREAM_EX_E_CALL_MIC = 0x0EF5,      //the audio Stream for E_CALL mic
    E_STREAM_EX_LOCAL_VR_MIC = 0x0EF6     //the audio Stream for E_CALL mic
};

static uint16_t getStreamEXTable[MAX_STREAM_TYPE] =
{
/*ama_streamType_e*/            /*ama_streamType_Ex_e*/
/*E_STREAM_FL_RADAR*/           E_STREAM_EX_FL_RADAR,           //The audio Stream for radar Front Left
/*E_STREAM_FR_RADAR*/           E_STREAM_EX_FR_RADAR,           //The audio Stream for radar Front Right
/*E_STREAM_RL_RADAR*/           E_STREAM_EX_RL_RADAR,           //The audio Stream for radar Rear Left
/*E_STREAM_RR_RADAR*/           E_STREAM_EX_RR_RADAR,           //The audio Stream for radar Rear Right
/*E_STREAM_RADAR_ALERT*/        E_STREAM_EX_RADAR_ALERT,           //The audio Stream for radar alert
/*E_STREAM_NOTIFICATION*/       E_STREAM_EX_NOTIFICATION,       //the audio Stream for notifications
/*E_STREAM_BACKDOOR_NOTI*/      E_STREAM_EX_BACKDOOR_NOTI,      //the audio Stream for RR backdoor notifications
/*E_STREAM_FEEDBACK_NOTI*/      E_STREAM_EX_FEEDBACK_NOTI,      //the audio Stream for feedbackTone
/*E_STREAM_AVM_ALERT*/          E_STREAM_EX_AVM_ALERT,         //the audio Stream for feedbackTone
/*E_STREAM_CARPLAY_ALT*/        E_STREAM_EX_CARPLAY_ALT,         //the audio Stream for alt of carplay
/*E_STREAM_SOFTKEY*/            E_STREAM_EX_SOFTKEY,          //the audio Stream for soft key
/*E_STREAM_TUNER*/              E_STREAM_EX_TUNER,           //the audio Stream for TUNER
/*E_STREAM_MUSIC*/              E_STREAM_EX_MUSIC,           //the audio Stream for (EMMC,USB) music playback
/*E_STREAM_VIDEO_AUDIO*/        E_STREAM_EX_VIDEO_AUDIO,     //the audio Stream for sound of video playback
/*E_STREAM_BT_AUDIO*/           E_STREAM_EX_BT_AUDIO,        //the audio Stream for sound of bt music
/*E_STREAM_PHONELINK_AUDIO*/    E_STREAM_EX_PHONELINK_AUDIO, //the audio Stream for sound of phonelink
/*E_STREAM_3THAPP_AUDIO*/       E_STREAM_EX_3THAPP_AUDIO,    //the audio Stream for sound of 3th app
/*E_STREAM_FM*/                 E_STREAM_EX_FM,             //the audio stream for fm tuner
/*E_STREAM_AM*/                 E_STREAM_EX_AM,             //the audio stream for am tuner
/*E_STREAM_IPOD_AUDIO*/         E_STREAM_EX_IPOD_AUDIO,     //the audio stream for IPOD
/*E_STREAM_CARPLAY_AUDIO*/      E_STREAM_EX_CARPLAY_AUDIO,  //the audio stream for carplay
/*E_STREAM_CARLIFE_AUDIO*/      E_STREAM_EX_CARLIFE_AUDIO,  //the audio stream for carlife
/*E_STREAM_FM_SEC*/             E_STREAM_EX_FM_SEC,         //the audio stream for fm second channel
/*E_STREAM_SMS_TTS*/            E_STREAM_EX_SMS_TTS,         //the audio Stream for TTS of SMS
/*E_STREAM_VR_TTS*/             E_STREAM_EX_VR_TTS,          //the audio Stream for TTS of VR
/*E_STREAM_CARPLAY_SIRI*/       E_STREAM_EX_CARPLAY_SIRI,    //the audio Stream for siri of carplay
/*E_STREAM_NAVI_TTS*/           E_STREAM_EX_NAVI_TTS,        //the audio Stream for TTS of navi
/*E_STREAM_CALL_RING*/          E_STREAM_EX_CALL_RING,       //the audio Stream of call ring
/*E_STREAM_BT_CALL*/            E_STREAM_EX_BT_CALL,         //the audio Stream for bt call
/*E_STREAM_CARPLAY_CALL*/       E_STREAM_EX_CARPLAY_CALL,    //the audio Stream for call of carplay
/*E_STREAM_B_CALL*/             E_STREAM_EX_B_CALL,          //The audio Stream for B_Call
/*E_STREAM_I_CALL*/             E_STREAM_EX_I_CALL,          //The audio Stream for I_Call
/*E_STREAM_E_CALL*/             E_STREAM_EX_E_CALL,          //The audio Stream for E_Call
/*E_STREAM_POWER_OFF*/          E_STREAM_EX_POWER_OFF,       //It's not a audio stream,it is control audio state by PowerOff
/*E_STREAM_PHONE_MIC*/          E_STREAM_EX_PHONE_MIC,       //the audio Stream for phone mic
/*E_STREAM_VR_MIC*/             E_STREAM_EX_VR_MIC,          //the audio Stream for VR mic
/*E_STREAM_B_CALL_MIC*/         E_STREAM_EX_B_CALL_MIC,      //the audio Stream for B_CALL mic
/*E_STREAM_I_CALL_MIC*/         E_STREAM_EX_I_CALL_MIC,      //the audio Stream for I_CALL mic
/*E_STREAM_E_CALL_MIC*/         E_STREAM_EX_E_CALL_MIC,      //the audio Stream for E_CALL mic
/*E_STREAM_LOCAL_VR_MIC*/       E_STREAM_EX_LOCAL_VR_MIC     //the audio Stream for E_CALL mic};
};

enum ama_focusType_e{
    E_FOCUS_TYPE_RADAR = 0x00,
    E_FOCUS_TYPE_NOTIFICATION = 0x01,
    E_FOCUS_TYPE_SOFTKEY = 0x02,
    E_FOCUS_TYPE_MEDIA = 0x03,
    E_FOCUS_TYPE_SMSTTS = 0x04,
    E_FOCUS_TYPE_VRTTS = 0x05,
    E_FOCUS_TYPE_NAVITTS = 0x06,
    E_FOCUS_TYPE_PHONE = 0x07,
    E_FOCUS_TYPE_BICALL = 0x08,
    E_FOCUS_TYPE_ECALL = 0x09,
    E_FOCUS_TYPE_POWEROFF = 0x0A,
    E_FOCUS_TYPE_MAX = 0x0B,
    E_FOCUS_TYPE_MIC = 0x0E,
    E_FOCUS_TYPE_UNKNOWN = 0x0F
};

enum ama_volumeType_e{
    E_VOLUME_TYPE_FRONTRADAR = 0x0,
    E_VOLUME_TYPE_REARRADAR = 0x1,
    E_VOLUME_TYPE_NOTIFICATION = 0x2,
    E_VOLUME_TYPE_SOFTKEY = 0x3,
    E_VOLUME_TYPE_MEDIA = 0x4,
    E_VOLUME_TYPE_SMSTTS = 0x5,
    E_VOLUME_TYPE_VRTTS = 0x6,
    E_VOLUME_TYPE_NAVITTS = 0x7,
    E_VOLUME_TYPE_PHONE = 0x8,
    E_VOLUME_TYPE_ECALL = 0x9,
    E_VOLUME_TYPE_MAX = 0xA,
    E_VOLUME_TYPE_UNKNOWN = 0xF
};

#define CREATE_STREAMID_BY_AMPID_STREAMTYPE(AMPID, streamType)          (((AMPID & 0xFFFF) << 16)|((streamType&0x3F) << 10))
#define CREATE_STREAMID_BY_SEAT_APP_STREAMTYPE(seatID,appID,streamType) ((seatID<<28) | (appID<<16) | (streamType<<10))
#define SET_STREAMINDEX_FOR_STREAMID(streamIndex,streamID)              ((streamID) | (((streamIndex)&0x3F)<<4))

#define CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,deviceType)    ((streamID & 0xFFFFFFF0) | (deviceType & 0xF))
#define CREATE_CONNECTID_BY_AMPID_STREAMTYPE_DEVICETYPE(AMPID,streamType,deviceType) (((AMPID&0xFFFF)<<16)|((streamType&0x3F)<<10)|(deviceType&0xF))

#define GET_AMPID_FROM_STREAMID(streamID)                               ((streamID & 0xFFFF0000) >> 16)
#define GET_AMPID_FROM_CONNECTID(connectID)                             ((connectID & 0xFFFF0000) >> 16)

#define GET_STREAMTYPE_FROM_STREAMID(streamID)                          ((streamID & 0x0000FC00) >> 10)
#define GET_STREAMTYPE_EX_FROM_STREAMID(streamID)                       (getStreamEXTable[GET_STREAMTYPE_FROM_STREAMID(streamID)])

#define GET_FOCUSTYPE_FROM_STREAMID(streamID)                           (((GET_STREAMTYPE_EX_FROM_STREAMID(streamID))&0xFF00)>>8)
#define GET_VOLUMETYPE_FROM_STREAMID(streamID)                          (((GET_STREAMTYPE_EX_FROM_STREAMID(streamID))&0x00F0)>>4)

#define GET_STREAMID_FROM_CONNECTID(connectID)                          (connectID & 0xFFFFFFF0)
#define GET_STREAMTYPE_FROM_CONNECTID(connectID)                        GET_STREAMTYPE_FROM_STREAMID(connectID)
#define GET_FOCUSTYPE_FROM_CONNECTID(connectID)                         GET_FOCUSTYPE_FROM_STREAMID(connectID)
#define GET_VOLUMETYPE_FROM_CONNECTID(connectID)                        GET_VOLUMETYPE_FROM_STREAMID(connectID)
#define GET_DEVICETYPE_FROM_CONNECTID(connectID)                        (connectID & 0x0000000F)
#define GET_DEF_DEVICETYPE_FROM_STREAMID(streamID)                      ((streamID & 0xF0000000) >> 28)
#define GET_DEF_DEVICETYPE_FROM_CONNECTID(connectID)                    ((connectID & 0xF0000000) >> 28)
#define GET_SEATID_FROM_CONNECTID(connectID)                            ((connectID & 0xF0000000) >> 28)

#define GET_CONNECTINFO_FROM_CONNECTID(connectID)                       ((connectID)&0xF000FC0F)
#define CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(seatID,streamType,deviceType)      ((seatID<<28) | (streamType<<10) | (deviceType))

#define GET_VOLUMEINFO_FROM_CONNECTID(connectID)                        (((GET_VOLUMETYPE_FROM_CONNECTID(connectID))<<4) | (connectID&0x0000000F))
#define CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(volumeType,deviceType)  ((volumeType<<4) | (deviceType))
#define GET_VOLUMEINFO_FROM_VOLUMEINFO(volumeInfo)                      (((volumeInfo)&0xf0)>>4)

#define IS_ALREADY_HAVE_FOCUS(FocusSta)                                 ((FocusSta)&0xF0)

#endif //AMA_AUDIOTYPES_H_
