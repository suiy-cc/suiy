/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ZOrder.h
/// @brief contains class ZOrder
///
/// Created by zs on 2016/6/30
/// this file contains the definition of class ZOrder
///

#ifndef ZORDER_H
#define ZORDER_H

#include <list>

#include "ama_types.h"

class ZOrder
{
public:
    /// @name constructors & destroyers
    /// @{
    ZOrder();
    virtual ~ZOrder();
    /// @}

    /// @name operations
    /// @{
    virtual AMPID GetTopApp()const;
    virtual void SetTopApp(AMPID);
    virtual void RemoveApp(AMPID);
    virtual bool IsThere(AMPID)const;
    virtual unsigned int GetSize()const;
    virtual void Clear();
    virtual void Print(bool upSideDown = false);
    /// @}

private:
    std::list<AMPID> apps;
};


#endif // ZORDER_H
