#pragma once
#include "Engine/Graphics/Device.h"

namespace rv
{
	struct Buffer
	{
		Buffer() = default;
		Buffer(Device& device, const VkBufferUsageFlags& usage, const u32& size, const void* source = nullptr);
		Buffer(const Buffer&) = delete;
		Buffer(Buffer&& rhs) noexcept;
		~Buffer();

		Buffer& operator= (const Buffer&) = delete;
		Buffer& operator= (Buffer&& rhs) noexcept;

		void* Map(Device& device, u32 size, u32 offset = 0);
		void  Unmap(Device& device);
		void Copy(Device& device, const void* source, u32 size, u32 offset = 0);

		void Release();

		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
	};

	namespace detail
	{
		OIndex FindMemoryType(Device& device, u32 typeFilter, VkMemoryPropertyFlags properties);
	}
}