#pragma once
#include "Engine/Graphics/Instance.h"
#include "Engine/Graphics/Device.h"
#include "Engine/Libraries/VulkanMemoryAllocator/vk_mem_alloc.h"

namespace rv
{
	struct MemoryAllocator
	{
		MemoryAllocator() = default;
		MemoryAllocator(Instance& instance, Device& device);
		MemoryAllocator(const MemoryAllocator&) = delete;
		MemoryAllocator(MemoryAllocator&& rhs) noexcept;
		~MemoryAllocator();

		MemoryAllocator& operator= (const MemoryAllocator&) = delete;
		MemoryAllocator& operator= (MemoryAllocator&& rhs) noexcept;

		operator VmaAllocator();
		operator const VmaAllocator() const;

		void Release();

		VmaAllocator allocator = nullptr;
	};
}