#include "Serializer.h"
#include <iostream>

namespace P2P{

  void Serializer::WriteByte(char b) {
	buffer[write] = b;
	write++;
  }

  char Serializer::ReadByte() {
	if(read <= write) {
	  char data = buffer[read];
	  read++;
	  return data;
	}
	return 0;
  }
  
// int
void Serializer::Serialize(int i) {
  char* data = (char*)&i;
  if(is_big_endian()) {
	WriteByte(data[3]);
	WriteByte(data[2]);
	WriteByte(data[1]);
	WriteByte(data[0]);
  }else {
  WriteByte(data[0]);
  WriteByte(data[1]);
  WriteByte(data[2]);
  WriteByte(data[3]);
  }
}

bool Serializer::Deserialize(int* i) {
  char* data = (char*)i;
  if(is_big_endian()) {
	data[3] = ReadByte();
	data[2] = ReadByte();
	data[1] = ReadByte();
	data[0] = ReadByte();
  }else {
	data[0] = ReadByte();
	data[1] = ReadByte();
	data[2] = ReadByte();
	data[3] = ReadByte();
  }
  return true;
}

// string

  void Serializer::Serialize(std::string str) {
	Serialize((int)str.length());
	for(int i = 0; i < str.length(); ++i) {
	  WriteByte(str[i]);
	}
  }

  void Serializer::Deserialize(std::string* str) {
	int len = 0;
	Deserialize(&len);
	std::cout << "String Length: " << len << "\n";
	std::string temp = "";
	for(int i = 0; i < len; ++i) {
	  temp += ReadByte();
	}
	*str = temp;
  }

  // char

  void Serializer::Serialize(char c) {
	WriteByte(c);
  }

  void Serializer::Deserialize(char* c) {
	*c = ReadByte();
  }
}
