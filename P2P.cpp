#include <iostream>
#include <string>
#include "Socket.h"
#include "RPCManager.h"
#include "TaskManager.h"

void print(char* data, int len) {
  std::cout << "RPC Called!\n";
}

int main() {
  std::cout << "Starting...\n";
  P2P::TaskManager::InitializeThreads(4);
  P2P::Socket socket = P2P::Socket(0);
  std::cout << "Socket bound on port: " << socket.GetPort() << "\n";

  socket.onConnection = [](const char* ip, int port, int uid) {
  	std::cout << "New peer " << uid << " connected with IP: " << ip << " and port: " << port << "\n";
  };

  bool server = true;

  //Setup RPCS
  RPCManager::CreateRPC("Print", print);

  std::cout << "Server? 1/0\n";

  std::cin >> server;

  if(server) {
	while(true) {
	  socket.Recieve();
	  //for(std::map<int, P2P::Peer>::value_type& x : socket.peers) {
		//while(x.second.buff.CanGet())
		  //std::cout << x.second.buff.Get();
		//}
	}
  } else {
	const char*  IP = "127.0.0.1";

	std::cout << "Enter host port:\n";
	int port = 0;
	std::cin >> port;

	std::cout << "Connecting to host at: " << IP << ":" << port << "\n";
	int peerHandle = socket.NewPeer((char*)&IP, port);
	while(true) {
	  P2P:: Serializer serializer;
	  std::string string = "Hello RPC!";
	  serializer.Serialize(string);
	  socket.SendRPC("Print", &serializer, peerHandle);
	  
	  std::cin.get();
	}
  }

  std::cin.get();
	
}
