#include <iostream>
#include <string>
#include <typeinfo>
#include "Socket.h"
#include "TaskManager.h"

int print(std::string str) {
  std::cout << str;
  return 0;
}

void DisplayMessage(char a, int b) {
  std::cout << a << " " << b << "\n";
}

int main() {
  std::cout << "Starting...\n";
  
  P2P::TaskManager::InitializeThreads(4);
  
  P2P::Socket socket = P2P::Socket(0); //Bind socket to random port

  socket.bindRPC("Print", &print);

  P2P::Serializer serializer;
  serializer.buffer = (char*)malloc(500);
  serializer.Serialize("yo doggy!\n");
  P2P::Task t;
  P2P::Function func = (P2P::Function)P2P::FunctionImpl<int(*)(std::string)>(std::move(&print));
  t.func = &func;
  t.ser = serializer;
  P2P::TaskManager::AssignTask(t);
  
  std::cout << "Socket bound on port: " << socket.GetPort() << "\n";

  socket.onConnection = [](const char* ip, int port, int peerHandle) { //On connection callback
  	std::cout << "New peer " << peerHandle << " connected with IP: " << ip << " and port: " << port << "\n";
  };

  bool server = true;

  std::cout << "Server? 1/0\n";

  std::cin >> server;

  if(server) {
	while(true) {
	  socket.Recieve(); //process RPCs
	}
  } else {
	const char*  IP = "127.0.0.1";

	std::cout << "Enter host port:\n";
	int port = 0;
	std::cin >> port;

	std::cout << "Connecting to host at: " << IP << ":" << port << "\n";
	int peerHandle = socket.NewPeer((char*)&IP, port);
	while(true) {
	  std::cout << "Enter message!\n";
	  std::string str;
	  std::cin >> str;
	  if(str == "exit")
		return 0;
	  str += "\n";
	  
	  socket.call("Print", peerHandle, str);
	  std::cin.get();
	}
  }
  std::cin.get();
}
