#pragma once
#include <Map>
#include <string>
#include "Socket.h"

struct RPC { //Wrapper for an RPC
public:
  void(*function)(void*, int);
};

class RPCManager {
public:
static void CreateRPC(std::string name, void(*function)(void*, int));

  static void RaiseRPC(std::string name, void* data, int len);

  static void SendRPC(int peerHandle, std::string name, void* data);
private:
    static std::map<std::string, RPC> rpcs;
};

