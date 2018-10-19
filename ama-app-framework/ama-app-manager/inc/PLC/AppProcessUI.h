/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef APPPROCESSUI
#define APPPROCESSUI
#include "App.h"

class AppProcessUI : public App
{
public:
    AppProcessUI();
    ~AppProcessUI();

    char *appLargeIcon() const;
    void setAppLargeIcon(char *appLargeIcon);

    char *appSmallIcon() const;
    void setAppSmallIcon(char *appSmallIcon);

    char **getAgrv() const;
    void setAgrv(char **value);

private:
    char **agrv;
    char *m_appLargeIcon;
    char *m_appSmallIcon;
};


#endif // APPPROCESSUI


