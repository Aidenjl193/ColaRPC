#include <iostream>
#include <string>
#include "Serializer.h"
#include "Socket.h"

void print(void* string, int len) {
  char chars[len];
  memcpy(&chars, string, len);
  for(int i = 0; i < len; ++i) {
	std::cout << chars[i];
  }
}

int main() {
  std::cout << "Starting...\n";

  P2P::Socket socket = P2P::Socket(0);
  std::cout << "Socket bound on port: " << socket.GetPort() << "\n";

  socket.onConnection = [](const char* ip, int port, int uid) {
  	std::cout << "New peer " << uid << " connected with IP: " << ip << " and port: " << port << "\n";
  };

  bool server = true;

  std::cout << "Server? 1/0\n";

  std::cin >> server;

  if(server) {
	while(true) {
	  socket.Recieve();
	  for(std::map<int, P2P::Peer>::value_type& x : socket.peers) {
		while(x.second.buff.CanGet())
		  std::cout << x.second.buff.Get();
	  }
	}
  } else {
	std::cout << "Enter host IP:\n";
	std::string IP = "";
	std::cin >> IP;

	std::cout << "Enter host port:\n";
	int port = 0;
	std::cin >> port;

	std::cout << "Connecting to host at: " << IP << ":" << port << "\n";
	int peerHandle = socket.NewPeer((char*)&IP, port);
	while(true) {
	  socket.Send("Hello\n", peerHandle);
	  std::cin.get();
	}
  }

  std::cin.get();
	
}
