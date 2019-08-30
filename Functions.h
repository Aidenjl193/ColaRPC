
#pragma once
#include <unordered_map>
#include "Function.h"
#include "Value.h"
#include "Serializer.h"
namespace P2P {
	class Functions {
		private:
			std::unordered_map<std::string, Function> functions_;

			template <typename A>
			void SerializeArgs(const A& a) {}

			//Recursively serialize the arguments
			template <typename A, typename ...B>
			void SerializeArgs(P2P::Serializer* serializer, const A& a, B&&... Args) {
				serializer->Serialize(a);
				SerializeArgs(serializer, Args...);
			}

		public:
			template<class F>
			void add(std::string const& name, F f) {
				functions_[name] = FunctionImpl<F>(std::move(f));
			}

			Value raise(std::string name, P2P::Serializer* ser) {
				return functions_[name](ser);
			}

			template <typename ...A>
			P2P::Serializer call(std::string name, A... Args) {
				char* data = (char*)malloc(1000); //Needs to be the packet size
				P2P::Serializer ser = P2P::Serializer();
				ser.buffer = data;
				SerializeArgs(&ser, Args...);

				return ser;

				//return type should eventually be void and it should send the data over the socket
			}
	};
}