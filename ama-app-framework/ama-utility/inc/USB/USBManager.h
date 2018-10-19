/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef USBMANAGER_H
#define USBMANAGER_H
#include <memory>
#include "USB.h"
#include "USB_OTG.h"
#include "ama_enum.h"

extern "C"
{
#include "hwprop-0/libhwprop.h"
};
//call back function
typedef void(*USBStatusCB)(ama_USBSignal signal);

void StartListenUSBStateCB(USBStatusCB f);
void  StopListenUSBStateCB(USBStatusCB f);

class USBManager
{
public:
    USBManager();
    ~USBManager();

    void Init();
    static USBManager* GetInstance();
    void SetUSBStateCB(USBStatusCB f);
    void SetUSBStateCB();
    void getUSBStateAsync();
    void callUSBStateFunc(ama_USBSignal signal);

    bool UsbOperate(ama_USBOperate op);
    void SendMsgByCAPI(int val);

private :
    bool OpenFileHandle();
    void StartTimer();
    bool operate(ama_USBOperate op);

    USBStatusCB m_USBStatusCB;
    USB *usbHost ;
    USB_OTG *usbOtg ;
    char *usbFaultPath;
    bool isInit=false;
    bool isUsbHostOn;
    bool isUsbOtgOn;
};
#endif // USBMANAGER_H
