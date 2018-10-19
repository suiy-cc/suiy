/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppShareManager.h
/// @brief contains class AppShareManager
///
/// Created by zs on 2016/12/19
/// this file contains the definition of class AppShareManager
///

#ifndef APPSHAREMANAGER_H
#define APPSHAREMANAGER_H

#include <map>

#include "ama_types.h"
#include "AppShareContext.h"
#include "Surface.h"
#include "Layer.h"
#include "ThreadSafeValueKeeper.h"

typedef std::vector<shared_ptr<AppShareContext>> ContextPtrs;

class AppShareManager
{
public:
    virtual ~AppShareManager();

    static AppShareManager* GetInstance();

    void AddContext(AMPID app, shared_ptr<AppShareContext> context);
    void RemoveContext(AMPID app);
    std::vector<Surface> GetSourceSurfaces(AMPID app);
    std::vector<Layer> GetWorkingLayers(AMPID app);
    shared_ptr<AppShareContext> GetContext(AMPID app);
    ContextPtrs GetContext(std::function<bool(shared_ptr<AppShareContext>)> filter);

    static void OnSourceSurfaceDestroyed(const Surface& surface);

private:
    AppShareManager();

    typedef std::map<AMPID, std::shared_ptr<AppShareContext> > ContextMap;
    ThreadSafeValueKeeper<ContextMap> contexts;
};


#endif // APPSHAREMANAGER_H
