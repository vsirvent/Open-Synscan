/*
 * SerialComm.h
 *
 *  Created on: Dec 31, 2018
 *      Author: vsirvent
 */

#ifndef APP_SERIALCOMM_H_
#define APP_SERIALCOMM_H_

#include "Comm.h"

class SerialComm: public Comm {

	static const int MAX_MSG_SIZE = 10;
	HardwareSerial* mSerial = NULL;
	int mSize = 0;
	char mData[MAX_MSG_SIZE + 1];

	void onCharRx(Stream &source, char arrivedChar, uint16_t availableCharsCount);

public:
	SerialComm();
	virtual ~SerialComm();

	bool sendReply(const Reply* reply);
};

#endif /* APP_SERIALCOMM_H_ */
