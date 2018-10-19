/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Configuration.cpp
/// @brief contains configurations of ama-app-manager
///
/// Created by zs on 2017/05/25.
///

#include "Configuration.h"
#include "ama_types.h"
#include "ReadXmlConfig.h"

bool IsEnableDLTLocalPrint()
{
    std::string key = "EnableDLTLocalPrint";
    std::string value;
    RXC_Error error = GetConfigFromString(key, value);
    if(error==Error_None)
    {
        return value!="0";
    }
    else
        return false;
}
int GetRearTimerToCloseLCD()
{
    std::string key = "RearTimerToCloseLCD";
    std::string value;
    RXC_Error error = GetConfigFromString(key, value);
    if(error==Error_None)
    {
        int retVal=atol(value.c_str());
        return retVal;
    }
    else
        return 30;
}
int GetMaxBackgroundProcess(int seatid)
{
    if (seatid>3||seatid<1)
        return 100;
    std::string key[3] = {"IVIMaxBackgroundProcess",
                          "RSELMaxBackgroundProcess",
                          "RSERMaxBackgroundProcess"
                         };
    std::string value;
    RXC_Error error = GetConfigFromString(key[seatid-1], value);
    if(error==Error_None)
    {
        int retVal=atol(value.c_str());
        return retVal;
    }
    else
        return 100;
}
int GetMinimumMemoryCanAccept()
{
    std::string key = "MinimumMemoryCanAccept";
    std::string value;
    RXC_Error error = GetConfigFromString(key, value);
    if(error==Error_None)
    {
        int retVal=atol(value.c_str());
        return retVal;
    }
    else
        return 10;
}
bool IsFastBootEnabled()
{
    std::string key = "EnableFastBoot";
    std::string value;
    RXC_Error error = GetConfigFromString(key, value);
    if(error==Error_None)
    {
        return value!="0";
    }
    else
        return true; // home fast boot is valid by default
}

int GetAppBootTimeoutInSec()
{
    const int defValue = 15;
    std::string key = "AppBootTimeout";
    std::string value;
    RXC_Error error = GetConfigFromString(key, value);
    if(error==Error_None)
    {
        int second = 0;
        try
        {
            second = std::stol(value, 0, 0);
        }
        catch(std::exception& e)
        {
            second = defValue;
        }
        return second;
    }
    else
        return defValue;
}
bool IsRearAlwaysON()
{
    std::string key = "IsRearAlwaysON";
    std::string value;
    RXC_Error error = GetConfigFromString(key, value);
    if(error==Error_None)
    {
        return value!="0";
    }
    else
        return false;
}

bool IsScreenDisabled(unsigned int seat)
{
    std::string key;
    switch(seat)
    {
        case E_SEAT_IVI: key = "DisableIVI";break;
        case E_SEAT_RSE1: key = "DisableRSE1";break;
        case E_SEAT_RSE2: key = "DisableRSE2";break;
        default: key = "";
    }
    std::string value;
    RXC_Error error = GetConfigFromString(key, value);
    if(error==Error_None)
    {
        return value!="0";
    }
    else
        return false;
}

bool IsAutoStartDisabled()
{
    std::string key = "DisableAutoStart";
    std::string value;
    RXC_Error error = GetConfigFromString(key, value);
    if(error==Error_None)
    {
        return value!="0";
    }
    else
        return false;
}

bool IsSysUIMemOptmz()
{
    std::string key = "SysUIMemOptmz";
    std::string value;
    RXC_Error error = GetConfigFromString(key, value);
    if(error==Error_None)
    {
        return value!="0";
    }
    else
        return true;
}

bool IsLastSourceEnabled()
{
    std::string key = "EnableLastSource";
    std::string value;
    RXC_Error error = GetConfigFromString(key, value);
    if(error==Error_None)
    {
        return value!="0";
    }
    else
        return true;
}