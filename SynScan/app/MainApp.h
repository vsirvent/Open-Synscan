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

#ifndef MAINAPP_H_
#define MAINAPP_H_

#include "Comm.h"
#include "Motor.h"
#include "UdpComm.h"
#include "SerialComm.h"

class MainApp: public ICommListener {
private:

	UdpComm* mUdpComm = NULL;
	SerialComm* mSerialComm = NULL;
	Motor* mRaMotor = NULL;
	Motor* mDecMotor = NULL;
	Timer* mOneSecTimer = NULL;

public:
	MainApp();
	virtual ~MainApp();

	void onCommandReceived(Comm* comm, Command* cmd);
	void oneSecTick();
};

#endif /* MAINAPP_H_ */
