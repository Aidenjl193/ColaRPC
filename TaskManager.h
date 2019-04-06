#pragma once
#include <cstdlib>
#include <thread>
#include <vector>
#include <iostream>
#include <map>
#include <mutex>
#include "TSRingBuffer.h"

namespace P2P {
	struct Task {
	public:
	  void(*function)(char*, int);
	  char* data;
	  int len;
	};

	class TaskThread {
	private:
		std::thread t;

	public:
		//Constructors and de-constructors
		~TaskThread();

		//Variables

		volatile bool threadEnabled = false;

		//Member functions
		void MakeThread();

		void Work();
	};


	class TaskManager {
	public:
		//Deconstructor
		~TaskManager();

		//Variables

		//Will hold all of the threads we can use

		static std::vector<TaskThread*> threads;

		static TSRingBuffer<Task> commandBuffer;

		static std::mutex getMtx;
		static std::mutex putMtx;

		//Member functions

		static void InitializeThreads(int amt);

		static void AssignTask(Task t);
	};
}
