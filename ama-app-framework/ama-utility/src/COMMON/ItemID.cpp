/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ItemID.cpp
/// @brief contains 
/// 
/// Created by zs on 2018/01/05.
///

#include "ItemID.h"
#include "ama_types.h"
#include "COMMON/StringOperation.h"

bool IsValidItemID(const std::string& itemID)
{
    auto domains = split(itemID, ".");
    if(domains.size()==4)
    {
        return ScreenID2Seat.find(domains[3]) != ScreenID2Seat.end()
               && !domains[0].empty()
               && !domains[1].empty()
               && !domains[2].empty();
    }
    else
        return false;
}