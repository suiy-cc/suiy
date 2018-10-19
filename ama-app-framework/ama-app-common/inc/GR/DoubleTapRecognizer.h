/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file DoubleTouchRecognizer.h
/// @brief contains class DoubleTouchRecognizer
/// 
/// Created by zs on 2017/02/28.
///
/// this file contains the definition of class DoubleTouchRecognizer
/// 

#ifndef DOUBLETOUCHRECOGNIZER_H
#define DOUBLETOUCHRECOGNIZER_H

#include <string>

#include <GR/TouchRecognizer.h>

class DoubleTapRecognizer
: public TouchRecognizer
{
public:
    DoubleTapRecognizer();
    virtual ~DoubleTapRecognizer();
    std::string GetStateString() const override;
};


#endif // DOUBLETOUCHRECOGNIZER_H
