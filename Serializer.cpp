#include "Serializer.h"
#include <iostream>

namespace ColaRPC {
  
  void Serializer::writeByte(char b) {
	buffer[write] = b;
	write++;
  }

  char Serializer::readByte() {
	if(read <= write) {
	  char data = buffer[read];
	  read++;
	  return data;
	}
	return 0;
  }

  //recursive
  void Serializer::serialize(Serializer s) {
	for(int i = 0; i < s.write; ++i) {
	  writeByte(s.readByte());
	}
  }
  
// int
void Serializer::serialize(int i) {
  char* data = (char*)&i;
  if(isBigEndian()) {
	writeByte(data[3]);
	writeByte(data[2]);
	writeByte(data[1]);
	writeByte(data[0]);
  }else {
  writeByte(data[0]);
  writeByte(data[1]);
  writeByte(data[2]);
  writeByte(data[3]);
  }
}

bool Serializer::deserialize(int* i) {
  char* data = (char*)i;
  if(isBigEndian()) {
	data[3] = readByte();
	data[2] = readByte();
	data[1] = readByte();
	data[0] = readByte();
  }else {
	data[0] = readByte();
	data[1] = readByte();
	data[2] = readByte();
	data[3] = readByte();
  }
  return true;
}

// uint_32
void Serializer::serialize(uint32_t i) {
  char* data = (char*)&i;
  if(isBigEndian()) {
	writeByte(data[3]);
	writeByte(data[2]);
	writeByte(data[1]);
	writeByte(data[0]);
  }else {
  writeByte(data[0]);
  writeByte(data[1]);
  writeByte(data[2]);
  writeByte(data[3]);
  }
}

bool Serializer::deserialize(uint32_t* i) {
  char* data = (char*)i;
  if(isBigEndian()) {
	data[3] = readByte();
	data[2] = readByte();
	data[1] = readByte();
	data[0] = readByte();
  }else {
	data[0] = readByte();
	data[1] = readByte();
	data[2] = readByte();
	data[3] = readByte();
  }
  return true;
}

// string

  void Serializer::serialize(std::string str) {
	serialize((int)str.length());
	for(int i = 0; i < str.length(); ++i) {
	  writeByte(str[i]);
	}
  }

  void Serializer::deserialize(std::string* str) {
	int len = 0;
	deserialize(&len);
	std::string temp = "";
	for(int i = 0; i < len; ++i) {
	  temp += readByte();
	}
	*str = temp;
  }

  // char

  void Serializer::serialize(char c) {
	writeByte(c);
  }

  void Serializer::deserialize(char* c) {
	*c = readByte();
  }
}
