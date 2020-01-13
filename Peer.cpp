#include "Peer.h"

namespace ColaRPC {
Peer::Peer(const char* destinationIP, int destinationPort) {
	address.sin_family = AF_INET;
	inet_pton(AF_INET, destinationIP, &(address.sin_addr));
	address.sin_port = htons(destinationPort);
}

Peer::Peer(sockaddr_in add) { address = add; }

Peer::Peer() {}
}  // namespace ColaRPC
