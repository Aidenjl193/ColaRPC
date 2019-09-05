#pragma once
#include <string>
#include <stdlib.h>
#include <tuple>
#include <unordered_map>
#include <map>
#include <future>
#include <any>
#include <memory>
#include "Peer.h"
#include "TaskManager.h"
#include "Serializer.h"
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

namespace ColaRPC {
	class Socket {
	public:
		//Callbacks
		typedef bool(STDCALL *ConnectionParams)(const char* ip, int port, const char* buffer, int recieved);
		typedef void(STDCALL *OnConnection)(const char* ip, int port, int uid);

		ConnectionParams connectionParams = [](const char*, int, const char*, int) { return true; }; //Accept all connections by default
		OnConnection onConnection;

		std::map<int, Peer> peers;

		Socket(int port);

		int send(const char* packet, int len, int peerHandle);
		int recieve();
		int getPort();

		int newPeer(const char* IP, int port);

		//RPC SHIT
		std::unordered_map<std::string, std::tuple<Function, int>> rpcs;
		std::unordered_map<int, std::string> rpcNames;

		template<class F>
		void bindRPC(std::string const& name, F f) {
			rpcs[name] = std::make_tuple(FunctionImpl<F>(std::move(f)), rpcCount);
			rpcNames[rpcCount] = name;
			rpcCount++;
		}

		template <typename A>
		void serializeArgs(const A& a) {}

		//Recursively serialize the arguments
		template <typename A, typename ...B>
		void serializeArgs(ColaRPC::Serializer* serializer, const A& a, B&&... Args) {
			serializer->serialize(a);
			serializeArgs(serializer, Args...);
		}

	  //Might be worth threadsafing this? Need to think of use cases
	  template <typename ...A>
	  std::future<Value> call(std::string name, int peerHandle, A... Args) {
		char* data = (char*)malloc(PACKET_SIZE); 
			ColaRPC::Serializer ser = ColaRPC::Serializer();
			ser.buffer = data;

			int rpcID = getRpcID(name);

			uint32_t callID = getCallID();

			ser.serialize(rpcID);
			ser.serialize(callID);
			
			serializeArgs(&ser, Args...);
			send((char*)&ser.buffer[0], ser.write, peerHandle);
			free(data);

			//Make a shared pointer of the promise so when the response is sent back
			//We can un-sleep the thread if any are waiting for the return value
			
			auto promise = std::make_shared<std::promise<Value>>();
			callPromises[callID] = promise;

			return promise->get_future();
		}

		int getRpcID(std::string RPC);

	private:
		sockaddr_in local;
		SOCKET s;

		//Needs a more robust method
		int rpcCount = 0;

	  uint32_t callID = 0;

	  std::unordered_map<uint32_t, std::shared_ptr<std::promise<Value>>> callPromises;

	  int getCallID() {
		return ++callID;
	  }

		int generateUID();
	};
}
