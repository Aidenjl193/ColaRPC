#include "RPCManager.h"
#include "Serializer.h"

std::vector<RPC> RPCManager::rpcs;
std::vector<std::string> RPCManager::rpcNames;

void RPCManager::CreateRPC(std::string name, void(*function)(char*, int)) {
  RPC rpc;
  rpc.function = function;
  rpcs.push_back(rpc);
  rpcNames.push_back(name);
}

void RPCManager::RaiseRPC(int rpc, char* data, int len) {
  rpcs[rpc].function(data, len);
}

void RPCManager::SendRPC(int peerHandle, std::string name, char* data) {
  
}

int RPCManager::GetRPCID(std::string name) {
  for(int i = 0; i < rpcNames.size(); ++i) {
	if(rpcNames[i] == name) {
	  return i;
	}
  }
  return -1;
}
