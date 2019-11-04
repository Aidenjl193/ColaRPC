#include <iostream>
#include <string>
#include <typeinfo>

#include "Socket.h"
#include "TaskManager.h"

int print(std::string str) {
	std::cout << str;
	return 0;
}

int add(int a, int b) {
	std::cout << a + b << "\n";
	return 0;
}

int main() {
	ColaRPC::TaskManager::initializeThreads(4);

	ColaRPC::Socket sock = ColaRPC::Socket(0);	// Bind sock to random port

	sock.bindRPC("print", &print);
	sock.bindRPC("add", &add);

	std::cout << "Socket bound on port: " << sock.getPort() << "\n";

	sock.onConnection = [](const char* ip, int port,
						   int peerHandle) {  // On connection callback
		std::cout << "New peer " << peerHandle << " connected with IP: " << ip
				  << " and port: " << port << "\n";
	};

	bool server = true;

	std::cout << "Server? 1/0\n";

	std::cin >> server;

	if (server) {
		while (true) {
			sock.recieve();	 // process RPCs
		}
	} else {
		const char* IP = "127.0.0.1";

		std::cout << "Enter host port:\n";
		int port = 0;
		std::cin >> port;

		std::cout << "Connecting to host at: " << IP << ":" << port << "\n";
		int peerHandle = sock.newPeer((char*)&IP, port);

		while (true) {
			std::cout << "Enter message!\n";
			std::string str;
			std::cin >> str;
			str += "\n";
			sock.call("print", peerHandle, str);
			sock.call("add", peerHandle, 10, 2);
			std::cin.get();
		}
	}
	std::cin.get();
}
