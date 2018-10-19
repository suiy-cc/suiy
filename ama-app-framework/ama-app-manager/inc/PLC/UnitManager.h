/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef UNITMANAGER_H
#define UNITMANAGER_H

#include <mutex>
#include <string>
#include <libLifecycle.hpp>

#include "AppList.h"
#include "ama_types.h"
#include "AMGRLogContext.h"

//CALLBACK FUNCTIONS
class UnitManager
{
public:
    UnitManager();
    ~UnitManager();

    void init();
	void destroy();

	bool StartUnit(std::string unitName, const char **argv = nullptr, void *userData = nullptr);
    std::string StartUnit(unsigned int ampid, const char **argv); // return empty string if anything is wrong.
    void StopUnit(AMPID app);
    void StopUnit(const std::string &unitName);
	bool GetUnitState(const std::string& unitName);
    bool GetUnitState(unsigned int proc);
    LLC_Result listenUnitState();

    Broadcaster<void(std::string, bool)> unitStateChangeBroadcaster;

    static UnitManager *GetInstance();

private:
    AMPID GetAMPID(const std::string& unitName);
    std::string GetUnitName(unsigned int proc);
	bool isInitialized;
};

#endif // UNITMANAGER_H
