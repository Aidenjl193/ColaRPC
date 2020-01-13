#pragma once
#include <future>

#include "Value.h"

//A simple wrapper for futures that supports deserialization of types
namespace ColaRPC {
class Future {
   public:
	Future(std::future<Value> f) : future_(std::move(f)) {}

	template <typename T>
	T as() {
		//Wait for the result to come back from the peer and deserialize it into the right type
		future_.wait();
		Value v = future_.get();
		return v.get<T>();
	}

   private:
	std::future<Value> future_;
};
}  // namespace ColaRPC
