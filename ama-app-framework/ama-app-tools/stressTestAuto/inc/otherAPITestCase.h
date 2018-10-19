/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef SETAPITESTCASE_H
#define SETAPITESTCASE_H

void loadOtherAPITestCase(void);
void loadOnlyPopupAPITestCase(void);

bool testCase_setByAppManager(void);
bool testCase_AMGR_getState(void);
bool testCase_imeOpenIME(void);
bool testCase_imeCloseIME(void);
bool testCase_psShowAllTypePopup(void);
bool testCase_psShowPopup(void);
bool testCase_psClosePopup(void);
bool testCase_GetDestinationRectangle();
bool testCase_SetDestinationRectangle();
bool testCase_GetSourceRectangle();
bool testCase_SetSourceRectangle();
bool testCase_SetBothRectangle();
bool testCase_SetUnderLayerCenter(void);
bool testCase_IsUnderLayerCenter(void);
bool testCase_ShowTitlebar(void);
bool testCase_HideTitlebar(void);
bool testCase_IsTitlebarVisible(void);
bool testCase_EnableTouch(void);

bool testCase_SM_getState(void);
bool testCase_SM_req2SetState(void);
bool testCase_SM_updateState(void);

void createStateMapForTestCase(void);

// bool SM_isStateExist(std::string state);//for app&server return:true exist false none
// std::string SM_getState(std::string state);//for app&server  return:the value of state if none return "null"
// void SM_req2SetState(std::string state,std::string value);//for app   set state req anyone who want to change the value of state
// void SM_regReply4Req(SM_handleReqSet callback);//for server   provider who set callback when someone want to change the key of state note:only ampid_reply of state have the opportunity
// void SM_updateState(uint16_t ampid_req, string state,string value);//for server   provider who need to notify when change occor
// void SM_regCb4StateChanged(SM_handleStateChange callback);//for app   set callback when changes occored


#endif//SETAPITESTCASE_H
