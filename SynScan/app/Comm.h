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

#ifndef SERIALCOMM_H_
#define SERIALCOMM_H_

#include <Stream.h>
#include "Defines.h"
#include "Commands.h"

class Comm;

class ICommListener {
public:
	virtual ~ICommListener() {}

	virtual void onCommandReceived(Comm* comm, Command* cmd) = 0;
};

class Comm {
protected:
	ICommListener* mListener = NULL;

public:
	virtual ~Comm() {}

	void setListener(ICommListener* listener) {
		mListener = listener;
	}

	virtual bool sendReply(const Reply* reply) = 0;
};

#endif /* SERIALCOMM_H_ */
