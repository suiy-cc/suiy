/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file IntentInfo.h
/// @brief contains class IntentInfo
///
/// Created by zs on 2016/11/23.
/// this file contains the definition of class IntentInfo
///

#ifndef INTENTINFO_H
#define INTENTINFO_H

#include <string>

#include "cereal/access.hpp"
#include "cereal/cereal.hpp"

class IntentInfo{
public:
    std::string itemID;
    std::string action;
    std::string data;
    bool isQuiet;

public:
    IntentInfo()
        : itemID()
        , action()
        , data()
        , isQuiet(false)
    {}
    IntentInfo(const std::string &action, const std::string &data, bool isQuiet)
        : itemID()
        , action(action)
        , data(data)
        , isQuiet(isQuiet)
    {}
    IntentInfo(const std::string &itemID, const std::string &action, const std::string &data, bool isQuiet)
        : itemID(itemID)
        , action(action)
        , data(data)
        , isQuiet(isQuiet)
    {}
    IntentInfo(const IntentInfo& info){
        *this = info;
    }
    IntentInfo(IntentInfo&& info){
        *this = info;
    }

    IntentInfo& operator=(IntentInfo&& info)
    {
        itemID = std::move(info.itemID);
        action = std::move(info.action);
        data = std::move(info.data);
        isQuiet = std::move(info.isQuiet);

        return *this;
    }
    IntentInfo& operator=(const IntentInfo& info)
    {
        itemID = info.itemID;
        action = info.action;
        data = info.data;
        isQuiet = info.isQuiet;

        return *this;
    }
    bool operator==(const IntentInfo& info)const{
        return itemID==info.itemID
            && action==info.action
            && data == info.data
            && isQuiet == info.isQuiet;
    }

protected:
    // serialization support
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & sereal )
    {
        sereal(CEREAL_NVP(itemID)
               , CEREAL_NVP(action)
               , CEREAL_NVP(data)
               , CEREAL_NVP(isQuiet)
              );
    }
};

#endif // INTENTINFO_H
