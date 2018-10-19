/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ZoomInAnimateStrategy.h
/// @brief contains class ZoomInAnimateStrategy
///
/// Created by zs on 2016/7/6.
/// this file contains the definition of class ZoomInAnimateStrategy
///

#ifndef ZOOMINANIMATESTRATEGY_H
#define ZOOMINANIMATESTRATEGY_H

#include <functional>
#include <chrono>

#include "HMIType.h"

class ZoomInAnimateStrategy
{
public:
    ZoomInAnimateStrategy();
    virtual ~ZoomInAnimateStrategy();
public:
    void Play(std::function<void(const Section&)> funcDrawAFrame);

    void SetStartPosition(const Position& pos);
    void SetStartSize(const Size& size);
    void SetEndSize(const Size& size);

private:
    virtual void InitializeForOnePlay();
    virtual void DestroyForOnePlay();
    virtual Section GetSection();
    virtual bool IsAnimateEnd()const;

private:
    unsigned int frameCount;
    std::chrono::time_point<std::chrono::high_resolution_clock> timestampStart;
    Position startPosition;
    Size startSize;
    Size endSize;
};


#endif // ZOOMINANIMATESTRATEGY_H
