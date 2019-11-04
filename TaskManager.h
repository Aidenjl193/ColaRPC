#pragma once
#include <cstdlib>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include "Serializer.h"
#include "TSRingBuffer.h"
#include "Value.h"

namespace ColaRPC {
// Can't include function and compile / probably a circular dependancy
typedef std::function<Value(Serializer*)> Function;

struct Task {
   public:
	Function* func;
	Serializer ser;
};

class TaskThread {
   private:
	std::thread t;

   public:
	// Constructors and de-constructors
	~TaskThread();

	// Variables

	volatile bool threadEnabled = false;

	// Member functions
	void makeThread();

	void work();
};

class TaskManager {
   public:
	// Deconstructor
	~TaskManager();

	// Variables

	// Will hold all of the threads we can use

	static std::vector<TaskThread*> threads;

	static TSRingBuffer<Task> commandBuffer;

	static std::mutex getMtx;
	static std::mutex putMtx;

	// Member functions

	static void initializeThreads(int amt);

	static void assignTask(Task t);
};
}  // namespace ColaRPC
