/*
 * MainApp.cpp
 *
 *  Created on: 30 dic. 2018
 *      Author: Vicen
 */

#include "MainApp.h"
#include "TickTimer.h"

MainApp::MainApp() {
	Logger::notice("MainApp: Initializing objects...");
	mRaMotor = new Motor(EAxis::AXIS_RA);
	mDecMotor = new Motor(EAxis::AXIS_DEC);
	mUdpComm = new UdpComm();
	//mSerialComm = new SerialComm();
	mOneSecTimer = new Timer();

	Logger::notice("MainApp: Setting callbacks...");
	AppTimer.addListener(mRaMotor);
	AppTimer.addListener(mDecMotor);

	mUdpComm->setListener(this);
	//mSerialComm->setListener(this);

	AppTimer.setIntervalTicks((float)TickTimer::TICK_FREQ/(float)Defines::TIMER_FREQ);

	Logger::notice("MainApp: Start timers...");
	AppTimer.start(true);
	mOneSecTimer->initializeMs(1000, TimerDelegate(&MainApp::oneSecTick, this)).start();

	Logger::notice("MainApp: Init done");
}

MainApp::~MainApp() {
	AppTimer.stop();
	mOneSecTimer->stop();
	delete mOneSecTimer;
	delete mRaMotor;
	delete mDecMotor;
	delete mUdpComm;
	//delete mSerialComm;
}

void
MainApp::oneSecTick() {
	mRaMotor->printInfo();
	mDecMotor->printInfo();
}

void
MainApp::onCommandReceived(Comm* comm, Command* cmd) {
	if (cmd != NULL) {
		Reply* reply = NULL;
		if (cmd->getAxis() == EAxis::AXIS_RA) {
			reply = mRaMotor->processCommand(cmd);
		} else if (cmd->getAxis() == EAxis::AXIS_DEC) {
			reply = mDecMotor->processCommand(cmd);
		} else if (cmd->getAxis() == EAxis::AXIS_BOTH) {
			Logger::notice("onCommandReceived: Command %d for both axis", cmd->getCmd());
			reply = mRaMotor->processCommand(cmd);
			Reply* reply2 = mDecMotor->processCommand(cmd);
			delete reply2;
		}
		if (reply != NULL) {
			comm->sendReply(reply);
			delete reply;
		}
	}
}
