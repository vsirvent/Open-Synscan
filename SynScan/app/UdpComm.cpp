/*
 * UdpUdpComm.cpp
 *
 *  Created on: Dec 31, 2018
 *      Author: vsirvent
 */

#include "UdpComm.h"
#include <Logger.h>


UdpComm::UdpComm() {
	mUdp = new UdpConnection(UdpConnectionDataDelegate(&UdpComm::onUdpRx, this));
	mUdp->listen(Defines::SYNSCAN_PORT);
}

UdpComm::~UdpComm() {
	delete mUdp;
}

void
UdpComm::onUdpRx(UdpConnection& connection, char *data, int size, IPAddress remoteIP, uint16_t remotePort) {
	bool ok = false;
	for (int i = 0; i < size; ++i) {
		if (data[i] == '\r') {
			data[i] = 0;
			size = i;
			ok = true;
			break;
		}
	}
	Logger::verbose("UDP <= [%s][%d]: %s", ok?"OK":"ERR", size, data);
	if (data != NULL && size > 1) {
		Command* cmd = CommandFactory::parseData(data, size);
		if (cmd != NULL) {
			mRemoteIP = remoteIP;
			mRemotePort = remotePort;
			if (mListener != NULL) {
				mListener->onCommandReceived(this, cmd);
			}
			delete cmd;
		}
	}
}

bool
UdpComm::sendReply(const Reply* reply) {
	bool ret = false;
	if (mRemotePort != 0) {
		String str = reply->toString();
		Logger::verbose("UDP => %s", str.c_str());
		mUdp->sendStringTo(mRemoteIP, mRemotePort, str);
		ret = true;
	}
	return ret;
}




