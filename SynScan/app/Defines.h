/*
 * This file is part of the Open Synscan Project distribution (https://github.com/vsirvent/Synscan).
 * Copyright (c) 2019 Vicente Sirvent Orts.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#include <WString.h>
#include <Logger.h>
#include <SmingCore/SmingCore.h>

struct Defines {
	static const u32 LOG_PORT = 6667;
	static const u32 SYNSCAN_PORT = 11880;
	static const u32 TIMER_FREQ = 64935*3; //EQ6 interrupt freq
};

//Use WIFI_STATION to connect Open-Synscan to an existing wifi network.
//Open-synscan can communicate through "Synscan" network AND configured WIFI_STATION_SSID
//network. This is usefull if you have several WIFI devices in your setup

/**
 * Example:
 *                                    |<-->[DEVICE: Camera]
 *                                    |<-->[DEVICE: Focuser]
 * [LAPTOP]<-->[WIFI_STATION_SSID]<-->|
 *                                    |<-->[DEVICE: Open-synscan]<-->[SSID:Synscan(192.168.5.1)]<-->...
 */
#if 1
#define WIFI_STATION
#define WIFI_STATION_SSID "Focuser"
#define WIFI_STATION_PASS "12345678"
#endif

#define OVERCLOCK
#ifdef OVERCLOCK
#define CPU_FREQ eCF_160MHz
#else
#define CPU_FREQ eCF_80MHz
#endif

#define INFINITE 0xFFFFFFFF

#define MIN(a, b) ((a < b)?a:b)

enum class EAxis {
	AXIS_RA = 1, AXIS_DEC = 2, AXIS_BOTH = 3, AXIS_NONE = 0
};

enum class ESlewType {
	GOTO, TRACKING, NONE
};
enum class ESpeed {
	FAST, SLOW, NONE
};
enum class EDirection {
	CCW, CW, NONE
};


#endif /* DEFINES_H_ */
