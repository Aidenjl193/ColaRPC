#include "Socket.h"

namespace P2P {
	bool operator==(const sockaddr_in& a, const sockaddr_in& b) { //Comparison operator for sockaddr_in
		return memcmp(&a, &b, sizeof(sockaddr_in)) == 0;
	}

  void ExecuteRPC(char* data, int len) {
	Serializer serializer;
	serializer.buffer = data + len - 4; //Read int from back
	serializer.write = len;
	int rpcID = 0;
	serializer.Deserialize(&rpcID);
	//RPCManager::RaiseRPC(rpcID, data, serializer.write - 4);
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
	  #ifdef _WIN32
		WSAData data;
		WSAStartup(MAKEWORD(2, 2), &data);
		#endif
		local.sin_family = AF_INET;
		local.sin_addr.s_addr = INADDR_ANY;
		local.sin_port = htons(port);
		s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		bind(s, (sockaddr *)&local, sizeof(local));
	}

  int Socket::Send(const char* packet, int len, int peerHandle) {
		return sendto(s, packet, len, 0, (sockaddr *)&peers[peerHandle].address, sizeof(peers[peerHandle].address));
	}

  void Socket::SendRPC(std::string RPC, Serializer* serializer, int peerHandle) {
	//int rpcID = RPCManager::GetRPCID(RPC);
	//serializer->Serialize(rpcID);//Need to serialize rpcid at front

	Send((char*)&serializer->buffer[0], serializer->write, peerHandle);
  }

	int Socket::Recieve() { //Dish out messages to peers
		sockaddr_in senderAddr;
		socklen_t SenderAddrSize = sizeof(sockaddr_in);

		//Malloc a buffer (Should pool this at some point)
		char* buffer = (char*)malloc(BUFFER_SIZE);
		
		int len = PACKET_SIZE;

#ifdef _WIN32	
		int32_t recieved = recvfrom(s, buffer, len, 0, (SOCKADDR *)& senderAddr, &SenderAddrSize);
#else
		int32_t recieved = recvfrom(s, (void*)buffer, len, 0, (SOCKADDR *)& senderAddr, &SenderAddrSize);
#endif

		if(recieved == 0) {
		  delete[] buffer;
		  return 0;
		}

		//Check if we have a connection with this peer currently
		int index = -1;

		for (std::map<int, Peer>::value_type& x : peers) {
			if (x.second.address == senderAddr) {
				index = x.second.handle;
			}
		}

		if (index == -1) { //If the peer does not exist add it to our pool
		 //Check if it maches our connection criteria
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

		//Run the RPC on the threadpool
	    Task t;
  	    t.data = buffer;
	    t.len = recieved;
	    t.function = ExecuteRPC;
	    TaskManager::AssignTask(t);
		
		return recieved;
	}

	int Socket::GetPort() {
		socklen_t len = sizeof(local);
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
