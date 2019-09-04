#pragma once
#include <vector>
#include <string>

namespace ColaRPC {
  class Serializer {
  public:
	
	bool isBigEndian() {
	    union {
    	    uint32_t i;
    		 char c[4];
   		 } endian = {0x01020304};
   		 return endian.c[0] == 1;
	}
	char* buffer;
	int write = 0;
	int read = 0;

	void writeByte(char b);
	char readByte();

	//Recursive
	void serialize(Serializer s);
	
	//int
	void serialize(int i);

	bool deserialize(int* i);

	//float
	void serialize(float f);
	void deserialize(float * f);
	
	//String
	void serialize(std::string str);

	void deserialize(std::string* str);

	//char
	void serialize(char c);
	void deserialize(char* c);
  };
}
