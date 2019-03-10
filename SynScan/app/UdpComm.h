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

#ifndef UDPCOMM_H_
#define UDPCOMM_H_

#include <Comm.h>

class UdpComm: public Comm {
private:
	IPAddress mRemoteIP;
	uint16_t mRemotePort = 0;
	UdpConnection* mUdp = NULL;

	void onUdpRx(UdpConnection& connection, char *data, int size, IPAddress remoteIP, uint16_t remotePort);

public:

	UdpComm();
	virtual ~UdpComm();

	bool sendReply(const Reply* reply);
};

#endif /* UDPCOMM_H_ */
