#pragma once
#include "Engine/Graphics/MemoryAllocator.h"
#include "Engine/Graphics/Buffer.h"
#include "Engine/Graphics/Image.h"
#include "Engine/Graphics/CommandPool.h"

namespace rv
{
	struct ResourceAllocator
	{
		ResourceAllocator() = default;
		ResourceAllocator(Instance& instance, Device& device, const DeviceQueue& graphicsQueue);
		ResourceAllocator(const ResourceAllocator&) = delete;
		ResourceAllocator(ResourceAllocator&& rhs) noexcept;
		~ResourceAllocator();

		ResourceAllocator& operator= (const ResourceAllocator&) = delete;
		ResourceAllocator& operator= (ResourceAllocator&& rhs) noexcept;

		void StageBuffer(Device& device, Buffer& dest, const void* source, u64 size);
		void CopyBuffers(Device& device, Buffer& source, Buffer& dest, u64 size);
		void TransitionImageLayout(Device& device, Image& image, VkImageLayout oldLayout, VkImageLayout newLayout);
		void CopyBufferToImage(Device& device, Buffer& source, Image& image, const Size& size, VkImageLayout layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		void Release();

		MemoryAllocator allocator;
		CommandPool pool;
		DeviceQueue graphics;
	};
}