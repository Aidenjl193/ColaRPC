#pragma once

#include <string>
#include <stdlib.h>
#include "Peer.h"
#include <map>
#include "TaskManager.h"

#ifdef _WIN32						//Windows sockets
	#include <WinSock2.h>
	#include <Ws2tcpip.h>
	#pragma comment(lib, "Ws2_32.lib")
#else								//Posix socks
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <unistd.h>
#endif

#define PACKET_SIZE 65507

#ifdef _WIN32
	#define STDCALL __stdcall
#else
	#define SOCKET int
	#define SOCKADDR struct sockaddr
	#define STDCALL 
#endif

namespace P2P {
	class Socket {
	public:
		//Callbacks
		typedef bool(STDCALL *ConnectionParams)(const char* ip, int port, const char* buffer, int recieved);
		typedef void(STDCALL *OnConnection)(const char* ip, int port, int uid);

		ConnectionParams connectionParams = [](const char*, int, const char*, int) { return true; }; //Accept all connections by default
		OnConnection onConnection;

		std::map<int, Peer> peers;

		Socket(int port);

		int Send(const char* packet, int peerHandle);
		int Recieve();
		int GetPort();

		int NewPeer(const char* IP, int port);

	private:
		sockaddr_in local;
		SOCKET s;

		int GenerateUID();
	};
}
