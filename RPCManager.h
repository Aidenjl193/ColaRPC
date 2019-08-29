#pragma once
#include <vector>
#include <string>
#include "Socket.h"
#include <typeinfo>
#include <typeindex>

//Type Wrapping

class TypeBase {
public:
    virtual ~TypeBase() = 0;
};

inline TypeBase::~TypeBase() {}

template<class T>
class TypeWrapper : public TypeBase {
public:
    typedef T Type;
    explicit TypeWrapper(const Type& data) : data(data) {}
    TypeWrapper() {}
    Type data;
};

std::map<std::type_index, std::unique_ptr<TypeBase>> types;

class RPC { //Wrapper for an RPC
public: 
  void* func;
  
  std::vector<std::type_index> signature;

  //2 arguments
  template <typename R, typename T1, typename T2>
  RPC(R (*f)(T1, T2)) {
	//Register the type so we can initialize it based on the type index
	types[typeid(R)].reset(new TypeWrapper<R>());
	types[typeid(T1)].reset(new TypeWrapper<T1>());
	types[typeid(T2)].reset(new TypeWrapper<T2>());

	//Store the index of the types locally
	signature.push_back(typeid(R));
	signature.push_back(typeid(T1));
	signature.push_back(typeid(T2));
  }
};

class RPCManager {
public:
static void CreateRPC(std::string name, void(*func)(void*...));

  static void RaiseRPC(int rpc, char* buffer);

  static void SendRPC(int peerHandle, std::string name, char* data);

  static int GetRPCID(std::string name);
private:
    static std::vector<RPC> rpcs;
  static std::vector<std::string> rpcNames;
};

