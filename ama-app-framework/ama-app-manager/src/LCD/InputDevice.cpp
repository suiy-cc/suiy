/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "InputDevice.h"

#include <string.h>
#define BUFFER_SIZE 1024
InputDevice::InputDevice(const char* DevPath)
{
    logVerbose(LCDM,"InputDevice::InputDevice()--->IN DevPath:",std::string(DevPath));
    memcpy(m_DevPath,DevPath,strlen(DevPath));
    init();
    logVerbose(LCDM,"InputDevice::InputDevice()--->OUT ");
}
InputDevice::~InputDevice()
{
    deInit();
}
void InputDevice::init()
{
    logVerbose(LCDM,"InputDevice::Init()--->IN ");

    if(NULL==m_DevPath)
    {
        logError(LCDM,"InputDevice::INIT m_DevPath:",(m_DevPath) ," PATH IS NULL!");
        return;
    }
    inputFile = open(m_DevPath , O_RDONLY|O_NONBLOCK);
    if(inputFile<=0)
    {
        logErrorF(LCDM,"open  %s device error!",m_DevPath);
        close(inputFile);
    }
    else
    {
        logInfo(LCDM,"open  ",std::string(m_DevPath) ," device success!!");

    }
    logVerbose(LCDM,"InputDevice::Init()--->OUT ");

}
void InputDevice::deInit()
{
    if (inputFile!=0)
        close(inputFile);

    inputFile=0;
}
bool InputDevice::checkPress()
{
    //logVerbose(LCDM,"InputDevice::checkPress--> IN!");
    char buff [BUFFER_SIZE]={0};
    if (!m_DevPath){
         logError(LCDM,"checkPress  ",m_DevPath ," device null!");
        return false;
    }
    int remaimData= read(inputFile,&buff,BUFFER_SIZE);
    //考虑增加一次没有读完的情况。
    //logVerbose(LCDM,"InputDevice::checkPress--> OUT! remaimData:",remaimData);
    if (remaimData>0)
        return true;
    else
        return false;

}
