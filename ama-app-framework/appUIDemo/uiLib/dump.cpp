/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// #include <iostream>
// using std::cout;

// #include <iomanip>
// using std::dec;
// using std::hex;

#include "common.h"
#include "hmi.h"


void printSurfaceProperties(unsigned int surfaceid, const char* prefix)
{
    // cout << prefix << "surface " << surfaceid << " (0x" << hex << surfaceid
    //         << dec << ")\n";
    // cout << prefix << "---------------------------------------\n";

    LOGI("surface %d (0x%08X)\n", surfaceid, surfaceid);
    LOGI("---------------------------------------\n");

    ilmSurfaceProperties p;

    ilmErrorTypes callResult = ilm_getPropertiesOfSurface(surfaceid, &p);
    if (ILM_SUCCESS != callResult)
    {
        // cout << "LayerManagerService returned: " << ILM_ERROR_STRING(callResult) << "\n";
        // cout << "No surface with ID " << surfaceid << " found\n";
        LOGI("LayerManagerService returned: %s\n", ILM_ERROR_STRING(callResult));
        LOGI("No surface with ID %d found\n", surfaceid);
        return;
    }

    // cout << prefix << "- created by pid:       " << p.creatorPid << "\n";
    LOGI("%s- created by pid:       %d\n", prefix, p.creatorPid);

    // cout << prefix << "- original size:      x=" << p.origSourceWidth << ", y="
    //         << p.origSourceHeight << "\n";
    // cout << prefix << "- destination region: x=" << p.destX << ", y=" << p.destY
    //         << ", w=" << p.destWidth << ", h=" << p.destHeight << "\n";
    // cout << prefix << "- source region:      x=" << p.sourceX << ", y="
    //         << p.sourceY << ", w=" << p.sourceWidth << ", h=" << p.sourceHeight
    //         << "\n";

    LOGI("%s- original size:      x=%d, y=%d\n", prefix, p.origSourceWidth, p.origSourceHeight);
    LOGI("%s- destination region: x=%d, y=%d, w=%d, h=%d\n", prefix, p.destX, p.destY, p.destWidth, p.destHeight);
    LOGI("%s- source region:      x=%d, y=%d, w=%d, h=%d\n", prefix, p.sourceX, p.sourceY, p.sourceWidth, p.sourceHeight);

    // cout << prefix << "- orientation:        " << p.orientation << " (";
    LOGI("%s- orientation:        %d (", prefix, p.orientation);
    switch (p.orientation)
    {
    case 0/*Zero*/:
        // cout << "up is top)\n";
        LOGI("up is top)\n");
        break;
    case 1/*Ninety*/:
        // cout << "up is right)\n";
        LOGI("up is right)\n");
        break;
    case 2/*OneEighty*/:
        // cout << "up is bottom)\n";
        LOGI("up is bottom)\n");
        break;
    case 3/*TwoSeventy*/:
        // cout << "up is left)\n";
        LOGI("up is left)\n");
        break;
    default:
        // cout << "unknown)\n";
        LOGI("unknown)\n");
        break;
    }

    // cout << prefix << "- opacity:            " << p.opacity << "\n";
    // cout << prefix << "- visibility:         " << p.visibility << "\n";
    LOGI("%s- opacity:            %f\n", prefix, p.opacity);
    LOGI("%s- visibility:         %d\n", prefix, p.visibility);

    // cout << prefix << "- pixel format:       " << p.pixelformat << " (";
    LOGI("%s- pixel format:       %d (", prefix, p.pixelformat);
    switch (p.pixelformat)
    {
    case 0/*PIXELFORMAT_R8*/:
        // cout << "R-8)\n";
        LOGI("R-8)\n");
        break;
    case 1/*PIXELFORMAT_RGB888*/:
        // cout << "RGB-888)\n";
        LOGI("RGB-888)\n");
        break;
    case 2/*PIXELFORMAT_RGBA8888*/:
        // cout << "RGBA-8888)\n";
        LOGI("RGBA-8888)\n");
        break;
    case 3/*PIXELFORMAT_RGB565*/:
        // cout << "RGB-565)\n";
        LOGI("RGB-565)\n");
        break;
    case 4/*PIXELFORMAT_RGBA5551*/:
        // cout << "RGBA-5551)\n";
        LOGI("RGBA-5551)\n");
        break;
    case 5/*PIXELFORMAT_RGBA6661*/:
        // cout << "RGBA-6661)\n";
        LOGI("RGBA-6661)\n");
        break;
    case 6/*PIXELFORMAT_RGBA4444*/:
        // cout << "RGBA-4444)\n";
        LOGI("RGBA-4444)\n");
        break;
    default:
        // cout << "unknown)\n";
        LOGI("unknown)\n");
        break;
    }

    // cout << prefix << "- native surface:     " << p.nativeSurface << "\n";
    LOGI("%s- native surface:     %d\n", prefix, p.nativeSurface);

    // cout << prefix << "- counters:           frame=" << p.frameCounter
    //         << ", draw=" << p.drawCounter << ", update=" << p.updateCounter
    //         << "\n";
    LOGI("%s- counters:           frame=%d, draw=%d, update=%d\n", prefix, p.frameCounter, p.drawCounter, p.updateCounter);

    // cout << prefix << "- on layer:           ";
    LOGI("%s- on layer:           ", prefix);
    int layerCount = 0;
    unsigned int* layerArray = NULL;

    callResult = ilm_getLayerIDs(&layerCount, &layerArray);
    if (ILM_SUCCESS != callResult)
    {
        // cout << "LayerManagerService returned: " << ILM_ERROR_STRING(callResult) << "\n";
        // cout << "Failed to get available layer IDs\n";
        LOGI("LayerManagerService returned: %s\n", ILM_ERROR_STRING(callResult));
        LOGI("Failed to get available layer IDs\n");
        return;
    }

    for (int layerIndex = 0; layerIndex < layerCount; ++layerIndex)
    {
        unsigned int layerid = layerArray[layerIndex];
        int surfaceCount = 0;
        unsigned int* surfaceArray = NULL;

        callResult = ilm_getSurfaceIDsOnLayer(layerid, &surfaceCount, &surfaceArray);
        if (ILM_SUCCESS != callResult)
        {
            // cout << "LayerManagerService returned: " << ILM_ERROR_STRING(callResult) << "\n";
            // cout << "Failed to get surface IDs on layer" << layerid << "\n";
            LOGI("LayerManagerService returned: %s\n", ILM_ERROR_STRING(callResult));
            LOGI("Failed to get surface IDs on layer%d\n", layerid);
            return;
        }

        for (int surfaceIndex = 0; surfaceIndex < surfaceCount;
                ++surfaceIndex)
        {
            unsigned int id = surfaceArray[surfaceIndex];
            if (id == surfaceid)
            {
                // cout << layerid << "(0x" << hex << layerid << dec << ") ";
                LOGI("%d(0x%08X)", layerid, layerid);
            }
        }
    }
    // cout << "\n";
    LOGI("\n");
}
