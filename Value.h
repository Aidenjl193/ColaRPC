#pragma once
#include "Serializer.h"

namespace ColaRPC {
	class Value {
		long value_;
	public:
		template<class T>
		T get() {
			return (T)value_;
		}

		template<class T>
		T deserialize(ColaRPC::Serializer* ser) {
			T t = T();
			ser->deserialize(&t);
			return t;
		}

		template<class T>
		Value& operator=(T const& x) {
		  value_ = x;
			return *this;
		}
	};
}
