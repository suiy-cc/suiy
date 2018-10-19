/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TestAPI.cpp
/// @brief contains APIs for all kinds of test
/// 
/// Created by zs on 2017/09/14.
///

#include "TestAPI.h"
#include "AppManager.h"

void Mock(AMPID app)
{
    AppManager::GetInstance()->SetAMPID(app);
}

void Mock(const std::string& itemID)
{
    AMPID app = AppManager::GetInstance()->GetAMPID(itemID);
    Mock(app);
}
