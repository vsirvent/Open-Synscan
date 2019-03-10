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

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "Defines.h"

//------Synscan replies-------//

class Reply {
protected:
	static void toHex(u32 data, char* output, int len) {
		u8 b6, b5, b4, b3, b2, b1;
		b6 = (data & 0xF00000) >> 20;
		b5 = (data & 0xF0000) >> 16;
		b4 = (data & 0xF000) >> 12;
		b3 = (data & 0xF00) >> 8;
		b2 = (data & 0xF0) >> 4;
		b1 = (data & 0xF);

		if (len == 6) {
			sprintf(output, "%X%X%X%X%X%X", b2, b1, b4, b3, b6, b5);
		} else if (len == 4) {
			sprintf(output, "%X%X%X%X", b2, b1, b4, b3);
		} else if (len == 2) {
			sprintf(output, "%X%X", b2, b1);
		}
	}

public:
	Reply() {
	}
	virtual ~Reply() {
	}

	virtual String toString() const = 0;
};

class EmptyReply: public Reply {
public:
	String toString() const override {
		return "=\r";
	}
};

class PositionReply: public Reply {
private:
	char mData[7];
public:
	PositionReply() {
		memset(mData, 0, 7);
	}

	void setData(u32 data, int len) {
		toHex(data, mData, len);
	}

	String toString() const override {
		char ret[11];
		sprintf(ret, "=%s\r", mData);
		return ret;
	}
};

class DataReply: public Reply {
private:
	char mData[7];
public:
	DataReply() {
		memset(mData, 0, 7);
	}

	void setData(u32 data, int len) {
		toHex(data, mData, len);
	}

	String toString() const override {
		char ret[11];
		sprintf(ret, "=%s\r", mData);
		return ret;
	}
};

class VersionReply: public Reply {
private:
	char mVersion[7];
public:
	VersionReply() {
		memset(mVersion, 0, 7);
	}

	void setVersion(u8 major, u8 minor, u8 micro, u8 patch) {
		sprintf(mVersion, "%X%X%X%X00", major, minor, micro, patch);
	}

	String toString() const override {
		char ret[11];
		sprintf(ret, "=%s\r", mVersion);
		return ret;
	}
};

class ErrorReply: public Reply {
public:
	enum class EErrorCode {
		Unknown_Command = 0,
		Command_Length_Error = 1,
		Motor_not_Stopped = 2,
		Invalid_Character = 3,
		Not_Initialized = 4,
		Driver_Sleeping = 5,
		PEC_Training_is_running = 7,
		No_Valid_PEC_data = 8
	};
private:
	EErrorCode mCode = EErrorCode::Unknown_Command;
public:
	ErrorReply() {
	}

	ErrorReply(EErrorCode code) {
		mCode = code;
	}

	void setError(EErrorCode code) {
		mCode = code;
	}

	String toString() const override {
		char ret[11];
		sprintf(ret, "!%X\r", (int) mCode);
		return ret;
	}
};

class StatusReply: public Reply {
private:
	/*
	 B0:0: 1=Tracking 0=Goto
	 B0:1: 1=CCW 0=CW
	 B0:2: 1=Fast 0=Slow
	 B1:0: 1=Running 0=Stopped
	 B1:1: 1=Blocked 0=Normal
	 B2:0: 0 = Not Init 1 = Init done
	 B2:1: 1 = Level switch on
	 */
	u8 b0 = 0;
	u8 b1 = 0;
	u8 b2 = 0;

public:
	StatusReply() {
	}

	void setSlewMode(ESlewType mode) {
		if (mode == ESlewType::GOTO) {
			b0 &= ~0x01;
		} else if (mode == ESlewType::TRACKING) {
			b0 |= 0x01;
		}
	}

	void setDirection(EDirection dir) {
		if (dir == EDirection::CW) {
			b0 &= ~0x02;
		} else if (dir == EDirection::CCW) {
			b0 |= 0x02;
		}
	}
	void setSpeedMode(ESpeed speed) {
		if (speed == ESpeed::FAST) {
			b0 |= 0x04;
		} else if (speed == ESpeed::SLOW) {
			b0 &= ~0x04;
		}
	}

	void setRunning(bool running) {
		if (running) {
			b1 |= 0x01;
		} else {
			b1 &= ~0x01;
		}
	}

	void setBlocked(bool blocked) {
		if (blocked) {
			b1 |= 0x02;
		} else {
			b1 &= ~0x02;
		}
	}

	void setInitDone(bool init) {
		if (init) {
			b2 |= 0x01;
		} else {
			b2 &= ~0x01;
		}
	}

	String toString() const override {
		char ret[11];
		sprintf(ret, "=%X%X%X\r", b0, b1, b2);
		return ret;
	}
};

class StatusExReply: public Reply {
private:

#define PEC_TRAINING 1
#define PEC_TRACKING 2
#define SUPPORT_DUAL_ENC 1
#define SUPPORT_PPEC 2
#define SUPPORT_ORIGINAL_POSITION_IDX 4
#define SUPPORT_EQAZ_MODE 8
#define HAS_POLAR_LED 1
#define TWO_AXES_SEPARATE 2
#define SUPPORT_TORQUE 3

	u8 b0 = 0;
	u8 b1 = 0;
	u8 b2 = 0;

public:
	StatusExReply() {
	}

	void setPecTraining(bool enabled) {
		if (enabled) {
			b0 |= PEC_TRAINING;
		} else {
			b0 &= ~PEC_TRAINING;
		}
	}

	void setPecTracking(bool enabled) {
		if (enabled) {
			b0 |= PEC_TRACKING;
		} else {
			b0 &= ~PEC_TRACKING;
		}
	}

	void setDualEncSupport(bool enabled) {
		if (enabled) {
			b1 |= SUPPORT_DUAL_ENC;
		} else {
			b1 &= ~SUPPORT_DUAL_ENC;
		}
	}

	void setPPECSupport(bool enabled) {
		if (enabled) {
			b1 |= SUPPORT_PPEC;
		} else {
			b1 &= ~SUPPORT_PPEC;
		}
	}

	void setOriginalIdxPosSupport(bool enabled) {
		if (enabled) {
			b1 |= SUPPORT_ORIGINAL_POSITION_IDX;
		} else {
			b1 &= ~SUPPORT_ORIGINAL_POSITION_IDX;
		}
	}

	void setEQAZModeSupport(bool enabled) {
		if (enabled) {
			b1 |= SUPPORT_EQAZ_MODE;
		} else {
			b1 &= ~SUPPORT_EQAZ_MODE;
		}
	}

	void setHasPolarLed(bool enabled) {
		if (enabled) {
			b2 |= HAS_POLAR_LED;
		} else {
			b2 &= ~HAS_POLAR_LED;
		}
	}

	void setTwoAxesSeparate(bool enabled) {
		if (enabled) {
			b2 |= TWO_AXES_SEPARATE;
		} else {
			b2 &= ~TWO_AXES_SEPARATE;
		}
	}

	void setTorqueSelectionSupport(bool enabled) {
		if (enabled) {
			b2 |= SUPPORT_TORQUE;
		} else {
			b2 &= ~SUPPORT_TORQUE;
		}
	}

	String toString() const override {
		char ret[11];
		sprintf(ret, "=%X%X%X000\r", b0, b1, b2);
		return ret;
	}
};

//------Synscan commands-------//

enum class ECommand {
	SET_POSITION,
	INIT_DONE,
	SET_MOTION_MODE,
	SET_GOTO_TARGET,
	SET_GOTO_TARGET_INCREMENT,
	GET_GOTO_TARGET,
	SET_STEP_PERIOD,
	GET_STEP_PERIOD,
	SET_BREAK_POINT_INCREMENT,
	START_MOTION,
	STOP_MOTION,
	INSTANT_STOP,
	SET_SWITCH_ONOFF,
	SET_AUTOGUIDE_SPEED,
	RUN_BOOTLOADER_MODE,
	SET_POLAR_SCOPE_LED_BRIGHTNESS,
	GET_COUNTS_PER_REVOLUTION,
	GET_PEC_PERIOD,
	GET_TIMER_FREQ,
	GET_POSITION,
	GET_STATUS,
	GET_HIGH_SPEED_RATIO,
	GET_SIDERAL_PERIOD,
	GET_AXIS_POSITION,
	GET_VERSION,
	EXT_SET,
	EXT_GET,
	NONE
};

class Command {
protected:
	ECommand mCmd = ECommand::NONE;
	EAxis mAxis = EAxis::AXIS_NONE;
	bool mInit = false;

protected:
	Command() {
	}

	static EAxis parseAxis(char axis) {
		switch (axis) {
		case '1':
			return EAxis::AXIS_RA;
		case '2':
			return EAxis::AXIS_DEC;
		case '3':
			return EAxis::AXIS_BOTH;
		default:
			return EAxis::AXIS_NONE;
		}
	}

	static u32 charToHex(char c) {
		switch (c) {
		case '0':
			return 0x00;
		case '1':
			return 0x01;
		case '2':
			return 0x02;
		case '3':
			return 0x03;
		case '4':
			return 0x04;
		case '5':
			return 0x05;
		case '6':
			return 0x06;
		case '7':
			return 0x07;
		case '8':
			return 0x08;
		case '9':
			return 0x09;
		case 'A':
			return 0x0A;
		case 'B':
			return 0x0B;
		case 'C':
			return 0x0C;
		case 'D':
			return 0x0D;
		case 'E':
			return 0x0E;
		case 'F':
			return 0x0F;
		default:
			Logger::error("charToHex: Bad char %c", c);
			return 0x00;
		}
	}

	static u32 parseHexUlong(const char* data, int len) {
		u32 ret = 0;
		if (len == 6) {
			ret |= (charToHex(data[1]));
			ret |= (charToHex(data[0]) <<  4);
			ret |= (charToHex(data[3]) <<  8);
			ret |= (charToHex(data[2]) << 12);
			ret |= (charToHex(data[5]) << 16);
			ret |= (charToHex(data[4]) << 20);
		} else if (len == 4) {
			ret |= (charToHex(data[1]));
			ret |= (charToHex(data[0]) <<  4);
			ret |= (charToHex(data[3]) <<  8);
			ret |= (charToHex(data[2]) << 12);
		} else if (len == 2) {
			ret |= (charToHex(data[1]));
			ret |= (charToHex(data[0]) << 4);
		} else if (len == 1) {
			ret = (charToHex(data[0]));
		}
		return ret;
	}

	static int parseHexLong(const char* data, int len) {
		int ret = 0;
		if (len == 6) {
			ret |= ((int)charToHex(data[1]));
			ret |= ((int)charToHex(data[0]) <<  4);
			ret |= ((int)charToHex(data[3]) <<  8);
			ret |= ((int)charToHex(data[2]) << 12);
			ret |= ((int)charToHex(data[5]) << 16);
			ret |= ((int)charToHex(data[4]) << 20);
		} else if (len == 4) {
			ret |= ((int)charToHex(data[1]));
			ret |= ((int)charToHex(data[0]) <<  4);
			ret |= ((int)charToHex(data[3]) <<  8);
			ret |= ((int)charToHex(data[2]) << 12);
		} else if (len == 2) {
			ret |= ((int)charToHex(data[1]));
			ret |= ((int)charToHex(data[0]) << 4);
		} else if (len == 1) {
			ret = ((int)charToHex(data[0]));
		}
		return ret;
	}

public:

	virtual ~Command() {
	}

	virtual bool Parse(const char* data, int len) = 0;

	EAxis getAxis() const {
		return mAxis;
	}

	void setAxis(EAxis axis) {
		mAxis = axis;
	}

	ECommand getCmd() const {
		return mCmd;
	}

	void setCmd(ECommand cmd) {
		mCmd = cmd;
	}

	bool isInit() const {
		return mInit;
	}
};

class SetPositionCommand: public Command {
private:
	static const int MSG_SIZE = 9;
	int mPosition = 0;

public:

	SetPositionCommand() {
		mCmd = ECommand::SET_POSITION;
	}

	int GetPosition() const {
		return mPosition;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'E') {
					mAxis = parseAxis(data[2]);
					mPosition = parseHexLong(data + 3, 6);
					Logger::verbose(
							"SetPositionCommand: Parse => Axis: %d, Pos: %X",
							mAxis, mPosition);
					mInit = true;
					ret = true;
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class SetGotoTarget: public Command {
private:
	static const int MSG_SIZE = 9;
	int mPosition = 0;

public:

	SetGotoTarget() {
		mCmd = ECommand::SET_GOTO_TARGET;
	}

	int GetPosition() const {
		return mPosition;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'S') {
					mAxis = parseAxis(data[2]);
					mPosition = parseHexLong(data + 3, 6);
					Logger::verbose("SetGotoTarget: Parse => Axis: %d, Pos: %X",
							mAxis, mPosition);
					mInit = true;
					ret = true;
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class SetGotoTargetIncrement: public Command {
private:
	static const int MSG_SIZE = 9;
	int mIncrement = 0;

public:

	SetGotoTargetIncrement() {
		mCmd = ECommand::SET_GOTO_TARGET_INCREMENT;
	}

	int GetIncrement() const {
		return mIncrement;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'H') {
					mAxis = parseAxis(data[2]);
					mIncrement = parseHexLong(data + 3, 6);
					Logger::notice("SetGotoTargetIncrement: 0X%s: Parse => Axis: %d, Increment: %d",
							data + 3, mAxis, mIncrement);
					mInit = true;
					ret = true;
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class SetBreakPointIncrement: public Command {
private:
	static const int MSG_SIZE = 9;
	int mIncrement = 0;

public:

	SetBreakPointIncrement() {
		mCmd = ECommand::SET_BREAK_POINT_INCREMENT;
	}

	int GetIncrement() const {
		return mIncrement;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'M') {
					mAxis = parseAxis(data[2]);
					mIncrement = parseHexLong(data + 3, 6);
					Logger::verbose("SetBreakPointIncrement: Parse => Axis: %d, Increment: %d",
							mAxis, mIncrement);
					mInit = true;
					ret = true;
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class SetStepPeriod: public Command {
private:
	static const int MSG_SIZE = 9;
	int mPeriod = 0;

public:

	SetStepPeriod() {
		mCmd = ECommand::SET_STEP_PERIOD;
	}

	int GetPeriod() const {
		return mPeriod;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'I') {
					mAxis = parseAxis(data[2]);
					mPeriod = parseHexLong(data + 3, 6);
					Logger::verbose("SetStepPeriod: Parse => Axis: %d, Pos: %X",
							mAxis, mPeriod);
					mInit = true;
					ret = true;
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class InitializationDone: public Command {
private:
	static const int MSG_SIZE = 3;

public:

	InitializationDone() {
		mCmd = ECommand::INIT_DONE;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'F') {
					mAxis = parseAxis(data[2]);
					Logger::verbose("InitializationDone: Parse => Axis: %d", mAxis);
					mInit = true;
					ret = true;
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class SetSwitch: public Command {
private:
	static const int MSG_SIZE = 4;
	bool mActive = false;
public:

	SetSwitch() {
		mCmd = ECommand::SET_SWITCH_ONOFF;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'O') {
					mAxis = parseAxis(data[2]);
					mActive = parseHexUlong(data + 3, 1);
					Logger::verbose(
							"SetAuxSwitch: Parse => Axis: %d, Active: %d",
							mAxis, mActive);
					mInit = true;
					ret = true;
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}

	bool isActive() const {
		return mActive;
	}
};

class SetAutoGuideSpeed: public Command {
private:
	static const int MSG_SIZE = 4;
	u32 mSpeed = 1000; //1000 = x1
public:

	SetAutoGuideSpeed() {
		mCmd = ECommand::SET_AUTOGUIDE_SPEED;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'P') {
					mAxis = parseAxis(data[2]);
					u32 val = parseHexUlong(data + 3, 1);
					switch (val) {
					case 0:
						mSpeed = 1000;
						break;
					case 1:
						mSpeed = 750;
						break;
					case 2:
						mSpeed = 500;
						break;
					case 3:
						mSpeed = 250;
						break;
					case 4:
						mSpeed = 125;
						break;
					default:
						mSpeed = 1000;
						break;
					}
					Logger::verbose(
							"SetAutoGuideSpeed: Parse => Axis: %d, val: %d, speed: %d",
							mAxis, val, mSpeed);
					mInit = true;
					ret = true;
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}

	u32 getSpeed() const {
		return mSpeed;
	}
};
class StartMotion: public Command {
private:
	static const int MSG_SIZE = 3;
public:

	StartMotion() {
		mCmd = ECommand::START_MOTION;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'J') {
					mAxis = parseAxis(data[2]);
					Logger::verbose("StartMotion: Parse => Axis: %d", mAxis);
					mInit = true;
					ret = true;
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class StopMotion: public Command {
private:
	static const int MSG_SIZE = 3;

public:

	StopMotion() {
		mCmd = ECommand::STOP_MOTION;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'K') {
					mAxis = parseAxis(data[2]);
					Logger::verbose("StopMotion: Parse => Axis: %d", mAxis);
					mInit = true;
					ret = true;
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class InstantStop: public Command {
private:
	static const int MSG_SIZE = 3;
public:

	InstantStop() {
		mCmd = ECommand::INSTANT_STOP;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'L') {
					mAxis = parseAxis(data[2]);
					Logger::verbose("InstantStop: Parse => Axis: %d", mAxis);
					mInit = true;
					ret = true;
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};
class SetMotionMode: public Command {

private:
	static const int MSG_SIZE = 5;
	ESlewType mType = ESlewType::NONE;
	ESpeed mSpeed = ESpeed::NONE;
	EDirection mDir = EDirection::NONE;

public:

	SetMotionMode() {
		mCmd = ECommand::SET_MOTION_MODE;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		Logger::verbose("Try parse motion mode");
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'G') {
					mAxis = parseAxis(data[2]);
					char mode = data[3];
					switch (mode) {
					case '0': {
						mType = ESlewType::GOTO;
						mSpeed = ESpeed::FAST;
						mInit = true;
						ret = true;
					}
						break;
					case '1': {
						mType = ESlewType::TRACKING;
						mSpeed = ESpeed::SLOW;
						mInit = true;
						ret = true;
					}
						break;
					case '2': {
						mType = ESlewType::GOTO;
						mSpeed = ESpeed::SLOW;
						mInit = true;
						ret = true;
					}
						break;
					case '3': {
						mType = ESlewType::TRACKING;
						mSpeed = ESpeed::FAST;
						mInit = true;
						ret = true;
					}
						break;
					default:
						Logger::error("SetMotionMode: Bad mode %d", mode);
					}

					char dir = data[4] & 0x01;
					if (dir) {
						mDir = EDirection::CCW;
					}else{
						mDir = EDirection::CW;
					}
					if (mInit) {
						Logger::verbose(
								"SetMotionMode: Parse axis: %d, mode: %d, speed: %d, dir: %d",
								mAxis, mType, mSpeed, mDir);
					}
				}else{
					Logger::error("Bada header: [%c]", header);
				}
			}else{
				Logger::error("Bad data[0]: [%c]", data[0]);
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}

	ESpeed getSpeed() const {
		return mSpeed;
	}

	void setSpeed(ESpeed speed = ESpeed::NONE) {
		mSpeed = speed;
	}

	ESlewType getType() const {
		return mType;
	}

	void setType(ESlewType type = ESlewType::NONE) {
		mType = type;
	}

	EDirection getDir() const {
		return mDir;
	}
};

class GetVersion: public Command {

private:
	static const int MSG_SIZE = 3;

public:

	GetVersion() {
		mCmd = ECommand::GET_VERSION;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'e') {
					mAxis = parseAxis(data[2]);
					mInit = true;
					ret = true;
					Logger::verbose("GetVersion: Parse axis: %d", mAxis);
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class GetCountsPerRev: public Command {

private:
	static const int MSG_SIZE = 3;

public:

	GetCountsPerRev() {
		mCmd = ECommand::GET_COUNTS_PER_REVOLUTION;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'a') {
					mAxis = parseAxis(data[2]);
					mInit = true;
					ret = true;
					Logger::verbose("GetCountsPerRev: Parse axis: %d", mAxis);
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class GetTimerFreq: public Command {

private:
	static const int MSG_SIZE = 3;

public:

	GetTimerFreq() {
		mCmd = ECommand::GET_TIMER_FREQ;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'b') {
					mAxis = parseAxis(data[2]);
					mInit = true;
					ret = true;
					Logger::verbose("GetTimerFreq: Parse axis: %d", mAxis);
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class GetGotoTargetPosition: public Command {

private:
	static const int MSG_SIZE = 3;

public:

	GetGotoTargetPosition() {
		mCmd = ECommand::GET_GOTO_TARGET;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'h') {
					mAxis = parseAxis(data[2]);
					mInit = true;
					ret = true;
					Logger::verbose("GetGotoTargetPosition: Parse axis: %d", mAxis);
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class GetStepPeriod: public Command {

private:
	static const int MSG_SIZE = 3;

public:

	GetStepPeriod() {
		mCmd = ECommand::GET_STEP_PERIOD;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'i') {
					mAxis = parseAxis(data[2]);
					mInit = true;
					ret = true;
					Logger::verbose("GetStepPeriod: Parse axis: %d", mAxis);
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class GetPosition: public Command {

private:
	static const int MSG_SIZE = 3;

public:

	GetPosition() {
		mCmd = ECommand::GET_POSITION;
	}

	bool
	Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'j') {
					mAxis = parseAxis(data[2]);
					mInit = true;
					ret = true;
					Logger::verbose("GetPosition: Parse axis: %d", mAxis);
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class GetStatus: public Command {

private:
	static const int MSG_SIZE = 3;

public:

	GetStatus() {
		mCmd = ECommand::GET_STATUS;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'f') {
					mAxis = parseAxis(data[2]);
					mInit = true;
					ret = true;
					Logger::verbose("GetStatus: Parse axis: %d", mAxis);
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class GetHighSpeedRatio: public Command {

private:
	static const int MSG_SIZE = 3;

public:

	GetHighSpeedRatio() {
		mCmd = ECommand::GET_HIGH_SPEED_RATIO;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'g') {
					mAxis = parseAxis(data[2]);
					mInit = true;
					ret = true;
					Logger::verbose("GetHighSpeedRatio: Parse axis: %d", mAxis);
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class GetSideralPeriod: public Command {

private:
	static const int MSG_SIZE = 3;

public:

	GetSideralPeriod() {
		mCmd = ECommand::GET_SIDERAL_PERIOD;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'D') {
					mAxis = parseAxis(data[2]);
					mInit = true;
					ret = true;
					Logger::verbose("GetSideralPeriod: Parse axis: %d", mAxis);
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class GetPecPeriod: public Command {

private:
	static const int MSG_SIZE = 3;

public:

	GetPecPeriod() {
		mCmd = ECommand::GET_PEC_PERIOD;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 's') {
					mAxis = parseAxis(data[2]);
					mInit = true;
					ret = true;
					Logger::verbose("GetPecPeriod: Parse axis: %d", mAxis);
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class GetAxisPosition: public Command {

private:
	static const int MSG_SIZE = 3;

public:

	GetAxisPosition() {
		mCmd = ECommand::GET_AXIS_POSITION;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'd') {
					mAxis = parseAxis(data[2]);
					mInit = true;
					ret = true;
					Logger::verbose("GetAxisPosition: Parse axis: %d", mAxis);
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}
};

class GetExtended: public Command {
public:
	enum class EType {
		POSITION, STATUS_EX, NONE
	};
private:
	static const int MSG_SIZE = 9;
	EType mType = EType::NONE;

public:

	GetExtended() {
		mCmd = ECommand::EXT_GET;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'q') {
					mAxis = parseAxis(data[2]);
					mInit = true;
					u32 id = parseHexUlong(data + 3, 6);
					if (id == 1) {
						mType = EType::STATUS_EX;
					} else if (id == 0) {
						mType = EType::POSITION;
					}
					ret = true;
					Logger::verbose("GetExtended: Parse axis: %d, id: %d",
							mAxis, id);
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}

	EType getType() const {
		return mType;
	}
};

class SetPolarLedBrigth: public Command {

private:
	static const int MSG_SIZE = 5;
	u16 mValue = 0;

public:

	SetPolarLedBrigth() {
		mCmd = ECommand::SET_POLAR_SCOPE_LED_BRIGHTNESS;
	}

	bool Parse(const char* data, int len) {
		bool ret = false;
		if (len == MSG_SIZE) {
			if (data[0] == ':') {
				char header = data[1];
				if (header == 'V') {
					mAxis = parseAxis(data[2]);
					mValue = parseHexUlong(data + 3, 2);
					mInit = true;
					ret = true;
					Logger::verbose(
							"SetPolarLedBrigth: Parse axis: %d, brightness: %d",
							mAxis, mValue);
				}
			}
		} else {
			Logger::error("Bad message size %d", len);
		}
		return ret;
	}

	u16 getValue() const {
		return mValue;
	}
};

class CommandFactory {
private:
	CommandFactory(){}
public:
	static Command* parseData(const char* data, int len);
};

#endif /* COMMANDS_H_ */
