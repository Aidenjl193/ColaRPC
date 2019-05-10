#pragma once
#include <vector>
#include <string>

namespace P2P {
  class Serializer {
  public:
	
	bool is_big_endian() {
	    union {
    	    uint32_t i;
    		 char c[4];
   		 } endian = {0x01020304};
   		 return endian.c[0] == 1;
	}
	char* buffer;
	int write = 0;
	int read = 0;

	void WriteByte(char b);
	char ReadByte();

	//Recursive
	void Serialize(Serializer s);
	
	//int
	void Serialize(int i);

	bool Deserialize(int* i);

	//float
	void Serialize(float f);
	void Deserialize(float * f);
	
	//String
	void Serialize(std::string str);

	void Deserialize(std::string* str);

	//char
	void Serialize(char c);

	void Deserialize(char* c);

	
	
  };
}
