/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "ama_enum.h"
#include "App.h"

App::App()
: m_amaPid(INVALID_AMPID)
, m_itemID()
, m_appPath()
, m_appName()
, m_bIcon()
, m_sIcon()
, m_systemdFile()
, m_appType()
, m_isAutoStart(false)
, m_isStartupAnimationDisabled(false)
, m_target()
{}

App::~App(){}

App::App(const App& app)
{
    *this = app;
}

App::App(App&& app)
{
    *this = app;
}

void App::operator=(App&& app)
{
    m_amaPid = std::move(app.m_amaPid);
    m_itemID = std::move(app.m_itemID);
    m_appPath = std::move(app.m_appPath);
    m_appName = std::move(app.m_appName);
    m_bIcon = std::move(app.m_bIcon);
    m_sIcon = std::move(app.m_sIcon);
    m_systemdFile = std::move(app.m_systemdFile);
    m_appType = std::move(app.m_appType);
    m_isAutoStart = std::move(app.m_isAutoStart);
    m_isStartupAnimationDisabled = std::move(app.m_isStartupAnimationDisabled);
    m_target = std::move(app.m_target);
}

void App::operator=(const App& app)
{
    m_amaPid = app.m_amaPid;
    m_itemID = app.m_itemID;
    m_appPath = app.m_appPath;
    m_appName = app.m_appName;
    m_bIcon = app.m_bIcon;
    m_sIcon = app.m_sIcon;
    m_systemdFile = app.m_systemdFile;
    m_appType = app.m_appType;
    m_isAutoStart = app.m_isAutoStart;
    m_isStartupAnimationDisabled = app.m_isStartupAnimationDisabled;
    m_target = app.m_target;
}

unsigned int App::getAMPID() const
{
    return m_amaPid;
}

void App::setAMPID(unsigned int amaPid)
{
    m_amaPid = amaPid;
}

std::string App::getItemID() const
{
    return m_itemID;
}

void App::setItemID(const std::string& itemID)
{
    m_itemID = itemID;
}

std::string App::getAppPath() const
{
    return m_appPath;
}

void App::setAppPath(const std::string& appPath)
{
    m_appPath = appPath;
}

std::string App::getAppName() const
{
    return m_appName;
}

void App::setAppName(const std::string& appName)
{
    m_appName = appName;

}

//std::string App::getPkgID()
//{
//    return m_pkgID;
//}
//
//void App::setPkgID(const std::string& pkgID)
//{
//    m_pkgID = pkgID;
//}

std::string App::getAppType()const
{
    return m_appType;
}
void App::setAppType(const std::string& appType)
{
    m_appType = appType;
}

bool App::getIsAutoStart() const
{
    return m_isAutoStart;
}

void App::setIsAutoStart(bool isAutoStart)
{
    m_isAutoStart = isAutoStart;
}

bool App::getDisableStartupAnimation() const
{
    return m_isStartupAnimationDisabled;
}

void App::setDisableStartupAnimation(bool disable)
{
    m_isStartupAnimationDisabled = disable;
}

std::string App::getBigIcon() const
{
    return m_bIcon;
}
void App::setBigIcon(const std::string& bIcon)
{
    m_bIcon = bIcon;
}


std::string App::getSmallIcon() const
{
    return m_sIcon;
}
void App::setSmallIcon(const std::string& sIcon)
{
    m_sIcon = sIcon;
}

std::string App::getSystemdFile() const
{
    return m_systemdFile;
}

void App::setSystemdFile(const std::string& systemdFile)
{
    m_systemdFile = systemdFile;
}

std::string App::getTarget()const
{
    return m_target;
}

void App::setTarget(const std::string& target)
{
    m_target = target;
}
