#pragma once
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include "TSRingBuffer.h"

#define BUFFER_SIZE 131072 //Needs experimenting
namespace P2P{
	class Peer {
	public:
		sockaddr_in address;
		TSRingBuffer<char> buff;

		int handle;

		Peer(const char* destination, int destinationPort);
		Peer(sockaddr_in add);
		Peer();
	};
}