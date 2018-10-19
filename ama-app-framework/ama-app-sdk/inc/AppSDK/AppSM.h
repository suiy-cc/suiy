/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppSM.h
/// @brief contains State Manager's API
///
/// Created by zs on 2016/8/4.
/// this file contains the declaration of State Manager's APIs
///

#ifndef APPSM_H
#define APPSM_H

#include <string>
#include <map>

using namespace std;

typedef void(* SM_handleReqSet)(uint16_t ampid_req, std::string key, std::string value);
typedef void(* SM_handleStateChange)(uint16_t ampid_req, uint16_t ampid_reply, std::string  key,  std::string  value);

//extern SM_handleReqSet setReqCallback;

bool SM_isStateExist(std::string state);//for app&server return:true exist false none
std::string SM_getState(std::string state);//for app&server  return:the value of state if none return "null"
void SM_req2SetState(std::string state,std::string value);//for app   set state req anyone who want to change the value of state
void SM_regReply4Req(SM_handleReqSet callback);//for server   provider who set callback when someone want to change the key of state note:only ampid_reply of state have the opportunity
void SM_updateState(uint16_t ampid_req, string state,string value);//for server   provider who need to notify when change occor
void SM_regCb4StateChanged(SM_handleStateChange callback);//for app   set callback when changes occored

#endif // APPSM_H
