#include "Serializer.h"
#include <iostream>

namespace P2P{

// int
void Serializer::Serialize(int i) {
  char* data = (char*)&i;
  if(is_big_endian()) {
	buffer.push_back(data[3]);
	buffer.push_back(data[2]);
	buffer.push_back(data[1]);
	buffer.push_back(data[0]);
  }else {
  buffer.push_back(data[0]);
  buffer.push_back(data[1]);
  buffer.push_back(data[2]);
  buffer.push_back(data[3]);
  }
}

bool Serializer::Deserialize(int* i) {
  char* data = (char*)i;
  if(is_big_endian()) {
	data[0] = buffer[buffer.size() - 1];
    buffer.pop_back();
	data[1] = buffer[buffer.size() - 1];
    buffer.pop_back();
	data[2] = buffer[buffer.size() - 1];
    buffer.pop_back();
	data[3] = buffer[buffer.size() - 1];
    buffer.pop_back();
  }else {
	data[3] = buffer[buffer.size() - 1];
    buffer.pop_back();
	data[2] = buffer[buffer.size() - 1];
    buffer.pop_back();
	data[1] = buffer[buffer.size() - 1];
    buffer.pop_back();
	data[0] = buffer[buffer.size() - 1];
    buffer.pop_back();
  }
  return true;
}

// string

  void Serializer::Serialize(std::string str) {
	for(int i = 0; i < str.length(); ++i) {
	  buffer.push_back(str[i]);
	}
	Serialize((int)str.length());
  }

  void Serializer::Deserialize(std::string* str) {
	int len = 0;
	Deserialize(&len);
	std::string temp = "";
	for(int i = 0; i < len; ++i) {
	  temp += buffer[buffer.size() - 1];
	  buffer.pop_back();
	}
	// Needs optimization
	std::reverse(temp.begin(), temp.end());
	*str = temp;
  }

  // char

  void Serializer::Serialize(char c) {
	buffer.push_back(c);
  }

  void Serializer::Deserialize(char* c) {
	*c = buffer[buffer.size()-1];
	buffer.pop_back();
  }
}
