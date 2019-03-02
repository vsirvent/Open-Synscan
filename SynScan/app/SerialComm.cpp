/*
 * SerialComm.cpp
 *
 *  Created on: Dec 31, 2018
 *      Author: vsirvent
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
