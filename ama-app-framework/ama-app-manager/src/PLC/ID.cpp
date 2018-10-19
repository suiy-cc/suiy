/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ID.cpp
/// @brief contains
///
/// Created by zs on 2017/05/23.
///

#include <map>
#include <mutex>
#include <fstream>

#include "ID.h"
#include "ama_types.h"
#include "AMGRLogContext.h"
#include "HandleManagement.h"
#include "COMMON/StringOperation.h"
#include "ItemID.h"



std::string GET_APPLOGID_FROM_AMPID(AMPID ampid)
{
    std::string appLogID;

    // some fake application is implemented by KANZI program.
    // they need their own log IDs too.
//    // for AMGR AMPID
//    if(IS_AMGR(ampid))
//        return "AMGR";

    // for anonymous process
    if(IS_ANONYMOUS(ampid))
    {
        char hex[3];
        sprintf(hex, "%02x", ampid&0xFF);
        return "AM" + std::string(hex);
    }

    static std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    static std::map<E_APPID, std::string> App2LogID;

    static bool isInitialized = false;
    static const std::string path = "/etc/ama.d/LogID.txt";

    if(!isInitialized)
    {
        std::ifstream configFile(path.c_str());
        if(configFile.is_open())
        {
            while(configFile.peek() != EOF)
            {
                try
                {
                    std::string key, value;
                    configFile >> key >> value;
                    App2LogID.insert(std::make_pair(GetAppID(key), value));
                }
                catch(std::exception& exception)
                {
                    logWarn(PLC, "GET_APPLOGID_FROM_AMPID(): ", (&exception)->what());
                    continue;
                }
            }

            configFile.close();
        }
        isInitialized = true;
    }

    // get log id from AMPID
    bool isFound = true;
    if(App2LogID.find(GET_APPID(ampid))==App2LogID.end())
    {
        appLogID=ToHEX(ampid,false);
        isFound = false;
    }
    else
        appLogID = App2LogID[GET_APPID(ampid)];

    if(isFound)
    {
        switch(GET_SEAT(ampid))
        {
            case E_SEAT_IVI:  appLogID += "1";break;
            case E_SEAT_RSE1: appLogID += "2";break;
            case E_SEAT_RSE2: appLogID += "3";break;
//            case E_SEAT_SHARE:appLogID += "4";break;
            case E_SEAT_TEST: appLogID += "T";break;
            case E_SEAT_NONE: appLogID += "S";break;
            default:break;
        }
    }

    return appLogID;
}

char digits[]=
         {
             '0', '1', '2', '3', '4', '5',
             '6', '7', '8', '9', 'a', 'b',
             'c', 'd', 'e', 'f', 'g', 'h',
             'i', 'j', 'k', 'l', 'm', 'n',
             'o', 'p', 'q', 'r', 's', 't',
             'u', 'v', 'w', 'x', 'y', 'z'
         };

unsigned int GetSurfaceID(AMPID app)
{
    E_APPID appid = GET_APPID(app);
    E_SEAT seat = GET_SEAT(app);
    E_LAYER layer = GetLayer(appid);

    unsigned int surfaceid=(SURFACEID(app, LAYERID(seat, layer)));
    if(GetAppID("titlebar")==appid)
        logWarn("titlebar's surface id = ", ToHEX(surfaceid));
    return surfaceid;
}

E_APPID E_APPID_HOME = 0x01;
E_APPID E_APPID_TITLEBAR = 0x02;
E_APPID E_APPID_POPUP = 0x03;
E_APPID E_APPID_MUSIC = 0x04;
E_APPID E_APPID_VIDEO = 0x05;
E_APPID E_APPID_IMAGE = 0x06;
E_APPID E_APPID_RADIO = 0x07;
E_APPID E_APPID_IME = 0x09;
E_APPID E_APPID_BKG = 0x13;
E_APPID E_APPID_AP = 0x15;
E_APPID E_APPID_SYNC_APP = 0x1A;
E_APPID E_APPID_TOUCH_FORBIDDER = 0x1F;
E_APPID E_APPID_GR = 0x20;
E_APPID E_APPID_MASK_APPS = 0x21;
E_APPID E_APPID_CHILD_LOCK = 0x2B;

static std::map<std::string, E_APPID*> systemAPPIDMap = {
    {"home", &E_APPID_HOME},
    {"titlebar", &E_APPID_TITLEBAR},
    {"popup", &E_APPID_POPUP},
    {"radio", &E_APPID_RADIO},
    {"ime", &E_APPID_IME},
    {"background", &E_APPID_BKG},
    {"animation-player", &E_APPID_AP},
    {"sync", &E_APPID_SYNC_APP},
    {"touch-forbidder", &E_APPID_TOUCH_FORBIDDER},
    {"gesture-recognizer", &E_APPID_GR},
};

E_APPID GetAppID(const std::string& appid)
{
    static std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    typedef std::map<std::string, E_APPID> APPIDMAP;
    static APPIDMAP appIDMap;
    static HandleManager<int> appIDManager;

    static bool isInitialized = false;
    static const std::string path = "/etc/ama.d/AppID.txt";

    if(!isInitialized)
    {
        std::ifstream configFile(path.c_str());
        if(configFile.is_open())
        {
            while(configFile.peek() != EOF)
            {
                try
                {
                    std::string key, value;
                    configFile >> key >> value;
                    if(key.empty() && value.empty())
                        break;
                    E_APPID APPID = std::stoul(value, 0, 0);
                    appIDMap.insert(std::make_pair(key, APPID));
                    appIDManager.CreateHandle(APPID);

                    // update system app's appid
                    if(systemAPPIDMap.find(key)!=systemAPPIDMap.end())
                        *systemAPPIDMap[key] = APPID;
                }
                catch(std::exception& exception)
                {
                    logWarn(PLC, "GetAppID(): ", (&exception)->what());
                    continue;
                }
            }

            configFile.close();
        }
        isInitialized = true;
    }

    if(appIDMap.find(appid)==appIDMap.end())
    {
        E_APPID APPID = appIDManager.GetMinAvailableHandle();
        appIDMap.insert(std::make_pair(appid, APPID));
        return APPID;
    }
    else
        return appIDMap[appid];
}

E_LAYER GetLayer(E_APPID appid)
{
    static std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    static std::map<E_APPID, E_LAYER> App2Layer;

    static bool isInitialized = false;
    static const std::string path = "/etc/ama.d/LayerID.txt";

    if(!isInitialized)
    {
        std::ifstream configFile(path.c_str());
        if(configFile.is_open())
        {
            while(configFile.peek() != EOF)
            {
                try
                {
                    std::string key, value;
                    configFile >> key >> value;
                    if(key.empty() && value.empty())
                        break;
                    E_LAYER layer = static_cast<E_LAYER>(std::stoul(value, 0, 0));
                    E_APPID appID = GetAppID(key);
                    App2Layer.insert(std::make_pair(appID, layer));
                }
                catch(std::exception& exception)
                {
                    logWarn(PLC, "GetLayer(): ", (&exception)->what());
                    continue;
                }
            }

            configFile.close();
        }
        isInitialized = true;
    }


    if(App2Layer.find(appid)==App2Layer.end())
    {
        if(GetAppID("titlebar")==appid)
            logWarn("titlebar's record is not found! appid = ", appid);
        return E_LAYER_NORMALAPP;
    }
    else
        return App2Layer.at(appid);
}

std::string GetAppIDStr(const std::string& itemID)
{
    if(IsValidItemID(itemID))
    {
        return split(itemID, ".")[2];
    }
    else
        return std::string();
}

unsigned int GetScreenID(const std::string& itemID)
{
    if(IsValidItemID(itemID))
    {
        auto screen = split(itemID, ".")[3];
            return static_cast<unsigned int>(ScreenID2Seat.at(screen));
    }
    else
        return static_cast<unsigned int>(E_SEAT_INVALID);
}
