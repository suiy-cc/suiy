/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef AMA_SETTYPES_H_
#define AMA_SETTYPES_H_

enum ama_setType_e{
    E_MediaVolMixWithNavi = 0,               // default:10                      The volume of the multimedia when the navigation sound and the multimedia sound coexist 0~30 default:10
    E_MeidaVolMixWithRadar = 1,                     // default:10                      The volume of the multimedia when the radar sound and the multimedia sound coexist 0~30 default:10
    E_NaviVolMixWithPhone = 2,                      // default:15                      Navigation volume for simultaneous phone and navigation sounds 0~30 default:15
    E_FrontRadarVol = 3,                            // default:20                      Front Radar sound range:15~30 default:20
    E_RearRadarVol = 4,                             // default:20                      Rear Radar sound range:15~30 default:20
    E_IVINotificationVol = 5,                       // default:10                      IVI The system notification volume(include SMS coming) range:0~30 default:10
    E_LRseNotificationVol = 6,                      // default:10                      LRse The system notification volume(include SMS coming) range:0~30 default:10
    E_RRseNotificationVol = 7,                      // default:10                      RRse The system notification volume(include SMS coming) range:0~30 default:10
    E_IVISoftKeyVol = 8,                            // default:10                      IVI SoftKey tone range:0~30 default:10
    E_LRseSoftKeyVol = 9,                           // default:10                      LRse SoftKey tone range:0~30 default:10
    E_RRseSoftKeyVol = 10,                           // default:10                      RRse SoftKey tone range:0~30 default:10
    E_IVIMediaVol = 11,                              // default:10                      IVI Multimedia volume range:0~30 default:10
    E_LRseMediaVol = 12,                             // default:10                      LRse Multimedia volume range:0~30 default:10
    E_RRseMediaVol = 13,                             // default:10                      RRse Multimedia volume range:0~30 default:10
    E_SMSTTSVol = 14,                                // default:15                      IVI SMS tone range:0~30 default:15
    E_VRTTSVol = 15,                                 // default:15                      IVI VR tone range:0~30 default:15
    E_NaviTTSVol = 16,                               // default:15                      IVI Navigation volume range:0~30 default:15
    E_PhoneVol = 17,                                 // default:15                      IVI Telephone volume range:0~30
    E_ECallVol = 18,                                 // default:20                      IVI ECALL range:15~30 default:20

    E_IVISoundBalance = 19,                          // default:0                       IVI Balance 音场自定义模式下可调整 range:-6~6 default:0
    E_IVISoundFader = 20,                            // default:0                       IVI Fader  音场自定义模式下可调整 range:-6~6   default:0

    E_IVISoundTreble = 21,                           // default:0                       IVI Sound-Treble range:-6~6 default:0
    E_LRseSoundTreble = 22,                          // default:0                       LRse Sound-Treble range:-6~6 default:0
    E_RRseSoundTreble = 23,                          // default:0                       IRRseVI Sound-Treble range:-6~6 default:0
    E_IVISoundMid = 24,                              // default:0                       IVI Sound-Treble range:-6~6 default:0
    E_LRseSoundMid = 25,                             // default:0                       LRse Sound-Treble range:-6~6 default:0
    E_RRseSoundMid = 26,                             // default:0                       RRse Sound-Treble range:-6~6 default:0
    E_IVISoundBass = 27,                             // default:0                       IVI Sound-Treble range:-6~6 default:0
    E_LRseSoundBass = 28,                            // default:0                       LRse Sound-Treble range:-6~6 default:0
    E_RRseSoundBass = 29,                            // default:0                       RRse Sound-Treble range:-6~6 default:0

    E_IVISoundEnviroment = 30,                       // default:0                       IVI Environmental sound  range:0(CenterPoint)、1(Stereo)、2(StereoDriver)、3(StereoRear) default:0--CenterPoint
    E_LRseSoundEnviroment = 31,                      // default:0                       LRse Environmental sound  range:0(CenterPoint)、1(Stereo)、2(StereoDriver)、3(StereoRear) default:0--CenterPoint
    E_RRseSoundEnviroment = 32,                      // default:0                       RRse Environmental sound  range:0(CenterPoint)、1(Stereo)、2(StereoDriver)、3(StereoRear) default:0--CenterPoint

    E_IsEnablePhoneMixNavi = 33,                     // default:0                       0:false phone can not mix with navi  1:true phone can mix with navi

    E_FrontRadarToneType = 34,                       // default:1                       Front The tone of the radar sound  range:1/2/3  default:1
    E_RearRadarToneType = 35,                        // default:1                       Rear The tone of the radar sound  range:1/2/3  default:1

    E_IsIVIMediaMute = 36,                           // default:0                       IVI MUTE switch      0:false mediaVol is Unmute  1:true mediaVol is mute default:0
    E_IsLRseMediaMute = 37,                          // default:0                       LRse MUTE switch      0:false mediaVol is Unmute  1:true mediaVol is mute default:0
    E_IsRRseMediaMute = 38,                          // default:0                       RRse MUTE switch      0:false mediaVol is Unmute  1:true mediaVol is mute default:0

    E_IsEnableFeedback = 39,                         // default:1                       Feedback enable switch 1:enable feedback when we change volume 0:disable feedback

    E_IsDisableRseUseSpeaker = 40,                   // default:0                       0:false - Rse can use speaker, 1:true - Rse seats can not use speaker
    E_IsOpenKidsMode = 41,                           // default:0                       0:close - Disable Kids Mode , 1:Open - enable Kids Mode
    E_IsDisableUseRse = 42,                          // default:0                       0:false - User can use Rse Screen , 1:true - User can not use Rse Screen

    E_IVIScrBrightness = 43,                         // default:50                      IVI Adjust screen brightness mode 0~100
    E_LRseScrBrightness = 44,                        // default:50                      LRse Adjust screen brightness mode 0~100
    E_RRseScrBrightness = 45,                        // default:50                      RRse Adjust screen brightness mode 0~100

    E_NaviVolMixWithPhoneNoFB = 46,                  // default:15                      same as E_NaviVolMixWithPhone but no feedback sound
    E_FeedbackVol = 47,                              // default:0                       set volume for feedback

    E_Key_Max
};


// #define STATMGR_MEDIA_VOL_HK          "MEDIA/VOL/HK"                     // default:0                       Press to adjust the volume  >0:increase <0:reduce default:0

#endif //AMA_SETTYPES_H_
