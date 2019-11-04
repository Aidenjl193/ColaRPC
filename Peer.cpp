#include "Peer.h"

namespace ColaRPC {
Peer::Peer(const char* destinationIP, int destinationPort) {
	address.sin_family = AF_INET;
	unsigned long* destAddr = new unsigned long;
	inet_pton(AF_INET, destinationIP, destAddr);
	address.sin_addr.s_addr = *destAddr;
	address.sin_port = htons(destinationPort);
	delete destAddr;
}

Peer::Peer(sockaddr_in add) { address = add; }

Peer::Peer() {}
}  // namespace ColaRPC
