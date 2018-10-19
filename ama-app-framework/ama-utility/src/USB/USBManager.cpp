/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include <mutex>
#include <map>
#include <thread>
#include <sys/time.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <iostream>
#include <string>
#include <sys/prctl.h>
#include "USBManager.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

LOG_DECLARE_CONTEXT(USBM);
static pthread_t timerThread=0;//for count time
static bool USBThreadEndFlag=false;
static bool IsListenUSBState=false;
static int m_fileHandle=0;
//for count timer every 20 ms
static void *USBManagerTimer(void *)
{
    // set thread name
    prctl(PR_SET_NAME, "USBManagerTimer");

    int ret;
    int epoll_fd;

    unsigned char val;

    struct epoll_event m_events[1024];
    struct epoll_event ev;
    memset(m_events,0,sizeof(m_events));

    epoll_fd = epoll_create(1024);
    ev.events = EPOLLIN|EPOLLPRI;
    ev.data.fd = m_fileHandle;
    ret = epoll_ctl(epoll_fd,EPOLL_CTL_ADD,m_fileHandle,&ev);
    if(ret < 0)
        logErrorF(USBM,"Epoll ctl add error");

    while(!USBThreadEndFlag)
    {
        int nfds = epoll_wait (epoll_fd, m_events, 1024, 50);
        for(int i = 0;i < nfds;i++)
        {
            if(m_events[i].data.fd == m_fileHandle)
            {
                ret = read(m_fileHandle,&val,1);
                if(ret == 1)
                {
                    logInfoF(USBM,"USBM read val = 0x%02x",val);
                  //  USBManager::GetInstance()->SendMsgByCAPI(val);
                    if (IsListenUSBState)
                          USBManager::GetInstance()->callUSBStateFunc((ama_USBSignal)val);
                }
                //continue;
            }
        }
      //  std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
void StartListenUSBStateCB(USBStatusCB f)
{
    logVerbose(USBM,"USBManager()-->StartListenUSBStateCB  IN ");
    USBManager::GetInstance()->SetUSBStateCB(f);
    IsListenUSBState=true;

    logVerbose(USBM,"USBManager()-->StartListenUSBStateCB OUT  IsListenUSBState:",IsListenUSBState);
}
void StopListenUSBStateCB(USBStatusCB f)
{
    logVerbose(USBM,"USBManager()-->StopListenUSBStateCB  IN ");
    IsListenUSBState=false;
    logVerbose(USBM,"USBManager()-->StopListenUSBStateCB OUT ");
}
USBManager::USBManager()
:isUsbHostOn(false)
,isUsbOtgOn(false)
{
    RegisterContext(USBM,"USBM");
    logVerboseF(USBM,"USBManager::USBManager()-->IN");
    usbFaultPath={0};
    USBThreadEndFlag=false;
//    usbHost =new USB();
//    usbOtg =new USB_OTG();
    Init();
    logVerboseF(USBM,"USBManager::USBManager()-->OUT");
}
USBManager::~USBManager()
{
    logVerboseF(USBM,"USBManager::~USBManager()-->IN");
    USBThreadEndFlag=true;
    pthread_join(timerThread,NULL);
    if (usbFaultPath!=NULL)
        ama_free(usbFaultPath);

    logVerboseF(USBM,"USBManager::~USBManager()-->OUT");
}
void USBManager::Init()
{
    logVerboseF(USBM,"USBManager::init()-->IN");
    usbFaultPath=(char *) hw_get_system_property_string(HW__USB__FAULT__DEV_NODE);
    OpenFileHandle();
    StartTimer();
    isInit=true;
    logVerboseF(USBM,"USBManager::init()-->OUT");
}

void USBManager::StartTimer()
{

    logVerboseF(USBM,"USBManager::startTimer()-->IN");
    //创建计时线程
    int ret=0;
    ret=pthread_create(&timerThread,NULL,USBManagerTimer,NULL);
    if(ret!=0){
        logError (USBM,"USBManager::startTimer() Create thread error!");
    }
    else
    {
        logInfo (USBM,"USBManager::startTimer() Create thread success!");
    }

    logVerboseF(USBM,"USBManager::startTimer()-->OUT");
}
bool USBManager::OpenFileHandle()
{
    logVerboseF(USBM,"USBManager::OpenFileHandle()-->IN");
    if ((usbFaultPath==NULL)||(0==strcmp(usbFaultPath,"")))
    {
        logErrorF(USBM,"usbFaultPath path is empty!!!!");
        return false;
    }
    m_fileHandle = open(usbFaultPath,O_RDWR);

    if(m_fileHandle < 0)
    {
        logErrorF(USBM," open usb fault failed : %s",usbFaultPath);
        return false;
    }
    logVerboseF(USBM,"USBManager::OpenFileHandle()-->OUT");
    return true;
}
void USBManager::SendMsgByCAPI(int val)
{
    logVerboseF(USBM,"USBManager::SendMsgByCAPI --> IN val:0x%04x",val);

    logVerbose(USBM,"USBManager::SendMsgByCAPI -->OUT");
}
void USBManager::SetUSBStateCB(USBStatusCB f)
{
    logVerbose(USBM,"USBManager::SetUSBStateCB -->IN");
    m_USBStatusCB=f;
    logVerbose(USBM,"USBManager::SetUSBStateCB -->OUT");
}

void USBManager::getUSBStateAsync()
{
    logVerbose(USBM,"USBManager::getUSBStateAsync -->IN");
    if(m_fileHandle < 0) return;

    unsigned char val;
    int ret = read(m_fileHandle,&val,1);
    if(ret == 1)
    {
        logInfoF(USBM,"USBM read val = 0x%02x",val);
        //  USBManager::GetInstance()->SendMsgByCAPI(val);
        if (IsListenUSBState)
                USBManager::GetInstance()->callUSBStateFunc((ama_USBSignal)val);
    }
    logVerbose(USBM,"USBManager::getUSBStateAsync -->OUT");
}

void USBManager::callUSBStateFunc(ama_USBSignal signal)
{
    logVerboseF(USBM,"USBManager::callUSBStateFunc -->IN signal:%04x",signal);
    switch(signal)
    {
        case E_USB_OTG_OVERCURRENT:
            isUsbOtgOn = false;
            break;
        case E_USB_HOST_OVERCURRENT:
            isUsbHostOn = false;
            break;
    }
    m_USBStatusCB(signal);
    logVerbose(USBM,"USBManager::callUSBStateFunc -->OUT");
}
bool USBManager::UsbOperate(ama_USBOperate op)
{
    logVerbose(USBM,"USBManager::UsbOperate -->IN");

    bool isOperateSuccessed = false;
    switch(op){
    case E_USB_OTG_CHAGRE_ON:
        if(isUsbOtgOn){
            logVerbose(USBM,"usb otg already on");
            isOperateSuccessed = true;
        }else{
            if(operate(op)){
                isUsbOtgOn = true;
                isOperateSuccessed = true;
            }else{
                isOperateSuccessed = false;
            }
        }
        break;
    case E_USB_HOST_CHARGE_ON:
        if(isUsbHostOn){
            logVerbose(USBM,"usb host already on");
            isOperateSuccessed = true;
        }else{
            if(operate(op)){
                isUsbHostOn = true;
                isOperateSuccessed = true;
            }else{
                isOperateSuccessed = false;
            }
        }
        break;
    case E_USB_OTG_CHARGE_OFF:
        if(!isUsbOtgOn){
            logVerbose(USBM,"usb otg already off");
            isOperateSuccessed = true;
        }else{
            if(operate(op)){
                isUsbOtgOn = false;
                isOperateSuccessed = true;
            }else{
                isOperateSuccessed = false;
            }
        }
        break;
    case E_USB_HOST_CHARGE_OFF:
        if(!isUsbHostOn){
            logVerbose(USBM,"usb host already off");
            isOperateSuccessed = true;
        }else{
            if(operate(op)){
                isUsbHostOn = false;
                isOperateSuccessed = true;
            }else{
                isOperateSuccessed = false;
            }
        }
        break;
    case E_USB_OTG_TURNTO_DCP:
        if(operate(op)){
            isOperateSuccessed = true;
        }else{
            isOperateSuccessed = false;
        }
        break;
    case E_USB_OTG_TURNTO_CDP:
        if(operate(op)){
            isOperateSuccessed = true;
        }else{
            isOperateSuccessed = false;
        }
        break;
    default:
        logError(USBM,"op is not defined!");
        break;
    }

    logVerbose(USBM,"USBManager::UsbOperate -->OUT");
    return isOperateSuccessed;
}

bool USBManager::operate(ama_USBOperate op)
{
    logVerbose(USBM,"USBManager::operate -->IN");
    logInfoF(USBM,"USBManager::operate op:%04x",op);
    if (m_fileHandle < 0){
        logErrorF(USBM,"USBManager::operate open usb_fault file ERROR!!!");
        logVerbose(USBM,"USBManager::operate -->OUT");
        return false;
    }else{
        int retVal = ioctl(m_fileHandle,op,0);
        if (retVal==-1){
            logErrorF(USBM,"USBManager::operate ioctl return -1 check your CMD!!!");
            logVerbose(USBM,"USBManager::operate -->OUT return false");
            return false;
        }
    }
    logInfoF(USBM,"USBManager::operate SUCCESS");
    logVerbose(USBM,"USBManager::operate -->OUT");
    return true;
}

USBManager *USBManager::GetInstance()
{
    // the initialization of local static variable is not thread-safe
    static std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    static USBManager m_Instance;
    return &m_Instance;
}
