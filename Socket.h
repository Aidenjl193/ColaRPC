#pragma once

#include <string>
#include <stdlib.h>
#include <tuple>
#include "Peer.h"
#include <map>
#include "TaskManager.h"
#include "Serializer.h"
#include <unordered_map>
#include "Function.h"
#include "Value.h"

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

		int Send(const char* packet, int len, int peerHandle);
		int Recieve();
		int GetPort();

		int NewPeer(const char* IP, int port);

		//RPC SHIT
		std::unordered_map<std::string, std::tuple<Function, int>> RPCs;
		std::unordered_map<int, std::string> RPCNames;

		template<class F>
		void bindRPC(std::string const& name, F f) {
			RPCs[name] = std::make_tuple(FunctionImpl<F>(std::move(f)), rpcCount);
			RPCNames[rpcCount] = name;
			rpcCount++;
		}

		template <typename A>
		void SerializeArgs(const A& a) {}

		//Recursively serialize the arguments
		template <typename A, typename ...B>
		void SerializeArgs(P2P::Serializer* serializer, const A& a, B&&... Args) {
			serializer->Serialize(a);
			SerializeArgs(serializer, Args...);
		}

		template <typename ...A>
		void call(std::string name, int peerHandle, A... Args) {
			char* data = (char*)malloc(PACKET_SIZE);
			P2P::Serializer ser = P2P::Serializer();
			ser.buffer = data;

			int rpcID = GetRPCID(name);
			ser.Serialize(rpcID);//Need to serialize rpcid at front

			SerializeArgs(&ser, Args...);

			std::cout << "Sending RPC with ID: " << rpcID << " and peer ID: " << peerHandle << "\n";
			Send((char*)&ser.buffer[0], ser.write, peerHandle);
		}

		int GetRPCID(std::string RPC);

	private:
		sockaddr_in local;
		SOCKET s;

		//Needs a more robust method
		int rpcCount = 0;

		int GenerateUID();
	};
}
