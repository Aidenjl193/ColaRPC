#include "RPCManager.h"
#include "Serializer.h"

std::vector<RPC> RPCManager::rpcs;
std::vector<std::string> RPCManager::rpcNames;


//TEMPLATED AUTO SERIALIZATION AND TYPE EXTRACTION

template <typename A>
void SerializeArgs(const A& a) {}

//Recursively serialize the arguments
template <typename A, typename ...B>
void SerializeArgs(P2P::Serializer* serializer, const A& a, B&&... Args) {
  serializer->Serialize(a);
  SerializeArgs(serializer, Args...);
}

//1 arg
template <typename R, typename T1>
void DeserializeArgsAndExecute(R (*f)(T1), P2P::Serializer* s) {
  T1 arg1;
  s->Deserialize(&arg1);
  f(arg1);
}

//2 args
template <typename R, typename T1, typename T2>
void DeserializeArgsAndExecute(R (*f)(T1, T2), P2P::Serializer* s) {
  T1 arg1;
  T2 arg2;
  s->Deserialize(&arg1);
  s->Deserialize(&arg2);
  f(arg1, arg2);
}

void RPCManager::CreateRPC(std::string name, void(*func)(void*...)) {
  //RPC rpc;
  //rpc.func = func;
  // rpcs.push_back(rpc);
  //rpcNames.push_back(name);
}

void RPCManager::RaiseRPC(int rpc, char* data) {
  //P2P::Serializer serializer;
  //serializer.buffer = data;
  //DeserializeArgsAndExecute(rpcs[rpc].func, &serializer);
}

//P2P::Serializer SerializeRPC(std::string name, auto... arg) {
//	  char* data = (char*)malloc(PACKET_SIZE);
//	  P2P::Serializer serializer;
//	  serializer.buffer = data;
//	  SerializeArgs(&serializer, arg...);
//	  return serializer;
//  };

int RPCManager::GetRPCID(std::string name) {
  for(int i = 0; i < rpcNames.size(); ++i) {
	if(rpcNames[i] == name) {
	  return i;
	}
  }
  return -1;
}
