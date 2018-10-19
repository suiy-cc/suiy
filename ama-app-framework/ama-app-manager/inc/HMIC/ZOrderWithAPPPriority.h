/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ZOrderWithAPPPriority.h
/// @brief contains class ZOrderWithAPPPriority
///
/// Created by zs on 2016/6/30
/// this file contains the definition of class ZOrderWithAPPPriority
///

#ifndef ZORDERWITHAPPPRIORITY_H
#define ZORDERWITHAPPPRIORITY_H

#include <map>

#include "ZOrder.h"
#include "Layer.h"

class ZOrderWithAPPPriority
: public ZOrder
{
public:
    /// @name constructors & destroyers
    /// @{
    ZOrderWithAPPPriority();
    virtual ~ZOrderWithAPPPriority();
    /// @}

    /// @name operations
    /// @{
    virtual AMPID GetTopApp()const;;
    virtual void SetTopApp(AMPID);
    virtual void RemoveApp(AMPID);
    virtual bool IsThere(AMPID)const;
    virtual unsigned int GetSize()const;
    virtual void Clear();
    virtual void Print(bool upSideDown = false);
    /// @}

private:
    typedef E_LAYER Priority;
    std::map<Priority, ZOrder> zorder;
};


#endif // ZORDERWITHAPPPRIORITY_H
