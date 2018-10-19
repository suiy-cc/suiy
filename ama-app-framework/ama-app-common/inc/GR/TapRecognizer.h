/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TapRecognizer.h
/// @brief contains class TapRecognizer
/// 
/// Created by zs on 2017/03/01.
///

/// this file contains the definition of class TapRecognizer
/// 

#ifndef TAPRECOGNIZER_H
#define TAPRECOGNIZER_H

#include <memory>

#include <GR/TouchRecognizer.h>
#include <GR/TapState.h>

class TapRecognizer
:public TouchRecognizer
{
public:
    TapRecognizer();
    virtual ~TapRecognizer();
};


#endif // TAPRECOGNIZER_H
