#pragma once
#include <Vector>
#include <string>
#include "Socket.h"

struct RPC { //Wrapper for an RPC
public:
  void(*function)(char*, int);
};

class RPCManager {
public:
static void CreateRPC(std::string name, void(*function)(char*, int));

  static void RaiseRPC(int rpc, char* data, int len);

  static void SendRPC(int peerHandle, std::string name, char* data);

  static int GetRPCID(std::string name);
private:
    static std::vector<RPC> rpcs;
  static std::vector<std::string> rpcNames;
};

