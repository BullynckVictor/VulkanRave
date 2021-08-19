#pragma once
#include "Engine/Graphics/Device.h"
#include "Engine/Graphics/Allocation.h"

namespace rv
{
	struct Buffer
	{
		Buffer() = default;
		Buffer(Device& device, VmaAllocator allocator, const VkBufferUsageFlags& usage, const VmaMemoryUsage& mem, const u64& size, const void* source = nullptr);
		Buffer(const Buffer&) = delete;
		Buffer(Buffer&& rhs) noexcept;
		~Buffer();

		Buffer& operator= (const Buffer&) = delete;
		Buffer& operator= (Buffer&& rhs) noexcept;

		void* Map(Device& device);
		void  Unmap(Device& device);
		void Copy(Device& device, const void* source, u64 size = 0, u64 offset = 0);

		void Release();

		VkBuffer buffer = VK_NULL_HANDLE;
		Allocation memory;
	};

	namespace detail
	{
		OIndex FindMemoryType(Device& device, u32 typeFilter, VkMemoryPropertyFlags properties);
	}
}