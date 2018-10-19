/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <iostream>
#include <memory>
using std::cout;
#include <AppSDK.h>
#include <log.h>
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <ilm_types.h>

#include "launcher.h"

LogContext LC;

static bool quit = false;
unsigned int gSurfaceID = 0;

class CUIPLCEx : public AppPLC
{
public:
    virtual void OnCreate()
    {
        logDebug("CUIPLCEx::OnCreate\n");

        // your code ...
    }

    virtual void OnPreShow(long nArgs /*= -1*/)
    {
        logDebug("CUIPLCEx::OnPrevShow\n");

        // your code ...
    }

    virtual void OnShow()
    {
        logDebug("CUIPLCEx::OnShow\n");

        // your code ...
    }

    virtual void OnResume()
    {
        logDebug("CUIPLCEx::OnResume\n");

        // your code ...
    }

    virtual void OnHide()
    {
        logDebug("CUIPLCEx::OnHide\n");

        // your code ...
    }

    virtual void OnStop()
    {
        logDebug("CUIPLCEx::OnStop\n");

        // your code ...
        quit = true;
        sleep(1);
    }

    virtual bool ShowStartupScreen()const
    {
        return gSurfaceID!=12289;
    }
};

int getPidOf(char *procName)
{
    char cmd [50];
    int pid = 0;

    snprintf(cmd, sizeof(cmd), "/bin/pidof %s", procName);

    //printf("cmd=%s",cmd);

    FILE *fp = popen(cmd, "r");//打开管道，执行shell 命令
    char buffer[10] = {0};
    while (NULL != fgets(buffer, 10, fp)) //逐行读取执行结果并打印
    {
        //printf("PID:  %s", buffer);
        pid = atol(buffer);
    }
    pclose(fp);

    //printf("pid=%d",pid);
    return pid;
}

int main(int argc, char **argv)
{
    LogInitialize("AUID", true);
	for (int i = 0; i < argc; i++)
	{
		printf("running param(%d): %s\n", i, argv[i]);
	}

    // initialize SDK
    AppSDKInit();

	long nSurfaceID = -1;
	if (argc >= 2)
		nSurfaceID = GetSurfaceID();

    // log test
    RegisterContext(LC, "LC");
    logInfo("this is Info log.");
    logVerbose("this is Verbose log.");
    logWarn("this is Warn log.");
    logDebug("this is Debug log.");
    logError("this is Error log.");
    logInfo(LC, "this is a LC Info log.");
    logVerbose(LC, "this is a LC Verbose log.");
    logWarn(LC, "this is a LC Warn log.");
    logDebug(LC, "this is a LC Debug log.");
    logError(LC, "this is a LC Error log.");
    logInfo("this is Info log.");

    logInfo("surface id=", nSurfaceID);

	auto pUI = std::make_shared<CUIPLCEx>();

    gSurfaceID = nSurfaceID;

    logInfo("pUI->Initialize");
    assert(pUI->Initialize(nSurfaceID));

    logInfo("createMainwindow(0, 800, 480)");
    createMainwindow(0, 800, 480);

    // t_ilm_surface surfaceIDs[] = {nSurfaceID};
    // ilm_setInputFocus(surfaceIDs, ARRAY_LENGTH(surfaceIDs), ILM_INPUT_DEVICE_POINTER|ILM_INPUT_DEVICE_TOUCH, ILM_TRUE);

    logInfo("create surface over");

    while(!quit) {sleep(1);}

    releaseMainwindow();

    return 0;
}
