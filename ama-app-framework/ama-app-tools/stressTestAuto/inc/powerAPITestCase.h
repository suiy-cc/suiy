/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef POWERAPITESTCASE_H
#define POWERAPITESTCASE_H

void loadPowerAPITestCase(void);

bool testCase_checkIsPowerOn(void);
bool testCase_checkIsACCOn(void);
bool testCase_getPowerState(void);
bool testCase_requestPowerGoodbye(void);

#endif//POWERAPITESTCASE_H
