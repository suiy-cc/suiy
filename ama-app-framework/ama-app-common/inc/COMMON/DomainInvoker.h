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

#ifndef DOMAININVOKER_H
#define DOMAININVOKER_H

#include <functional>

class DomainInvoker
{
public:
    DomainInvoker(std::function<void(void)> i, std::function<void(void)> d);
    virtual ~DomainInvoker();

    void InvokeDestroyFunction();

private:
    std::function<void(void)> init;
    std::function<void(void)> destroy;
    bool isDestroyed;
};


#endif // DOMAININVOKER_H
