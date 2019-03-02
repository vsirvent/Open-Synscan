/*
 * MainApp.h
 *
 *  Created on: 30 dic. 2018
 *      Author: Vicen
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
