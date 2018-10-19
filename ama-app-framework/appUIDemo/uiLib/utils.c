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

#include "hmi.h"

unsigned int getSeatIndexBySurfaceId(unsigned int surface)
{
	unsigned int seatIdx = 0;

    if (DEMO_GET_SEAT_ID(surface) == DEMO_SEAT_ID_IVI_F) {
        seatIdx = DEMO_SCREEN_IVI_F;
    }
    else if (DEMO_GET_SEAT_ID(surface) == DEMO_SEAT_ID_RSE_L) {
        seatIdx = DEMO_SCREEN_RSE_L;
    }
    else {
        seatIdx = DEMO_SCREEN_RSE_R;
    }

    return seatIdx;
}
