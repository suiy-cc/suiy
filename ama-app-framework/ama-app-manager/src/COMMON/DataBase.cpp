/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file DataBase.cpp
/// @brief contains 
/// 
/// Created by zs on 2018/06/21.
///

#include "DataBase.h"
#include "AMGRLogContext.h"

typedef std::map<std::string, std::string> DBMap;

static const DBMap AMGRDBDefaultValue = {
    {AMGRDB_KEY_IS_RSE_ENABLED, "0"}, // disabled by default
};

AppDatabase AMGRDB("ama-app-manager", AMGRDBDefaultValue);

bool IsRSEEnabled()
{
    FUNCTION_SCOPE_LOG_C(DB);

    if(!AMGRDB.isKeyExist(AMGRDB_KEY_IS_RSE_ENABLED))
        return false;

    std::string defValue;
    std::string value;
    AMGRDB.getValueOfKey(AMGRDB_KEY_IS_RSE_ENABLED, value);
    AMGRDB.getDefValueOfKey(AMGRDB_KEY_IS_RSE_ENABLED, defValue);

    logInfo(DB, LOG_HEAD, "value = ", value);
    return value!=defValue;
}

void EnableRSE(bool enable)
{
    FUNCTION_SCOPE_LOG_C(DB);

    if(!AMGRDB.isKeyExist(AMGRDB_KEY_IS_RSE_ENABLED))
    {
        logError(DB, LOG_HEAD, "key ", AMGRDB_KEY_IS_RSE_ENABLED, " is not existing!");
        return;
    }

    logDebug(DB, LOG_HEAD, "enable = ", enable?"true":"false");
    AMGRDB.setValueOfKey(AMGRDB_KEY_IS_RSE_ENABLED, enable?"1":"0");
}