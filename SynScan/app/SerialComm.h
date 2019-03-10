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
