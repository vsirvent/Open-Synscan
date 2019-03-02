/*
 * UdpComm.h
 *
 *  Created on: Dec 31, 2018
 *      Author: vsirvent
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
