/*
 * Defines.h
 *
 *  Created on: 29 dic. 2018
 *      Author: Vicen
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
