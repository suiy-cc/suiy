/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file PNG.h
/// @brief contains class PNG
/// 
/// Created by zs on 2017/04/06.
///

/// this file contains the definition of class PNG
/// 

#ifndef AMA_APP_FRAMEWORK_PNG_H
#define AMA_APP_FRAMEWORK_PNG_H

#include <string>

enum E_RGBA{
    E_RGBA_R,
    E_RGBA_G,
    E_RGBA_B,
    E_RGBA_A,

    E_RGBA_COUNT,

    E_RGBA_INVALID,
};

static E_RGBA ARGB[E_RGBA_COUNT] = {
    E_RGBA_B,
    E_RGBA_G,
    E_RGBA_R,
    E_RGBA_A,
};

class Pixel{
public:
    unsigned char rgba[E_RGBA_COUNT];

    Pixel();
    unsigned int GetRawPixel(E_RGBA format[E_RGBA_COUNT]);
};

class PNG
{
public:
    PNG();
    PNG(const std::string &path);
    virtual ~PNG();

    void Open(std::string pngPath);
    void Clear();
    bool IsOK();

    void GetDimension(unsigned int& width, unsigned int& height);
    unsigned char GetPixel(unsigned int x, unsigned int y, E_RGBA rgba);
    Pixel GetPixel(unsigned int x, unsigned int y);

private:
    std::string path;
    bool isOK;
    unsigned int width;
    unsigned int height;
    Pixel* buffer;
};


#endif //AMA_APP_FRAMEWORK_PNG_H
