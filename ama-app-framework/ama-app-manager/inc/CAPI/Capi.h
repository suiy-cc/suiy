/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/* Copyright (C) 2015 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CAPI_H
#define CAPI_H

#include <vector>
#include "popup.h"
#include "AppPKGInfo.h"
#include "Capi_StubImpl.h"
#include "ama_stateTypes.h"
#include "CAPINotifyEnum.h"

// initialization
void InitializeCAPI();

void notify_OnChange(uint16_t statMgr_AMPID,std::string key,std::string keyValue);
void update_orgDate(const ::v0_1::org::neusoft::AppMgr::key_value& key_value);

void popupUpdateInfoList(const std::vector<std::pair<uint64_t, PopInfo>> & vp);

#endif // CAPI_H
