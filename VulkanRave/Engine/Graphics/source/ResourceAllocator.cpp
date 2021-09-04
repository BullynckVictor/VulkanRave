#include "Engine/Graphics/ResourceAllocator.h"
#include "Engine/Graphics/CommandBuffer.h"

rv::ResourceAllocator::ResourceAllocator(Instance& instance, Device& device, const DeviceQueue& graphicsQueue)
	:
	allocator(instance, device),
	pool(device, graphicsQueue),
	graphics(graphicsQueue)
{
}

rv::ResourceAllocator::ResourceAllocator(ResourceAllocator&& rhs) noexcept
	:
	allocator(std::move(rhs.allocator)),
	pool(std::move(rhs.pool)),
	graphics(std::move(rhs.graphics))
{
	rhs.graphics.index = 0;
	rhs.graphics.queue = VK_NULL_HANDLE;
}

rv::ResourceAllocator::~ResourceAllocator()
{
	Release();
}

rv::ResourceAllocator& rv::ResourceAllocator::operator=(ResourceAllocator&& rhs) noexcept
{
	Release();
	allocator = std::move(rhs.allocator);
	pool = std::move(rhs.pool);
	graphics = std::move(rhs.graphics);
	rhs.graphics.index = 0;
	rhs.graphics.queue = VK_NULL_HANDLE;
	return *this;
}

void rv::ResourceAllocator::StageBuffer(Device& device, Buffer& dest, const void* source, u64 size)
{
	Buffer stagingBuffer(device, allocator, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, size, source);
	CopyBuffers(device, stagingBuffer, dest, size);
}

void rv::ResourceAllocator::CopyBuffers(Device& device, Buffer& source, Buffer& dest, u64 size)
{
	CommandBuffer command(device, pool);
	command.Begin();
	command.CopyBuffers(source, dest, size);
	command.End();
	command.Submit(graphics);
	graphics.Wait();
}

void rv::ResourceAllocator::TransitionImageLayout(Device& device, Image& image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	CommandBuffer command(device, pool);
	command.Begin();
	command.TransitionImageLayout(image, oldLayout, newLayout);
	command.End();
	command.Submit(graphics);
	graphics.Wait();
}

void rv::ResourceAllocator::CopyBufferToImage(Device& device, Buffer& source, Image& image, const Size& size, VkImageLayout layout)
{
	CommandBuffer command(device, pool);
	command.Begin();
	command.CopyBufferToImage(source, image, size, layout);
	command.End();
	command.Submit(graphics);
	graphics.Wait();
}

void rv::ResourceAllocator::Release()
{
	allocator.Release();
	pool.Release();
	graphics.index = 0;
	graphics.queue = VK_NULL_HANDLE;
}
