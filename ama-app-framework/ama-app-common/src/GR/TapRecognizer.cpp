/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TapRecognizer.cpp
/// @brief contains 
/// 
/// Created by zs on 2017/03/01.
///

/// this file contains the implementation of class TapRecognizer
///

#include "GR/TapRecognizer.h"
#include "GR/TapState.h"

TapRecognizer::TapRecognizer()
: TouchRecognizer(std::make_shared<TapStateInit>())
{}

TapRecognizer::~TapRecognizer()
{

}
