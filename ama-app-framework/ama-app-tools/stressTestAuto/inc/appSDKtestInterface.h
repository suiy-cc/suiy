/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "tools.h"
#include "appSDKTestCase.h"
#include "audioAPITestCase.h"
#include "PLCAPITestCase.h"
#include "otherAPITestCase.h"
#include "powerAPITestCase.h"

extern "C" {
int getAppMgrPid_t(void);
void loadXXXModuleTestCase_t(int moduleDef);
int getAppSDKTestCaseNum_t(void);
bool runAppSDKTestCaseByIndex_t(int index);
}
