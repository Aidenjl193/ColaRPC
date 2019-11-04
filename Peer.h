#pragma once
#ifdef _WIN32  // Windows sockets
#include <Ws2tcpip.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else  // Posix socks
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#define BUFFER_SIZE 131072	// Needs experimenting
namespace ColaRPC {
class Peer {
   public:
	sockaddr_in address;

	int handle;

	Peer(const char* destination, int destinationPort);
	Peer(sockaddr_in add);
	Peer();
};
}  // namespace ColaRPC
