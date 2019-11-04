#pragma once
#include <future>

#include "Value.h"

namespace ColaRPC {
class Future {
   public:
	Future(std::future<Value> f) : future_(std::move(f)) {}

	template <typename T>
	T as() {
		future_.wait();
		Value v = future_.get();
		return v.get<T>();
	}

   private:
	std::future<Value> future_;
};
}  // namespace ColaRPC
