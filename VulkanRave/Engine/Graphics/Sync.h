#pragma once
#include "Graphics/Device.h"

namespace rv
{
	struct Semaphore
	{
		Semaphore() = default;
		Semaphore(Device& device);
		Semaphore(const Semaphore&) = delete;
		Semaphore(Semaphore&& rhs) noexcept;
		~Semaphore();

		Semaphore& operator= (const Semaphore&) = delete;
		Semaphore& operator= (Semaphore&& rhs) noexcept;

		void Release();

		VkSemaphore semaphore = VK_NULL_HANDLE;
	};

	struct Fence
	{
		Fence() = default;
		Fence(Device& device, bool signaled = false);
		Fence(const Fence&) = delete;
		Fence(Fence&& rhs) noexcept;
		~Fence();

		Fence& operator= (const Fence&) = delete;
		Fence& operator= (Fence&& rhs) noexcept;

		void Release();

		void Wait(Device& device, uint64 timeout = std::numeric_limits<u64>::max());
		void Reset(Device& device, uint64 timeout = std::numeric_limits<u64>::max());

		VkFence fence = VK_NULL_HANDLE;
	};
}