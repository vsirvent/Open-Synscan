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

#include "SerialComm.h"
#include <Logger.h>


SerialComm::SerialComm() {
	mSerial = new HardwareSerial(UART_ID_1);
	mSerial->begin(9600);
	mSerial->setCallback(StreamDataReceivedDelegate(&SerialComm::onCharRx, this));
}

SerialComm::~SerialComm() {
}

void
SerialComm::onCharRx(Stream &source, char arrivedChar, uint16_t availableCharsCount) {
	if (mSize < MAX_MSG_SIZE) {
		if (arrivedChar == '\r') {
			mData[mSize] = 0;
			Logger::verbose("SERIAL <= [%d]: %s", mSize, mData);
			Command* cmd = CommandFactory::parseData(mData, mSize);
			if (cmd != NULL) {
				if (mListener != NULL) {
					mListener->onCommandReceived(this, cmd);
				}
				delete cmd;
			}
			mSize = 0;
		}else{
			mData[mSize++] = arrivedChar;
		}
	}
}

bool
SerialComm::sendReply(const Reply* reply) {
	bool ret = true;
	String str = reply->toString();
	Logger::verbose("SERIAL => %s", str.c_str());
	int size = str.length();
	for (int i = 0; i < size; ++i) {
		if (!mSerial->write(str[i])) {
			Logger::error("SerialComm::sendReply: Send error");
			ret = false;
			break;
		}
	}
	return ret;
}
