#include "RPCManager.h"
#include <iostream>

std::map<std::string, RPC> RPCManager::rpcs;

void RPCManager::CreateRPC(std::string name, void(*function)(void*, int)) {
  RPC rpc;
  rpc.function = function;
  rpcs.insert(std::pair<std::string, RPC>(name, rpc));
}

void RPCManager::RaiseRPC(std::string name, void* data, int len) {
  if(rpcs.count(name)) {
	  rpcs.find(name)->second.function(data, len);
  }
}

void RPCManager::SendRPC(int peerHandle, std::string name, void* data) {

}
