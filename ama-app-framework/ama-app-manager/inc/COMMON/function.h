/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#define DEF_RECORD_DIR_FOR_APPMANAGER_RUN_STA  "/media/ivi-data/ama.d/appManagerComplieTimeRecord"

static bool isTerminateSignalReceived = false;

void checkIsAppManagerFirstRuningInThisMachineAndRecordIt(void);
bool isAppMangerFirstRuningInThisMachine(void);
void setDltStorgeConfig(void);
void setSingalHandler(void);
void cameraSignalInit(void);
void cameraSignalDeinit(void);
void OutOfMemoryHandler(void);
