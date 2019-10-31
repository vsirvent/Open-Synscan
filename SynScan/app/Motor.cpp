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

#include "Motor.h"
#include "MainApp.h"

Motor::Motor(EAxis axis) {

	Logger::notice("Motor[%d]: Initializing %s motor...", axis,
			axis == EAxis::AXIS_RA ? "RA" : "DEC");
	mAxis = axis;
	if (mAxis == EAxis::AXIS_RA) {
		mDirPin = 4;
		mStepPin = 5;
		mMode[0] = 2;
		mMode[1] = 15;
		mMaxCount = 9484409;
		mMinCount = 4288680;
		mPosition = degreeToPosition(0.0f);
	} else if (mAxis == EAxis::AXIS_DEC) {
		mDirPin = 13;
		mStepPin = 12;
		mMode[0] = 14;
		mMode[1] = 16;
		mMaxCount = degreeToPosition(165.0f);
		mMinCount = degreeToPosition(15.0f);
		mPosition = degreeToPosition(90.0f);
	}

	pinMode(mDirPin, OUTPUT);
	pinMode(mStepPin, OUTPUT);
	pinMode(mMode[0], OUTPUT);
	pinMode(mMode[1], OUTPUT);

	//init pins
	setDir(EDirection::CW);
	digitalWrite(mStepPin, 0);

	//microstep active
	digitalWrite(mMode[0], 1);
	digitalWrite(mMode[1], 1);
	mMicroStep = true;

	Serial.printf("MOUNT_RATIO = %d\n", MOUNT_RATIO);
	Serial.printf("GEAR_RATIO_NUM = %d\n", GEAR_RATIO_NUM);
	Serial.printf("GEAR_RATIO_DEN = %d\n", GEAR_RATIO_DEN);
	Serial.printf("GEAR_RATIO = %f\n", GEAR_RATIO);
	Serial.printf("HIGH_SPEED_RATIO = %d\n", HIGH_SPEED_RATIO);
	Serial.printf("STEP_DEGREE = %f\n", STEP_DEGREE);
	Serial.printf("STEPS_PER_WORM_REV = %d\n", STEPS_PER_WORM_REV);
	Serial.printf("STEPS_PER_RA_REV = %d\n", STEPS_PER_RA_REV);
	Serial.printf("SIDERAL_STEP_COUNT = %f\n", SIDERAL_STEP_COUNT);
	Serial.printf("100MS_STEP = %d\n", m100msPeriod);
	Serial.printf("TIMER_FREQ = %d\n", Defines::TIMER_FREQ);

	m100msecTimer = new Timer();
	m100msecTimer->initializeMs(100, TimerDelegate(&Motor::on100msecTick, this)).start();

	Logger::notice("Motor[%d]: Init done", mAxis);
}

Motor::~Motor() {
	m100msecTimer->stop();
	delete m100msecTimer;
}

void Motor::setMicroSteps(bool active) {
	//always active
	if (HIGH_SPEED_RATIO == 1) {
		active = true; //same high and low speed
	}
	if (mMicroStep != active) {
		if (active) {
			digitalWrite(mMode[0], 1);
			digitalWrite(mMode[1], 1);
		} else {
			digitalWrite(mMode[0], 1);
			digitalWrite(mMode[1], 0);
		}
		mMicroStep = active;
	}
}

int Motor::updateCurrentStepPeriod(int target_step) {
	int ret = target_step;
	if (mCurrentStepPeriod != target_step) {
		int current_step_freq = mCurrentStepFreq;
		int target_step_freq = (target_step!=0)?Defines::TIMER_FREQ / target_step:INFINITE;
		int diff_freq = abs(current_step_freq - target_step_freq);

		//Only apply acceleration if diff is greater than sideral freq.
		if (diff_freq > SIDERAL_PERIOD_FREQ) {
			int block_freq = diff_freq / 8;
			if (block_freq < SIDERAL_PERIOD_FREQ) {
				block_freq = SIDERAL_PERIOD_FREQ;
			}

			if (current_step_freq < target_step_freq) {
				current_step_freq += block_freq;
				if (current_step_freq > target_step_freq) {
					current_step_freq = target_step_freq;
				}
			} else if (current_step_freq > target_step_freq) {
				current_step_freq -= block_freq;
				if (current_step_freq < target_step_freq) {
					current_step_freq = target_step_freq;
				}
			}
			ret = (current_step_freq!=0)?(Defines::TIMER_FREQ / current_step_freq):INFINITE; //new step period
		}
	}
	if (ret < 4) {
		ret = 4; //minimum 4 periods per step
	}
	return ret;
}

void Motor::setStepPeriod(u32 period) {
	u32 speed = (period!=0)?(mSideralStepPeriod / period):INFINITE;
	if (!mMicroStep) {
		speed *= HIGH_SPEED_RATIO;
	}

	//Limit speed to mFastStepMult
	if (speed > mFastStepMult) {
		period = mSideralStepPeriod / mFastStepMult;
		if (!mMicroStep) {
			period *= HIGH_SPEED_RATIO;
		}
	}
	mStepPeriod = period;
}

void Motor::setDir(EDirection dir) {
	if (dir != mDir) {
		Logger::notice("Motor[%d]: Direction: %s", mAxis,
				dir == EDirection::CW ? "CW" : "CCW");
		mDir = dir;
		if (mDir == EDirection::CW) {
			digitalWrite(mDirPin, 1);
		} else {
			digitalWrite(mDirPin, 0);
		}
	}
}

int Motor::degreeToPosition(float degree) {
	return (int) ((0x800000 + (int) (degree / DEGREE_PER_STEP)));
}

void Motor::stop() {
	mMoving = false;
	digitalWrite(mStepPin, 0);
	mStepCount = 0;
}

void Motor::on100msecTick() {
	if (mType == ESlewType::GOTO) {
		//Stop if controller is not sending commands
		//(comm fail protection).
		if (mCommStatusWD++ >= mCommStatusTimeout) {
			mToStop = true;
		}

		if (mToStop) {
			//Decelerate to sideral speed and stop
			mCurrentStepPeriod = updateCurrentStepPeriod(mSideralStepPeriod);
			mCurrentStepFreq = (mCurrentStepPeriod!=0)?(Defines::TIMER_FREQ/mCurrentStepPeriod):INFINITE;
			if (mCurrentStepPeriod >= mSideralStepPeriod) {
				stop();
			}

		} else {
			if (mTargetPosition == INFINITE) {
				stop();
			}
			//Normal goto
			u32 remain = abs(mTargetPosition - mPosition) / 50;
			u32 elapsed = abs(mPosition - mOrigPosition) / 50;
			u32 mult = MIN(remain, elapsed);
			mult = MIN(mFastStepMult, mult);
			mult = MIN(mSideralStepPeriod, mult);

			if (!mMicroStep) {
				mult /= HIGH_SPEED_RATIO;
			}

			if (mult > 1) {
				mCurrentStepPeriod = (mSideralStepPeriod / mult);
			} else {
				mCurrentStepPeriod = ((int) (SIDERAL_STEP_COUNT / 2.0f));
			}
			mCurrentStepFreq = (mCurrentStepPeriod!=0)?(Defines::TIMER_FREQ/mCurrentStepPeriod):INFINITE;
		}
	} else {
		//Track
		int sideral = mSideralStepPeriod;
		//This code was not really used
		//if (mAutoGuideMode != 1000) {
		//	sideral = (sideral * 1000) / mAutoGuideMode;
		//}
		int target_period = mToStop ? sideral : mStepPeriod;
		mCurrentStepPeriod = updateCurrentStepPeriod(target_period);
		mCurrentStepFreq = (mCurrentStepPeriod!=0)?Defines::TIMER_FREQ/mCurrentStepPeriod:INFINITE;

		if (mCurrentStepPeriod >= mSideralStepPeriod && mToStop) {
			stop();
		}
	}
	if (mMoving) {
		//if ((mDir == EDirection::CW && mPosition > mMaxCount)
		//		|| (mDir == EDirection::CCW && mPosition < mMinCount)) {
			//Absolute position protection
			//TODO: Calculate correctly the limits, depending the RA position
			//DEC limit changed. The current limit algorithm is too hard.
		//	stop();
		//}
	}
	if (mIsPulseDone) {
		Logger::notice("Motor[%d]::on100msecTick: Pulse guide done", mAxis);
		mIsPulseDone = false;
	}
}

void IRAM_ATTR Motor::onTick() {
	bool pulse_moving = false;
	int current_step_period = mCurrentStepPeriod;
	EDirection current_dir = mDir;

	if (++mPulseCurrentTicks <= mPulseTotalTicks) {
		//Modify current step period with pulse guide
		pulse_moving = true;
		if (mMoving) {
			int pulse_step_freq = mPulseStepFreq;
			if (mDir != mPulseDir) {
				pulse_step_freq *= -1;
			}
			int result_step_freq = mCurrentStepFreq + pulse_step_freq;
			if (result_step_freq < 0) {
				//If negative step freq change direction
				current_dir = (current_dir==EDirection::CW)?EDirection::CCW:EDirection::CW;
				result_step_freq = abs(result_step_freq);
			}
			current_step_period = (result_step_freq!=0)?Defines::TIMER_FREQ/result_step_freq:INFINITE;
		}else{
			//If motor is not moving just set pulse guide values
			current_dir = mPulseDir;
			setDir(current_dir);
			current_step_period = (mPulseStepFreq!=0)?Defines::TIMER_FREQ/mPulseStepFreq:INFINITE;
		}
		if (mPulseCurrentTicks == mPulseTotalTicks) {
			mIsPulseDone = true;
		}
	}
	if (mMoving || pulse_moving) {

		mStepCount++;

		u32 p2 = current_step_period / 2;
		if (mStepCount >= current_step_period) {

			mStepCount = 0;
			mStepDown = true;
			digitalWrite(mStepPin, 0);

			u32 amount = mMicroStep ? 1 : HIGH_SPEED_RATIO;

			if (current_dir == EDirection::CCW) {
				amount = -amount;
			}

			mPosition += amount;

			if (mType == ESlewType::GOTO) {
				if (current_dir == EDirection::CW) {
					if (mPosition >= mTargetPosition) {
						stop();
					}
				} else {
					if (mPosition <= mTargetPosition) {
						stop();
					}
				}
			}

		} else if (mStepCount >= p2 && mStepDown) {
			mStepDown = false;
			digitalWrite(mStepPin, 1);
		}
	}
}

void Motor::printInfo() const {
	u32 speed = mSideralStepPeriod / mCurrentStepPeriod;
	if (!mMicroStep) {
		speed *= HIGH_SPEED_RATIO;
	}
	Logger::notice(
			"Motor[%d]: running: %d, speed: x%d, micro: %d, step: %d, track_speed: %d, position: %d, target: %d, break: %d",
			mAxis, mMoving, speed, mMicroStep, mCurrentStepPeriod, mSpeed,
			mPosition, mTargetPosition, mBreakPosition);
}

Reply*
Motor::processCommand(const Command* cmd) {
	Reply* reply = NULL;
	mCommStatusWD = 0;
	switch (cmd->getCmd()) {
	case ECommand::GET_VERSION: {
		VersionReply* version_reply = new VersionReply();
		version_reply->setVersion(VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO,
				VERSION_PATCH);
		reply = version_reply;
		break;
	}
	case ECommand::PULSE_GUIDE: {
		PulseGuideCmd* pulse_cmd = (PulseGuideCmd*)cmd;
		Logger::notice("Motor[%d]: Pulse guide: dir = %d, msec = %d, rate(x10) = %d",
				mAxis, pulse_cmd->GetDir(), pulse_cmd->GetDuration(), (int)(pulse_cmd->GetRate()*10.0f));
		mPulseDir = pulse_cmd->GetDir();
		mPulseStepFreq = (int)((float)(Defines::TIMER_FREQ/(int)(mSideralStepPeriod))*pulse_cmd->GetRate());
		mPulseCurrentTicks = 0;
		mPulseTotalTicks = (pulse_cmd->GetDuration()*Defines::TIMER_FREQ)/1000;
		break;
	}
	case ECommand::SET_POSITION: {
		Logger::notice("Motor[%d]: SET_POSITION", mAxis);
		if (!mMoving) {
			SetPositionCommand* pos_cmd = (SetPositionCommand*) cmd;
			mPosition = pos_cmd->GetPosition();
			Logger::notice("Motor[%d]: position = %d [%X]", mAxis, mPosition,
					mPosition);
			reply = new EmptyReply();
		} else {
			Logger::error("Motor[%d]: Motor not stopped", mAxis);
			reply = new ErrorReply(ErrorReply::EErrorCode::Motor_not_Stopped);
		}
		break;
	}
	case ECommand::INIT_DONE: {
		Logger::notice("Motor[%d]: INIT_DONE", mAxis);
		reply = new EmptyReply();
		break;
	}
	case ECommand::SET_MOTION_MODE: {
		Logger::notice("Motor[%d]: SET_MOTION_MODE", mAxis);
		if (!mMoving) {
			SetMotionMode* motion_mode = (SetMotionMode*) cmd;
			mSpeed = motion_mode->getSpeed();
			mType = motion_mode->getType();
			setDir(motion_mode->getDir());
			Logger::notice("Motor[%d]: slew = %s, speed = %s, dir = %s", mAxis,
					mType == ESlewType::GOTO ? "GOTO" : "TRACK",
					mSpeed == ESpeed::FAST ? "FAST" : "SLOW",
					mDir == EDirection::CW ? "CW" : "CCW");
			reply = new EmptyReply();
		} else {
			Logger::error("Motor[%d]: Motor not stopped", mAxis);
			reply = new ErrorReply(ErrorReply::EErrorCode::Motor_not_Stopped);
		}
		break;
	}
	case ECommand::SET_GOTO_TARGET: {
		Logger::notice("Motor[%d]: SET_GOTO_TARGET", mAxis);
		if (!mMoving) {
			SetGotoTarget* target = (SetGotoTarget*) cmd;
			mOrigPosition = mPosition;
			mTargetPosition = target->GetPosition();
			Logger::notice("Motor[%d]: target = %d [%X]", mTargetPosition,
					mTargetPosition);
			reply = new EmptyReply();
		} else {
			Logger::error("Motor[%d]: Motor not stopped", mAxis);
			reply = new ErrorReply(ErrorReply::EErrorCode::Motor_not_Stopped);
		}
		break;
	}
	case ECommand::SET_GOTO_TARGET_INCREMENT: {
		Logger::notice("Motor[%d]: SET_GOTO_TARGET_INCREMENT", mAxis);
		if (!mMoving) {
			SetGotoTargetIncrement* target = (SetGotoTargetIncrement*) cmd;
			mOrigPosition = mPosition;
			int increment = target->GetIncrement();
			if (mDir == EDirection::CW) {
				mTargetPosition = mPosition + increment;
			} else {
				mTargetPosition = mPosition - increment;
			}
			Logger::notice(
					"Motor[%d]: position = %d, increment = %d, target = %d",
					mAxis, mPosition, increment, mTargetPosition);
			reply = new EmptyReply();
		} else {
			Logger::error("Motor[%d]: Motor not stopped", mAxis);
			reply = new ErrorReply(ErrorReply::EErrorCode::Motor_not_Stopped);
		}
		break;
	}
	case ECommand::SET_BREAK_POINT_INCREMENT: {
		Logger::notice("Motor[%d]: SET_BREAK_POINT_INCREMENT", mAxis);
		if (!mMoving) {
			SetBreakPointIncrement* target = (SetBreakPointIncrement*) cmd;
			mBreakPosition = target->GetIncrement();
			Logger::notice("Motor[%d]: break point = %d", mAxis,
					mBreakPosition);
			reply = new EmptyReply();
		} else {
			Logger::error("Motor[%d]: Motor not stopped", mAxis);
			reply = new ErrorReply(ErrorReply::EErrorCode::Motor_not_Stopped);
		}
		break;
	}
	case ECommand::GET_GOTO_TARGET: {
		Logger::verbose("Motor[%d]: GET_GOTO_TARGET", mAxis);
		PositionReply* position_reply = new PositionReply();
		position_reply->setData(mTargetPosition, 6);
		reply = position_reply;
		break;
	}
	case ECommand::SET_STEP_PERIOD: {
		Logger::verbose("Motor[%d]: SET_STEP_PERIOD", mAxis);
		SetStepPeriod* step_period = (SetStepPeriod*) cmd;
		setStepPeriod(step_period->GetPeriod());
		Logger::notice("Motor[%d]: Step period = %d", mAxis, mStepPeriod);
		reply = new EmptyReply();
		break;
	}
	case ECommand::GET_STEP_PERIOD: {
		Logger::verbose("Motor[%d]: GET_STEP_PERIOD: %d", mAxis, mStepPeriod);
		DataReply* step_reply = new DataReply();
		step_reply->setData(mStepPeriod, 6);
		reply = step_reply;
		break;
	}
	case ECommand::START_MOTION: {
		Logger::notice("Motor[%d]: START_MOTION!", mAxis);
		switch (mSpeed) {
		case ESpeed::SLOW:
			setMicroSteps(true);
			break;
		case ESpeed::FAST:
			setMicroSteps(false);
			break;
		default: {
			Logger::warning("Motor[%d]: Speed not set, using slow speed",
					mAxis);
			setMicroSteps(true);
			break;
		}
		}
		if (mType != ESlewType::NONE) {
			mCurrentStepPeriod = mSideralStepPeriod;
			mCurrentStepFreq = Defines::TIMER_FREQ/mCurrentStepPeriod;
			mToStop = false;
			mMoving = true;
			reply = new EmptyReply();
		} else {
			reply = new ErrorReply(ErrorReply::EErrorCode::Not_Initialized);
		}
		break;
	}
	case ECommand::STOP_MOTION: {
		Logger::notice("Motor[%d]: STOP_MOTION", mAxis);
		mToStop = true;
		reply = new EmptyReply();
		break;
	}
	case ECommand::INSTANT_STOP: {
		Logger::warning("Motor[%d]: INSTANT_STOP", mAxis);
		stop();
		break;
	}
	case ECommand::SET_SWITCH_ONOFF: {
		Logger::notice("Motor[%d]: SET_SWITCH_ONOFF", mAxis);
		//Ignore
		reply = new EmptyReply();
		break;
	}
	case ECommand::SET_AUTOGUIDE_SPEED: {
		Logger::notice("Motor[%d]: SET_AUTOGUIDE_SPEED", mAxis);
		SetAutoGuideSpeed* auto_cmd = (SetAutoGuideSpeed*) cmd;
		mAutoGuideMode = auto_cmd->getSpeed();
		Logger::notice("Motor[%d]: Autoguide speed: %d/1000", mAxis,
				mAutoGuideMode);
		reply = new EmptyReply();
		break;
	}
	case ECommand::RUN_BOOTLOADER_MODE: {
		Logger::verbose("Motor[%d]: RUN_BOOTLOADER_MODE", mAxis);
		ErrorReply* error_reply = new ErrorReply();
		error_reply->setError(ErrorReply::EErrorCode::Unknown_Command);
		reply = error_reply;
		break;
	}
	case ECommand::SET_POLAR_SCOPE_LED_BRIGHTNESS: {
		Logger::verbose("Motor[%d]: SET_POLAR_SCOPE_LED_BRIGHTNESS", mAxis);
		//Ignore
		reply = new EmptyReply();
		break;
	}
	case ECommand::GET_PEC_PERIOD: {
		Logger::verbose("Motor[%d]: GET_PEC_PERIOD", mAxis);
		DataReply* data_reply = new DataReply();
		data_reply->setData(mPecPeriod, 6);
		reply = data_reply;
		break;
	}
	case ECommand::GET_COUNTS_PER_REVOLUTION: {
		Logger::verbose("Motor[%d]: GET_COUNTS_PER_REVOLUTION", mAxis);
		DataReply* data_reply = new DataReply();
		data_reply->setData(STEPS_PER_RA_REV, 6);
		reply = data_reply;
		break;
	}
	case ECommand::GET_TIMER_FREQ: {
		Logger::verbose("Motor[%d]: GET_TIMER_FREQ", mAxis);
		DataReply* data_reply = new DataReply();
		data_reply->setData(Defines::TIMER_FREQ, 6);
		reply = data_reply;
		break;
	}
	case ECommand::GET_POSITION: {
		PositionReply* position_reply = new PositionReply();
		position_reply->setData(mPosition, 6);
		reply = position_reply;
		break;
	}
	case ECommand::GET_STATUS: {
		StatusReply* status_reply = new StatusReply();
		status_reply->setInitDone(true);
		status_reply->setBlocked(false);
		status_reply->setRunning(mMoving);
		status_reply->setSlewMode(mType);
		status_reply->setSpeedMode(mSpeed);
		status_reply->setDirection(mDir);
		reply = status_reply;
		break;
	}
	case ECommand::GET_HIGH_SPEED_RATIO: {
		Logger::verbose("Motor[%d]: GET_HIGH_SPEED_RATIO", mAxis);
		DataReply* data_reply = new DataReply();
		data_reply->setData(HIGH_SPEED_RATIO, 2);
		reply = data_reply;
		break;
	}
	case ECommand::GET_SIDERAL_PERIOD: {
		Logger::verbose("Motor[%d]: GET_SIDERAL_PERIOD: %d ticks", mAxis,
				mSideralStepPeriod);
		DataReply* data_reply = new DataReply();
		data_reply->setData(mSideralStepPeriod, 6);
		reply = data_reply;
		break;
	}
	case ECommand::GET_AXIS_POSITION: {
		Logger::verbose("Motor[%d]: GET_AXIS_POSITION", mAxis);
		PositionReply* position_reply = new PositionReply();
		position_reply->setData(mPosition, 6);
		reply = position_reply;
		break;
	}
	case ECommand::EXT_SET: {
		Logger::verbose("Motor[%d]: EXT_SET", mAxis);
		ErrorReply* error_reply = new ErrorReply();
		error_reply->setError(ErrorReply::EErrorCode::Unknown_Command);
		reply = error_reply;
		break;
	}
	case ECommand::EXT_GET: {
		Logger::verbose("Motor[%d]: EXT_GET", mAxis);
		GetExtended* ext = (GetExtended*) cmd;
		if (ext->getType() == GetExtended::EType::STATUS_EX) {
			StatusExReply* status_reply = new StatusExReply();
			status_reply->setDualEncSupport(false);
			status_reply->setEQAZModeSupport(false);
			status_reply->setHasPolarLed(false);
			status_reply->setOriginalIdxPosSupport(false);
			status_reply->setPPECSupport(false);
			status_reply->setPecTracking(false);
			status_reply->setPecTraining(false);
			status_reply->setTorqueSelectionSupport(false);
			status_reply->setTwoAxesSeparate(false);
			reply = status_reply;
		} else {
			ErrorReply* error_reply = new ErrorReply();
			error_reply->setError(ErrorReply::EErrorCode::Unknown_Command);
			reply = error_reply;
		}
		break;
	}
	default: {
		ErrorReply* error_reply = new ErrorReply();
		error_reply->setError(ErrorReply::EErrorCode::Unknown_Command);
		reply = error_reply;
		break;
	}
	}

	return reply;
}
