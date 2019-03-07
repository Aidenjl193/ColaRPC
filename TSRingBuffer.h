#pragma once
#include<windows.h>
#include"CopyableAtomic.h"
namespace P2P {
	template< class type >
	class TSRingBuffer {
		typedef type				new_t(void);

	public:

		char*						buffer;
		__int32						length;
		CopyableAtomic<__int32>		read;
		CopyableAtomic<__int32>		write;
		__int32						granularity;

		int InitializeBuffer(__int32 size) {
			granularity = sizeof(type);
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
			return size;
		}

		bool Deposit(type* element) {
			if ((write < read && write + granularity >= read) || write + granularity - length >= read)			//Make sure we don't 'lap' or hit the read pointer, this would cause undefined behavior
				return false;
			memcpy(buffer + write, element, granularity);
			write += granularity;
			write -= length * (write > length);																	//If we enter the mirrored buffer, loop back into the first
			return true;
		}

		type Get() {
			type t = type();
			if (read == write)																					//Determine if there's anything to read might just remove this as it should be checked before being called
				return t;
			memcpy(&t, buffer + read, granularity);
			read += granularity;
			read -= length * (read > length);																	//If we enter the mirrored buffer, loop back into the first
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