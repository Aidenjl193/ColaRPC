#include "Socket.h"

namespace P2P {
	bool operator==(const sockaddr_in& a, const sockaddr_in& b) { //Comparison operator for sockaddr_in
		return memcmp(&a, &b, sizeof(sockaddr_in)) == 0;
	}

	int Socket::GenerateUID() {
		int UID = 0;
		bool isUnique = false;
		while (!isUnique) {
			UID = rand();
			isUnique = peers.count(UID) == 0;
		}
		return UID;
	}

	Socket::Socket(int port) {
		WSAData data;
		WSAStartup(MAKEWORD(2, 2), &data);
		local.sin_family = AF_INET;
		local.sin_addr.s_addr = INADDR_ANY;
		local.sin_port = htons(port);
		s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		bind(s, (sockaddr *)&local, sizeof(local));
	}


	int Socket::Send(const char* packet, int peerHandle) {
		return sendto(s, packet, strlen(packet), 0, (sockaddr *)&peers[peerHandle].address, sizeof(peers[peerHandle].address));
	}

	int Socket::Recieve() { //Dish out messages to peers
		sockaddr_in senderAddr;
		int SenderAddrSize = sizeof(sockaddr_in);
		char buffer[PACKET_SIZE];
		int len = PACKET_SIZE;
		int32_t recieved = recvfrom(s, buffer, len, 0, (SOCKADDR *)& senderAddr, &SenderAddrSize);

		//Check if we have a connection with this peer currently
		int index = -1;

		for (std::map<int, Peer>::value_type& x : peers) {
			if (x.second.address == senderAddr) {
				index = x.second.handle;
			}
		}

		if (index != -1) { //Connection already exists so add the message to its queue
			//If the connection has dropped remove the peer
			if (recieved == 0) { //Need to raise a connection dropped
				return -1;
			}
			for (int i = 0; i < recieved; ++i) {
				if (!peers[index].buff.Deposit(&buffer[i])) //if we fail, try again (Blocking)
					--i;
			}
		}
		else { //Create a new instance of Peer to host the new connection
		 //Check if it maches our connection criteria

		 //Needs abstraction
		 //Get IP
			char IP[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(senderAddr.sin_addr), IP, INET_ADDRSTRLEN);

			if (!(*connectionParams)(IP, senderAddr.sin_port, buffer, recieved)) { //If the criteria is not met, do not add the peer
				return 0;
			}

			//Generate a unique handle to reference the peer
			int UID = GenerateUID();

			Peer newPeer = Peer(senderAddr);
			newPeer.handle = UID;

			//Create a new peer and add it to the map
			peers[UID] = newPeer;

			//Call the onConnection Callback
			(*onConnection)(IP, senderAddr.sin_port, UID);
		}
		return recieved;
	}

	int Socket::GetPort() {
		int len = sizeof(local);
		getsockname(s, (sockaddr *)&local, &len);
		return ntohs(local.sin_port);
	}

	int Socket::NewPeer(const char* IP, int port) {
		int peerHandle = GenerateUID();
		Peer newPeer = Peer(IP, port);
		newPeer.handle = peerHandle;
		peers[peerHandle] = newPeer;
		return peerHandle;
	}
}