#include <iostream>
#include <string>
#include "Socket.h"
#include "RPCManager.h"
#include "TaskManager.h"
#include <typeinfo>

void print(char* data, int len) {
  std::cout << "RPC Called!\n";
  P2P::Serializer serializer;
  serializer.buffer = data;
  serializer.write = len;
  std::string str = "";
  serializer.Deserialize(&str);
  std::cout << str;
}

void SerializeArgs(void){}

//Recursively parse the arguments
template <typename A, typename ...B>
void SerializeArgs(const A& a, B&&... Args) {
  std::cout << a << std::endl;
  SerializeArgs(Args...);
}

int main() {
  std::cout << "Starting...\n";

  auto SendRPC = [](std::string name, auto... arg) { SerializeArgs(arg...); };
  SendRPC("Hello", 3, 'd');
  
  P2P::TaskManager::InitializeThreads(4);
  
  P2P::Socket socket = P2P::Socket(0); //Bind socket to random port
  
  std::cout << "Socket bound on port: " << socket.GetPort() << "\n";

  socket.onConnection = [](const char* ip, int port, int peerHandle) { //On connection callback
  	std::cout << "New peer " << peerHandle << " connected with IP: " << ip << " and port: " << port << "\n";
  };

  bool server = true;

  //Setup RPCS
  RPCManager::CreateRPC("Print", print);

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
	  
	  char* data = (char*)malloc(PACKET_SIZE);
	  P2P::Serializer serializer;
	  serializer.buffer = data;
	  serializer.Serialize(str);
	  socket.SendRPC("Print", &serializer, peerHandle);
	  delete[] data;
	  std::cin.get();
	}
  }
  std::cin.get();
}
