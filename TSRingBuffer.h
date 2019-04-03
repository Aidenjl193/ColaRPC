#pragma once
#ifdef _WIN32
	#include<windows.h>
#else
	#include <unistd.h>
	#include <sys/mman.h>
	#include <fcntl.h>
#endif
#include"CopyableAtomic.h"

namespace P2P {
	template< class T >
	class TSRingBuffer {
	public:

		char*						buffer;
		int							length;
		CopyableAtomic<int>  		read;
		CopyableAtomic<int>			write;
		int  						granularity;

		int InitializeBuffer(int size) {
			granularity = sizeof(T);
#ifdef _WIN32
			SYSTEM_INFO si;
			GetSystemInfo(&si);
			DWORD pageSize = si.dwAllocationGranularity;
			length = (size * granularity) + pageSize - ((size * granularity) % pageSize);						//Round up according to the page granularity
			size_t allocSize = length * 2;
			void* ptr = VirtualAlloc(0, allocSize, MEM_RESERVE, PAGE_NOACCESS);									//Reserve possible memory & hope we can claim it should implement a fail case
			VirtualFree(ptr, 0, MEM_RELEASE);
			HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, length, 0);			//Create & mirror the map
			void *pBuf = (void*)MapViewOfFileEx(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, length, ptr);
			MapViewOfFileEx(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, length, (char*)ptr + length);
			buffer = (char*)pBuf;
#else
			int fd;
			int pageSize = getpagesize();
			length = (size * granularity) + pageSize - ((size * granularity) % pageSize);
			// Make an anonymous file and set its size
			fd = shm_open("queue_region", O_RDWR | O_CREAT, 0600);
			ftruncate(fd, length);

			// Ask mmap for an address at a location where we can put both virtual copies of the buffer
			buffer = (char*)mmap(NULL, 2 * length, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

			// Map the buffer at that address
			mmap(buffer, length, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);

			// Now map it again, in the next virtual page
			mmap(buffer + length, length, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
#endif
			return size;
		}

		void Delete() {
			delete[] buffer;
			#ifdef _WIN32

			#else
				shm_unlink("queue_region");
			#endif	
			//Add ‘-lrt’ to your LDFLAGS and remember to shm_unlink() it when you’re done. Everything else stays the same, including the performance.
		}

		bool Put(T* element) {
			if ((write < read && write + granularity >= read) || write + granularity - length >= read)			//Make sure we don't 'lap' or hit the read pointer, this would cause undefined behavior
				return false;
			memcpy(buffer + write, element, granularity);
			write += granularity;
			write -= length * (write > length);																	//If we enter the mirrored buffer, loop back into the first
			return true;
		}

		T Get() {
			T t = T();
			if (read == write)																					//Determine if there's anything to read might just remove this as it should be checked before being called
				return t;
			memcpy(&t, buffer + read, granularity);
			read += granularity;
			read -= length * (read > length);																	//If we enter the mirrored buffer, loop back into the first(should probably just mod it for performance)
			return t;
		}

		void Reset() {
			read = write;
		}

		inline bool CanGet() {
			return read != write;
		}
	};
}
