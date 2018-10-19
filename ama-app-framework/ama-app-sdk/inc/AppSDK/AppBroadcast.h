/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef APPBROADCAST_H
#define APPBROADCAST_H

#include <string>

using namespace std;

typedef void (*handleBroadcastFun)(std::string event,std::string data);

void sendBroadcast(std::string event,std::string data);
void listenBroadcast(handleBroadcastFun handler);

void sendBroadcastForAutoTest(std::string event,std::string data);

#endif //APPBROADCAST_H
