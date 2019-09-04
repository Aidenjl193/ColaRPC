#include "TaskManager.h"

namespace ColaRPC {
	//Initialize static stuff
	std::vector<TaskThread*> TaskManager::threads;
	std::mutex TaskManager::putMtx;
	std::mutex TaskManager::getMtx;
	TSRingBuffer<Task> TaskManager::commandBuffer;

	TaskManager::~TaskManager() {
		//Clean up threads
		for (auto&& thread : threads) {
			delete thread;
		}
	}

	void TaskManager::initializeThreads(int amt) {
		commandBuffer.initializeBuffer(1);
		//Initialize threads
		for (int i = 0; i < amt; ++i) {
			threads.push_back(new TaskThread());
		}
	}

	void TaskManager::assignTask(Task t) {
		putMtx.lock();
		commandBuffer.put(&t);
		putMtx.unlock();

		//Cycle through each thread in the pool
		for (auto&& thread : threads) {
			//If the thread currently isn't running anything; start it
			if (!thread->threadEnabled) {
				thread->makeThread();
			}
		}
	}

	//Task Thread stuff

	TaskThread::~TaskThread() {
		t.join();
	}

	void TaskThread::work() {
		threadEnabled = true;

		//The main logic of our threads
		while (threadEnabled) {
			while (TaskManager::commandBuffer.canGet()) {
				TaskManager::getMtx.lock();

				if (!TaskManager::commandBuffer.canGet()) { //Make sure another thread hasn't claimed the task
					TaskManager::getMtx.unlock();
					threadEnabled = false;
					return;
				}

				Task task = TaskManager::commandBuffer.get();
				TaskManager::getMtx.unlock();

				(*task.func)(&task.ser);//Run the job with the supplied params

				//Free the memory
				free(task.ser.buffer);
			}

			//If there are no tasks kill the thread so we don't hog the CPU
			if (!TaskManager::commandBuffer.canGet()) {
				threadEnabled = false;
				return;
			}
		}
	}

	void TaskThread::makeThread() {
		if (t.joinable()) { // Shouldn't really happen
			t.join();
		}
		t = std::thread([&]() {work();});
	}
}
