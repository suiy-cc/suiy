/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppUI.h
/// @brief contains UI APIs
///
/// Created by zs on 2016/10/31.
/// this file contains the declaration of UI APIs
///

#ifndef APPUI_H
#define APPUI_H

/// @name window section control
/// @{
bool GetDestinationRectangle(int &x, int &y, unsigned int &width, unsigned int &height);
void SetDestinationRectangle(const int& x, const int& y, const unsigned int& width, const unsigned int& height);
bool GetSourceRectangle(int &x, int &y, unsigned int &width, unsigned int &height);
void SetSourceRectangle(const int& x, const int& y, const unsigned int& width, const unsigned int& height);
void SetBothRectangle(const int& x, const int& y, const unsigned int& width, const unsigned int& height);
void SetUnderLayerCenter(bool isCenter = true);
bool IsUnderLayerCenter();
/// @}

/// @name titlebar visibility control
/// @{
void ShowTitlebar(int style = 0);
void HideTitlebar(int style = 0);
bool IsTitlebarVisible();
/// @}

/// @name titlebar drop-down state control
/// @{
/// @param isDropDown : true if you mean
/// to drop down current titlebar; false
/// if you are to withdraw current titlebar
/// @param style : speed of drop-down action.
/// but the value's actual meaning is up
/// to titlebar APP.
void DropDownTitlebar(bool isDropDown = true, int style = 0);

// NOTE:
// the visibility of a titlebar has nothing
// to do with that if the titlebar is drop-
// down. they are irrelevant.
// it means you can write code to command a
// titlebar to drop down when it's invisible.
bool IsTitlebarDropDown();
/// @}

/// @name touch control
/// @{
void EnableTouch(bool enable = true);
/// @}

/// @name seat info
/// @{
std::vector<E_SEAT> GetScreens();
void GetScreenSize(E_SEAT seat, unsigned int& width, unsigned int& height);
/// @}

/// @name Z-order control
/// @{

// if you hide current app (except for
// home), another app will show.
// once you invoke this function, current
// app will not be shown by other app's
// hiding.
void DontPopMe();

// similar to "void DontPopMe()", but you
// can specify the app with its AMPID
void DontPopApp(unsigned int app);

// similar to "void DontPopMe()", but you
// can specify the app with its item-ID
void DontPopApp(const std::string& itemID);
/// @}

/// @name extra surface control
/// @{

// add extra surface of current app.
// and specify the layer of the surface.
// the surface will be shown as soon
// as it's avaiable.
void AddExtraSurface(unsigned int surface, unsigned int layer);
/// @}

#endif // APPUI_H
