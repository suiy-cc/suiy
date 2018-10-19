/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppSet.h
/// @brief contains API framework set
///
/// Created by wq on 2017/05/11.
/// this file contains API of framework set
///

#ifndef APPSET_H
#define APPSET_H

#include "ama_types.h"
#include <ama_setTypes.h>
#include <ama_stateTypes.h>

/**
* @brief set function by app manager
* @param [in]  void
* @return void.
*/
void setByAppManager(ama_setType_e key,int value);

void setVolumeByAppManager(ama_setType_e volumeType,int volume,bool isEnableFeedBack);
//get
std::string AMGR_getState(ama_stateType_e state_type);
//changed
typedef void(* AMGR_handleStateChange)(int state_type,  std::string  value);
void AMGR_regCb4StateChanged(AMGR_handleStateChange callback);

#endif //APPSET_H
