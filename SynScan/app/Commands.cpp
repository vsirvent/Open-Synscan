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

#include "Commands.h"

Command*
CommandFactory::parseData(const char* data, int len) {

	Command* cmd = NULL;
	if (data[0] == PulseGuideCmd::PULSE_PKT_START) {
		//Pulse guide command
		cmd = new PulseGuideCmd();
		cmd->Parse(data, len);
	}else{
		//Synscan command
		int pos = 0;
		bool found = false;
		while (data[pos] != 0 && data[pos] != '\r') {
			if (data[pos] == ':') {
				found = true;
				break;
			} else {
				pos++;
			}
		}

		if (found && (len - pos) > 1) {
			char c = data[pos + 1];
			switch (c) {
			case 'E': {
				cmd = new SetPositionCommand();
				break;
			}
			case 'F': {
				cmd = new InitializationDone();
				break;
			}
			case 'G': {
				cmd = new SetMotionMode();
				break;
			}
			case 'S': {
				cmd = new SetGotoTarget();
				break;
			}
			case 'H': {
				cmd = new SetGotoTargetIncrement();
				break;
			}
			case 'M': {
				cmd = new SetBreakPointIncrement();
				break;
			}
			case 'I': {
				cmd = new SetStepPeriod();
				break;
			}
			case 'J': {
				cmd = new StartMotion();
				break;
			}
			case 'K': {
				cmd = new StopMotion();
				break;
			}
			case 'L': {
				cmd = new InstantStop();
				break;
			}
			case 'O': {
				cmd = new SetSwitch();
				break;
			}
			case 'P': {
				cmd = new SetAutoGuideSpeed();
				break;
			}
			case 'V': {
				cmd = new SetPolarLedBrigth();
				break;
			}
			case 'a': {
				cmd = new GetCountsPerRev();
				break;
			}
			case 'b': {
				cmd = new GetTimerFreq();
				break;
			}
			case 'h': {
				cmd = new GetGotoTargetPosition();
				break;
			}
			case 'i': {
				cmd = new GetStepPeriod();
				break;
			}
			case 'j': {
				cmd = new GetPosition();
				break;
			}
			case 'f': {
				cmd = new GetStatus();
				break;
			}
			case 'g': {
				cmd = new GetHighSpeedRatio();
				break;
			}
			case 'D': {
				cmd = new GetSideralPeriod();
				break;
			}
			case 'd': {
				cmd = new GetAxisPosition();
				break;
			}
			case 'e': {
				cmd = new GetVersion();
				break;
			}
			case 's': {
				cmd = new GetPecPeriod();
				break;
			}
			case 'q': {
				cmd = new GetExtended();
				break;
			}
			default: {
				Logger::error("CommandFactory: Unknown cmd %d", c);
				break;
			}
			}
			if (cmd != NULL) {
				if (!cmd->Parse(data + pos, len - pos)) {
					Logger::error("CommandFactory: Can't parse [%s]", data);
				}
			}
		} else {
			Logger::error("CommandFactory: Bad data => [%s]", data);
		}
	}
	return cmd;
}

