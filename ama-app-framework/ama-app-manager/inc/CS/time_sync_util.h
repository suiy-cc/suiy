/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef _TIME_SYNC_UTIL_H
#define _TIME_SYNC_UTIL_H

#include <stdint.h>
#include <gnss.h>
#include <gnss-init.h>

struct GPSTime
{
	uint16_t Year;
	uint8_t Month;
	uint8_t Day;

	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
};

class TimeSyncUtil
{
public:
	static void updateTime(GPSTime t);
	static void startReceive();
	static void stopReceive();
	static void dispatchTime2System(const GPSTime & t);
	static void dispatchTime2MCU(const GPSTime & t);
	static void mcuTime2System();
	
private:
	static void printTime(const GPSTime & t);
	static GPSTime utc2local(const GPSTime & time);
	static void gpsTimeCallback(const TGNSSTime time[], uint16_t numElements);
	static void gpsPositionCallback(const TGNSSPosition position[], uint16_t numElements);
};

#endif
