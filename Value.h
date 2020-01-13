#pragma once
#include "Serializer.h"

namespace ColaRPC {
class Value {
	long value_;

   public:
	Serializer ser;

	~Value() { free(ser.buffer); }

	template <class T>
	T get() {
		return (T)value_;
	}

	template <class T>
	T deserialize(ColaRPC::Serializer* ser) {
		T t = T();
		ser->deserialize(&t);
		return t;
	}

	//Big stanky disgusting code
	template <class T>
	Value& operator=(T const& x) {
		// This could be a ram killer, need to find a better solution
		ser.buffer = (char*)malloc(65507);
		ser.serialize(x);
		value_ = x;
		return *this;
	}
};
}  // namespace ColaRPC
