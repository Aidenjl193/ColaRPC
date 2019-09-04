#pragma once
#include<atomic>

namespace ColaRPC {
	template<class T>
	class CopyableAtomic : public std::atomic<T> {
	public:
		CopyableAtomic() = default;

		constexpr CopyableAtomic(T desired) : std::atomic<T>(desired) {}

		constexpr CopyableAtomic(const CopyableAtomic<T>& other) : CopyableAtomic(other.load(std::memory_order_relaxed)) {}

		CopyableAtomic& operator=(const CopyableAtomic<T>& other) {
			this->store(other.load(std::memory_order_acquire), std::memory_order_release);
			return *this;
		}
	};
}
