/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
//
// Created by zs on 16-12-30.
//

#include "COMMON/DomainInvoker.h"

DomainInvoker::DomainInvoker(std::function<void(void)> i, std::function<void(void)> d)
: init(i)
, destroy(d)
, isDestroyed(false)
{
    init();
}

DomainInvoker::~DomainInvoker()
{
    if(!isDestroyed)
    {
        destroy();
        isDestroyed = true;
    }
}

void DomainInvoker::InvokeDestroyFunction()
{
    if(!isDestroyed)
    {
        destroy();
        isDestroyed = true;
    }
}