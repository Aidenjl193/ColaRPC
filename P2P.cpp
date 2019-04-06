#include <iostream>
#include <string>
#include "Socket.h"
#include "RPCManager.h"
#include "TaskManager.h"

void print(char* data, int len) {
  std::cout << "RPC Called!\n";
  P2P::Serializer serializer;
  serializer.buffer = data;
  serializer.write = len;
  std::string str = "";
  serializer.Deserialize(&str);
  std::cout << str;
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
	}
  } else {
	const char*  IP = "127.0.0.1";

	std::cout << "Enter host port:\n";
	int port = 0;
	std::cin >> port;

	std::cout << "Connecting to host at: " << IP << ":" << port << "\n";
	int peerHandle = socket.NewPeer((char*)&IP, port);
	while(true) {
	  char* data = (char*)malloc(PACKET_SIZE);
	  P2P:: Serializer serializer;
	  serializer.buffer = data;
	  std::string string = "Hello RPC!\n";
	  serializer.Serialize(string);
	  socket.SendRPC("Print", &serializer, peerHandle);
	  delete[] data;
	  std::cin.get();
	}
  }

  std::cin.get();
	
}
