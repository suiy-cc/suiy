/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file SplashLOGO.cpp
/// @brief contains
///
/// Created by zs on 2017/02/06.
/// this file contains the implementation of class ActiveAppHome
///

#include "SplashLOGO.h"
#include "log.h"
#include "AMGRLogContext.h"
#include "ID.h"

#include <fstream>

const std::string SplashLOGO::fileName = "/dev/splash-logo";

SplashLOGO::SplashLOGO()
{}

SplashLOGO::~SplashLOGO()
{

}

SplashLOGO *SplashLOGO::GetInstance()
{
    static SplashLOGO instance;

    return &instance;
}

void SplashLOGO::SetStateFile(const std::string &value)
{
    DomainVerboseLog chatter(PLC, formatText("SplashLOGO::SetStateFile(%s)", value.c_str()));

    std::ofstream ofs;

    ofs.open(fileName.c_str());
    if(ofs.is_open())
    {
        ofs << value;
        ofs.close();
    }
}

std::string SplashLOGO::GetStateFile() const
{
    DomainVerboseLog chatter(PLC, "SplashLOGO::GetStateFile()");

    std::ifstream ifs;

    ifs.open(fileName.c_str());
    if(ifs.is_open())
    {
        std::string content;

        std::ostringstream temp;
        temp<<ifs.rdbuf();
        content=temp.str();

        ifs.close();

        logInfo(PLC, "SplashLOGO::GetStateFile(): file content: ", content);
        return std::move(content);
    }

    return std::string();
}

void SplashLOGO::NotifyHomeReady(AMPID home)
{
    // DomainVerboseLog chatter(PLC, formatText("SplashLOGO::NotifyHomeReady(%#x)", home));
    //
    // if(GET_APPID(home)==E_APPID_HOME && GET_SEAT(home)==E_SEAT_IVI && GetStateFile()!="0")
    //     SetStateFile(std::string("0"));
}
