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

#include "UdpComm.h"
#include <Logger.h>


UdpComm::UdpComm() {
	mUdp = new UdpConnection(UdpConnectionDataDelegate(&UdpComm::onUdpRx, this));
	mUdp->listen(Defines::SYNSCAN_PORT);
	mPulseUdp = new UdpConnection(UdpConnectionDataDelegate(&UdpComm::onPulseUdpRx, this));
	mPulseUdp->listen(Defines::PULSE_GUIDE_PORT);
	mDiscoveryData[0] = Defines::DISCOVERY_PT;
	mBroadcast = IPAddress(IPADDR_BROADCAST);
}

UdpComm::~UdpComm() {
	delete mUdp;
	delete mPulseUdp;
}

void
UdpComm::onPulseUdpRx(UdpConnection& connection, char *data, int size, IPAddress remoteIP, uint16_t remotePort) {
	bool ok = false;
	if (data != NULL) {
		Command* cmd = CommandFactory::parseData(data, size);
		if (cmd != NULL) {
			if (mListener != NULL) {
				mListener->onCommandReceived(this, cmd);
			}
			delete cmd;
		}
	}
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
	//Logger::verbose("UDP <= [%s][%d]: %s", ok?"OK":"ERR", size, data);
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

void
UdpComm::sendDiscovery() {
	mUdp->sendTo(mBroadcast, Defines::PULSE_DISCOVERY_PORT, (const char*)mDiscoveryData, 2);
}

bool
UdpComm::sendReply(const Reply* reply) {
	bool ret = false;
	if (mRemotePort != 0) {
		String str = reply->toString();
		//Logger::verbose("UDP => %s", str.c_str());
		mUdp->sendStringTo(mRemoteIP, mRemotePort, str);
		ret = true;
	}
	return ret;
}




