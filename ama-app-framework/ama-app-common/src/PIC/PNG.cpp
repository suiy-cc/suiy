/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file PNG.cpp
/// @brief contains
///
/// Created by zs on 2017/04/06.
///

/// this file contains the implementation of class PNG
///

#include <png.h>

#include "PIC/PNG.h"
#include "log.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

#define PNG_BYTES_TO_CHECK 4

extern LOG_DECLARE_CONTEXT(SURF);

Pixel::Pixel()
{
    for(int i = 0; i < E_RGBA_COUNT; ++i)
        rgba[i] = 0;
}

unsigned int Pixel::GetRawPixel(E_RGBA format[E_RGBA_COUNT])
{
    unsigned int rawPixel = 0;
    for(int i = 0; i < E_RGBA_COUNT; ++i)
        if(format[i]!=E_RGBA_INVALID)
            rawPixel |= ((unsigned int)rgba[format[i]] << i*8);
    return rawPixel;
}

PNG::PNG()
    : path()
    , isOK(false)
    , width(0)
    , height(0)
    , buffer(nullptr)
{
    Clear();
}

PNG::PNG(const std::string &_path)
    : path(_path)
    , isOK(false)
    , width(0)
    , height(0)
    , buffer(nullptr)
{
    Clear();
    Open(_path);
}

PNG::~PNG()
{
    Clear();
}

void PNG::Open(std::string pngPath)
{
    DomainVerboseLog chatter(SURF, formatText("PNG::Open(%s)", pngPath.c_str()));

    path = pngPath;

    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep* row_pointers;
    char buf[PNG_BYTES_TO_CHECK];
    int x, y, temp, color_type;

    fp = fopen(path.c_str(), "rb");
    if(!fp)
    {
        isOK = false;
        logWarn(SURF, "PNG::Open(): file \"", path, "\" not found!");
        return;
    }

    png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );
    info_ptr = png_create_info_struct( png_ptr );

    setjmp( png_jmpbuf(png_ptr) );
    temp = fread( buf, 1, PNG_BYTES_TO_CHECK, fp );
    if( temp < PNG_BYTES_TO_CHECK ) {
        fclose(fp);
        png_destroy_read_struct( &png_ptr, &info_ptr, 0);
        isOK = false;
        logWarn(SURF, "PNG::Open(): byte check failure!");
        return;
    }

    temp = png_sig_cmp( (png_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK );
    if( temp != 0 ) {
        fclose(fp);
        png_destroy_read_struct( &png_ptr, &info_ptr, 0);
        isOK = false;
        logWarn(SURF, "PNG::Open(): signature check failure!");
        return;
    }


    rewind( fp );
    png_init_io( png_ptr, fp );
    png_read_png( png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0 );
    color_type = png_get_color_type( png_ptr, info_ptr );
    width = png_get_image_width( png_ptr, info_ptr );
    height = png_get_image_height( png_ptr, info_ptr );

    buffer = new Pixel[width*height];

    row_pointers = png_get_rows( png_ptr, info_ptr );
    switch( color_type )
    {
        case PNG_COLOR_TYPE_RGB_ALPHA:
            logInfo(SURF, "PNG::Open(): color type = ARGB");
            for( y=0; y<height; ++y )
            {
                for( x=0; x<width*4; )
                {
                    buffer[x/4+y*width].rgba[E_RGBA_R] = row_pointers[y][x++]; // red
                    buffer[x/4+y*width].rgba[E_RGBA_G] = row_pointers[y][x++]; // green
                    buffer[x/4+y*width].rgba[E_RGBA_B] = row_pointers[y][x++]; // blue
                    buffer[x/4+y*width].rgba[E_RGBA_A] = row_pointers[y][x++]; // alpha

                }
            }
            break;

        case PNG_COLOR_TYPE_RGB:
            logInfo(SURF, "PNG::Open(): color type = RGB");
            for( y=0; y<height; ++y )
            {
                for( x=0; x<width*3; )
                {
                    buffer[x/3+y*width].rgba[E_RGBA_R] = row_pointers[y][x++]; // red
                    buffer[x/3+y*width].rgba[E_RGBA_G] = row_pointers[y][x++]; // green
                    buffer[x/3+y*width].rgba[E_RGBA_B] = row_pointers[y][x++]; // blue
                    buffer[x/3+y*width].rgba[E_RGBA_A] = 0xff;
                }
            }
            break;
        default:
            isOK = false;
            fclose(fp);
            png_destroy_read_struct( &png_ptr, &info_ptr, 0);
            logWarn(SURF, "PNG::Open(): unsupported color type!");
            return;
    }
    fclose(fp);
    png_destroy_read_struct( &png_ptr, &info_ptr, 0);
    isOK = true;
}

void PNG::Clear()
{
    path.clear();
    isOK = false;
    width = 0;
    height = 0;
    delete[] buffer;
    buffer = nullptr;
}

bool PNG::IsOK()
{
    return isOK;
}

void PNG::GetDimension(unsigned int &width, unsigned int &height)
{
    width = this->width;
    height = this->height;
}

unsigned char PNG::GetPixel(unsigned int x, unsigned int y, E_RGBA rgba)
{
    if(buffer && x<width && y<height)
        return buffer[y*width + x].rgba[rgba];
    else
        return 0;
}

Pixel PNG::GetPixel(unsigned int x, unsigned int y)
{
    Pixel pixel;

    if(buffer && x<width && y<height)
        pixel = buffer[y*width + x];

    return pixel;
}
