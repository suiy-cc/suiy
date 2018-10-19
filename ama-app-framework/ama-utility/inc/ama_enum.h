/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
//
// Created by zs on 16-12-29.
//

#ifndef AMA_ENUM_H
#define AMA_ENUM_H

#include "ama_powerTypes.h"

typedef unsigned int AMPID;
#define INVALID_AMPID 0

enum E_SEAT{
    // 0x00 is reserved for cluster
    // because cluster-screen's screenID is 0
    E_SEAT_TEST = 0x0, // for test in virtual machine & check seatID vaild
    E_SEAT_IVI  = 0x1,
    E_SEAT_RSE1 = 0x2,
    E_SEAT_RSE2 = 0x3,
//    E_SEAT_SHARE = 0x4, // for the solution of NAVI-share between ivi & cluster.

    E_SEAT_MAX,   //for vaildating/counting seatID
    E_SEAT_RSE,

    E_SEAT_INVALID = 0x8, // representing invalid return-value. a hint to error.

    E_SEAT_NONE = 0xF // for services own no screen
};

#define IS_SEATID_VALID(seatID)             ((seatID>=E_SEAT_IVI)&&(seatID<=E_SEAT_RSE2))

enum E_APPTYPE{
    E_APPTYPE_APP = 0x0,
    E_APPTYPE_SERVICE = 0x1,
    E_APPTYPE_AMGR = 0x2,

    E_APPTYPE_ANONYMOUS = 0xf
};

/*
 * AppStatus
 * contains process' visibility, availability, etc.
 * */
typedef unsigned int AppStatus;

/*Enum app ID
 */
typedef unsigned int E_APPID;
#define E_APPID_BOOT_ANIMATION 0x19
#define E_APPID_RVC 0x0F // RVC
#define E_APPID_AVM 0x22 // AVM

/*Enum layer
 */
enum E_LAYER{
    E_LAYER_NONE = 0x00,

    E_LAYER_RELEASE_PLAY = 0x01,
    E_LAYER_RELEASE_HMI = 0x02,
    E_LAYER_BACKGROUND = 0x03,
    E_LAYER_VIDEOPLAY = 0x04,
    E_LAYER_NORMALAPP = 0x05,
    E_LAYER_IME0 = 0x06,
    E_LAYER_ANIMATION = 0x07,
    E_LAYER_TOUCH_FORBIDDEN_NORMALAPP = 0x08,
    E_LAYER_UNUSED1 = 0x09,
    E_LAYER_UNUSED5 = 0x0A,
    E_LAYER_UNUSED6 = 0x0B,
    E_LAYER_TITLEBAR = 0x0C,
    E_LAYER_IME2 = 0x0D,
    E_LAYER_UNUSED7 = 0x0E,
    E_LAYER_NOTIFICATION = 0x0F,
    E_LAYER_IME3 = 0x10,
    E_LAYER_APP_MASK_AIO = 0x11,
    E_LAYER_CAMERABKG = 0x12,
    E_LAYER_BOOT_IMAGE_BKG = 0x13,
    E_LAYER_BOOT_IMAGE = 0x14,
    E_LAYER_UNUSED12 = 0x15,
    E_LAYER_UNUSED13 = 0x16,
    E_LAYER_BOOT_ANIMATION = 0x17,
    E_LAYER_UNUSED15 = 0x18,
    E_LAYER_UNUSED16 = 0x19,
    E_LAYER_UNUSED17 = 0x1A,
    E_LAYER_CAMERAVIDEO = 0x1B,
    E_LAYER_UNUSED18 = 0x1C,
    E_LAYER_UNUSED19 = 0x1D,
    E_LAYER_UNUSED1A = 0x1E,
    E_LAYER_UNUSED20 = 0x1F,
    E_LAYER_UNUSED21 = 0x20,
    E_LAYER_UNUSED22 = 0x21,
    E_LAYER_CAMERAGUIDLINE = 0x22,
    E_LAYER_UNUSED23 = 0x23,
    E_LAYER_UNUSED24 = 0x24,
    E_LAYER_UNUSED25 = 0x25,
    E_LAYER_UNUSED26 = 0x26,
    E_LAYER_CAMERAHMI = 0x27,
    E_LAYER_UNUSED27 = 0x28,
    E_LAYER_UNUSED28 = 0x29,
    E_LAYER_E_CALL = 0x2A,
    E_LAYER_IME4 = 0x2B,
    E_LAYER_APP_MASK_HYPERVISOR = 0x2C,
    E_LAYER_TOUCH_FORBIDDEN_CHILD_SAFETY_LOCK = 0x2D,
    E_LAYER_UNUSED32 = 0x2E,
    E_LAYER_TOUCH_FORBIDDEN_EVERYTHING = 0x2F,
    E_LAYER_GR = 0x30,

    E_LAYER_MAX
};

/*Debug Command
 */
enum E_DEBUG_CMD{
    E_SHOW_CONNECTIONS = 0x00,
    E_SHOW_AMSOURCES,
    E_SHOW_AMSINKS,
    E_SHOW_CONRECORD,
    E_SHOW_ROUTINGMAP,
    E_SHOW_FOCUSSTA,
    E_SHOW_AUDIOSETTING,
    E_SET_SEAT_IVI_USE_SPEAKER,
    E_SET_SEAT_LRSE_USE_SPEAKER,
    E_SET_SEAT_RRSE_USE_SPEAKER,
    E_SET_MASK_SURFACE_VISIBLE,
    E_SET_MASK_SURFACE_INVISIBLE,

    // HMIC debug command
    E_HMIC_RESTART,

    E_DEBUG_CMD_MAX
};

/**
 * the errors of the ama. All possible errors are in here. This enum is
 * used widely as return parameter.
 */
// enum E_ERROR
enum ama_Error_e
{
    /**
     * no error - positive reply
     */
        E_OK = 0,
    /**
     * default
     */
        E_UNKNOWN = -1,
    /**
     * value out of range
     */
        E_OUT_OF_RANGE = -2,
    /**
     * not used
     */
        E_NOT_USED = -3,
    /**
     * a database error occurred
     */
        E_DATABASE_ERROR = -4,
    /**
     * the desired object already exists
     */
        E_ALREADY_EXISTS = -5,
    /**
     * there is no change
     */
        E_NO_CHANGE = -6,
    /**
     * the desired action is not possible
     */
        E_NOT_POSSIBLE = -7,
    /**
     * the desired object is non existent
     */
        E_NON_EXISTENT = -8,
    /**
     * the asynchronous action was aborted
     */
        E_ABORTED = -9,
    /**
     * This error is returned in case a connect is issued with a connectionFormat that
     * cannot be selected for the connection. This could be either due to the
     * capabilities of a source or a sink or gateway compatibilities for example
     */
        E_WRONG_FORMAT = -10,

    E_NO_SOURCE = -11,

    E_NO_SINK = -12,

    E_NO_ROUTINGMAP = -13,

    /**
     * In the program run encountered a null pointer
     */
        E_NULL_PTR = -14,

    E_MAX
};

enum ama_USBSignal{
    E_USB_SIGNAL_NONE      = 0x00,      
    E_USB_OTG_OVERCURRENT  = 0xF0,      //发生过流
    E_USB_HOST_OVERCURRENT = 0XF1,      //发生过流
    E_USB_OTG_HAS_DEV      = 0xF2,      //检测到充电设备
    E_USB_OTG_HAVE_NO_DEV  = 0xF3       //未检测到充电设备
};

enum ama_USBOperate{
    E_USB_OTG_CHAGRE_ON   = 0x80,       //过流恢复
    E_USB_HOST_CHARGE_ON  = 0X81,       //过流恢复
    E_USB_OTG_TURNTO_DCP  = 0x82,       //切换到DCP充电模式
    E_USB_OTG_TURNTO_CDP  = 0x83,       //切换到CDP充电模式
    E_USB_OTG_CHARGE_OFF  = 0x84,
    E_USB_HOST_CHARGE_OFF = 0x85
};
#endif // AMA_ENUM_H
