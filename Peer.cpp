#include "Peer.h"

namespace P2P {
	Peer::Peer(const char* destinationIP, int destinationPort) {
		address.sin_family = AF_INET;
		ULONG* destAddr = new ULONG;
		inet_pton(AF_INET, destinationIP, destAddr);
		address.sin_addr.s_addr = *destAddr;
		address.sin_port = htons(destinationPort);
		delete destAddr;
		buff.InitializeBuffer(BUFFER_SIZE);
	}

	Peer::Peer(sockaddr_in add) {
		address = add;
		buff.InitializeBuffer(BUFFER_SIZE);
	}

	Peer::Peer() {
	}
}