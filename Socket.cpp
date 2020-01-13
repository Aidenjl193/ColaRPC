#include "Socket.h"

namespace ColaRPC {
// RPC STUFF

bool operator==(const sockaddr_in& a,
				const sockaddr_in& b) {	 // Comparison operator for sockaddr_in
	return memcmp(&a, &b, sizeof(sockaddr_in)) == 0;
}

int Socket::generateUID() {
	int UID = 0;
	bool isUnique = false;
	while (!isUnique) {
		UID = rand();
		isUnique = peers.count(UID) == 0;
	}
	return UID;
}

Socket::Socket(int port) {
#ifdef _WIN32
	WSAData data;
	WSAStartup(MAKEWORD(2, 2), &data);
#endif
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(port);
	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	bind(s, (sockaddr*)&local, sizeof(local));
}

int Socket::send(const char* packet, int len, int peerHandle) {
	return sendto(s, packet, len, 0, (sockaddr*)&peers[peerHandle].address,
				  sizeof(peers[peerHandle].address));
}

int Socket::getRpcID(std::string RPC) { return std::get<1>(rpcs[RPC]); }


int Socket::recieve() {	 // Dish out messages to peers
	sockaddr_in senderAddr;
	socklen_t SenderAddrSize = sizeof(sockaddr_in);

	// Malloc a buffer (Should pool this at some point)
	char* buffer = (char*)malloc(BUFFER_SIZE);

	int len = PACKET_SIZE;

	int32_t recieved = recvFrom(buffer, len, &senderAddr, SenderAddrSize);

	if (recieved == 0) {
		free(buffer);
		// Should remove the peer
		return 0;
	}

	if (!addressExsits(senderAddr)) {  // If the peer does not exist add it to our pool
		// Check if it maches our connection criteria
		char IP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(senderAddr.sin_addr), IP, INET_ADDRSTRLEN);

		// if (!(*connectionParams)(IP, senderAddr.sin_port, buffer, recieved))
		// {
		// //If the criteria is not met, do not add the peer 	free(buffer);
		// return 0;
		//}

		int UID = newPeer(senderAddr);

		// Call the onConnection Callback
		(*onConnection)(IP, senderAddr.sin_port, UID);
	}
	// Run the RPC on the threadpool

	Serializer serializer;
	serializer.buffer = buffer;
	serializer.write = recieved;
	int rpcID = 0;
	uint32_t callID = 0;

	serializer.deserialize(&rpcID);
	serializer.deserialize(&callID);  // Should leave this at the end of the
									  // buffer

	Task t;
	t.func = &std::get<0>(rpcs[rpcNames[rpcID]]);
	t.ser = serializer;
	TaskManager::assignTask(t);

	return recieved;
}

int Socket::getPort() {
	socklen_t len = sizeof(local);
	getsockname(s, (sockaddr*)&local, &len);
	return ntohs(local.sin_port);
}

int Socket::newPeer(const char* IP, int port) {
	int peerHandle = generateUID();
	Peer newPeer = Peer(IP, port);
	newPeer.handle = peerHandle;
	peers[peerHandle] = newPeer;
	return peerHandle;
}

int Socket::newPeer(sockaddr_in addr) {
	int peerHandle = generateUID();
	Peer newPeer = Peer(addr);
	newPeer.handle = peerHandle;
	peers[peerHandle] = newPeer;
	return peerHandle;
}

// Cross platform wrapper
int32_t Socket::recvFrom(char* buffer, int length, sockaddr_in* senderAddr,
						 socklen_t SenderAddrSize) {
#ifdef _WIN32
	return recvfrom(s, buffer, length, 0, (SOCKADDR*)senderAddr,
					&SenderAddrSize);
#else
	return recvfrom(s, (void*)buffer, len, 0, (SOCKADDR*senderAddr,
					&SenderAddrSize);
#endif
}

bool Socket::addressExsits(sockaddr_in addr) {
	for (std::map<int, Peer>::value_type& x : peers) {
		if (x.second.address == addr) {
			return true;
		}
	}
	return false;
}
}  // namespace ColaRPC
