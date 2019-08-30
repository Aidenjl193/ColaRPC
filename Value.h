#pragma once
#include "Serializer.h"

namespace P2P {
	class Value {
		long value_;
	public:
		template<class T>
		T get() {
			return (T)value_;
		}

		template<class T>
		T deserialize(P2P::Serializer* ser) {
			T t = T();
			ser->Deserialize(&t);
			return t;
		}

		template<class T>
		Value& operator=(T const& x)
		{
			value_ = x;
			return *this;
		}
	};
}