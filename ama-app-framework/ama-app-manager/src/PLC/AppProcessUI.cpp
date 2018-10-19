/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "AppProcessUI.h"


char *AppProcessUI::appLargeIcon() const
{
    return m_appLargeIcon;
}

void AppProcessUI::setAppLargeIcon(char *appLargeIcon)
{
    m_appLargeIcon = appLargeIcon;
}

char *AppProcessUI::appSmallIcon() const
{
    return m_appSmallIcon;
}

void AppProcessUI::setAppSmallIcon(char *appSmallIcon)
{
    m_appSmallIcon = appSmallIcon;
}

char **AppProcessUI::getAgrv() const
{
    return agrv;
}

void AppProcessUI::setAgrv(char **value)
{
    agrv = value;
}
