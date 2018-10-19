/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ZoomInAnimateStrategy.cpp
/// @brief contains the implementation of class ZoomInAnimateStrategy
///
/// Created by zs on 2016/6/13.
/// this file contains the implementation of class ZoomInAnimateStrategy
///

#include <chrono>
#include <thread>

#include "ZoomInAnimateStrategy.h"

ZoomInAnimateStrategy::ZoomInAnimateStrategy()
:frameCount(0)
{
    ;
}

ZoomInAnimateStrategy::~ZoomInAnimateStrategy()
{
    ;
}

void ZoomInAnimateStrategy::Play(std::function<void(const Section&)> funcDrawAFrame)
{
    InitializeForOnePlay();

    while(!IsAnimateEnd())
    {
        funcDrawAFrame(GetSection());
    }

    DestroyForOnePlay();
}

void ZoomInAnimateStrategy::SetStartPosition(const Position& pos)
{
    startPosition = pos;
}

void ZoomInAnimateStrategy::SetStartSize(const Size& size)
{
    startSize = size;
}

void ZoomInAnimateStrategy::SetEndSize(const Size& size)
{
    endSize = size;
}

void ZoomInAnimateStrategy::InitializeForOnePlay()
{
    frameCount = 0;

    using namespace std::chrono;
    timestampStart = high_resolution_clock::now();
}

void ZoomInAnimateStrategy::DestroyForOnePlay()
{
    frameCount = 0;
}

static const std::chrono::duration<unsigned int, std::milli> totalTime(500);
static const unsigned int totalFrame = 25;
Section ZoomInAnimateStrategy::GetSection()
{
    // calculate section size
    Section section;
    section.left = (double(0)-startPosition.x)/totalFrame*frameCount + startPosition.x;
    section.top = (double(0)-startPosition.y)/totalFrame*frameCount + startPosition.y;
    section.width = (double(endSize.width) - startSize.width)/totalFrame*frameCount + startSize.width;
    section.height = (double(endSize.height) - startSize.height)/totalFrame*frameCount + startSize.height;

    // halt until it's right time to draw
    auto timestampDraw = timestampStart + totalTime/totalFrame*frameCount;
    auto timestampNow = std::chrono::high_resolution_clock::now();
    if(timestampNow < timestampDraw)
        std::this_thread::sleep_until(timestampDraw);

    // increase frameCount
    ++frameCount;

    return section;
}

bool ZoomInAnimateStrategy::IsAnimateEnd()const
{
    return frameCount > totalFrame;
}