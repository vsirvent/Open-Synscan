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

#ifndef MOTOR_H_
#define MOTOR_H_

#include "Defines.h"
#include "Commands.h"
#include "TickTimer.h"

class Motor: public ITickListener {
public:
	static const u8 VERSION_MAJOR = 2;
	static const u8 VERSION_MINOR = 0;
	static const u8 VERSION_MICRO = 5;
	static const u8 VERSION_PATCH = 1;

	static const u32 MICRO_STEPS = 32;
	static const u32 MOUNT_RATIO = 144;
	static const u32 GEAR_RATIO_NUM = 60;
	static const u32 GEAR_RATIO_DEN = 16;
	static const u32 HIGH_SPEED_RATIO = 2;

	const float GEAR_RATIO = ((float)GEAR_RATIO_NUM/(float)GEAR_RATIO_DEN);
	const float STEP_DEGREE = 0.9f;
	const float DEGREE_PER_STEP = STEP_DEGREE/(GEAR_RATIO*MOUNT_RATIO*MICRO_STEPS);
	const u32 STEPS_PER_WORM_REV = (360.0f/(STEP_DEGREE))*(float)MICRO_STEPS*(float)GEAR_RATIO;
	const u32 STEPS_PER_RA_REV = STEPS_PER_WORM_REV*MOUNT_RATIO;

	const float SIDERAL_SPEED_ARCSEC = 15.04108444f;
	const float ARCSEC_TO_DEGREE = 0.000277778f;
	const float SIDERAL_SPEED_DEGREE = SIDERAL_SPEED_ARCSEC*ARCSEC_TO_DEGREE;
	const float SIDERAL_PERIOD_FREQ =  ((float)MICRO_STEPS*SIDERAL_SPEED_DEGREE)/(STEP_DEGREE/((float)GEAR_RATIO*(float)MOUNT_RATIO));
	const float SIDERAL_STEP_COUNT = (float)Defines::TIMER_FREQ/SIDERAL_PERIOD_FREQ;

private:
	int mPecPeriod = 0;
	int mPosition = 0x800000;
	int mTargetPosition = INFINITE;
	int mOrigPosition = INFINITE;

	int mBreakCount = 0;
	int mBreakPosition = 3500;

	const int mSideralStepPeriod = (int)(SIDERAL_STEP_COUNT);
	int mStepPeriod = mSideralStepPeriod;
	int mFastStepMult = 200;
	int mStepCount = 0;
	bool mStepDown = false;
	int mCurrentStepPeriod = mSideralStepPeriod;
	int mCurrentStepFreq = Defines::TIMER_FREQ/mCurrentStepPeriod;

	bool mToStop = false;
	bool mMoving = false;
	bool mMicroStep = false;

	ESpeed mSpeed = ESpeed::SLOW;
	ESlewType mType = ESlewType::NONE;
	EDirection mDir = EDirection::NONE;
	u32 mAutoGuideMode = 1000;
	u8 mDirPin = 0;
	u8 mStepPin = 0;
	u8 mMode[2];

	Timer* m100msecTimer = NULL;

	int mMaxCount = 0;
	int mMinCount = 0;
	int mCommStatusWD = 0;
	int mCommStatusTimeout = 20; //5 seconds. x 100msec
	int mPulseTotalTicks = 0;
	int mPulseCurrentTicks = 0;
	EDirection mPulseDir = EDirection::NONE;
	int mPulseStepFreq = 0;
	bool mIsPulseDone = false;
	EAxis mAxis;

	int updateCurrentStepPeriod(u32 target_step);
	void setStepPeriod(u32 period);
	void setMicroSteps(bool active);
	void setDir(EDirection dir);
	void stop();
	int  degreeToPosition(float degree);
	void on100msecTick();
	
public:

	Motor(EAxis axis);
	virtual ~Motor();
	void IRAM_ATTR onTick() override;
	Reply* processCommand(const Command* cmd);
	void printInfo() const;
};

#endif /* MOTOR_H_ */
