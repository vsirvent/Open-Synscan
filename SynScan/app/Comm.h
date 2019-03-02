/*
 * SerialComm.h
 *
 *  Created on: 29 dic. 2018
 *      Author: Vicen
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
