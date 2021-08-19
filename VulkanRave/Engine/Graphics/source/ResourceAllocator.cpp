#include "Engine/Graphics/ResourceAllocator.h"
#include "Engine/Graphics/CommandBuffer.h"

rv::ResourceAllocator::ResourceAllocator(Instance& instance, Device& device, const DeviceQueue& graphicsQueue)
	:
	allocator(instance, device),
	pool(device, graphicsQueue)
{
}

rv::ResourceAllocator::ResourceAllocator(ResourceAllocator&& rhs) noexcept
	:
	allocator(std::move(rhs.allocator)),
	pool(std::move(rhs.pool))
{
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
	return *this;
}

void rv::ResourceAllocator::StageBuffer(Device& device, Buffer& dest, const void* source, u64 size)
{
	Buffer stagingBuffer(device, allocator, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, size, source);
	CopyBuffers(device, stagingBuffer, dest, size);
}

void rv::ResourceAllocator::CopyBuffers(Device& device, Buffer& source, Buffer& dest, u64 size)
{
	DeviceQueue graphicsQueue = device.GetQueue("graphics");
	CommandBuffer command(device, pool);
	command.Begin();
	command.CopyBuffers(source, dest, size);
	command.End();
	command.Submit(graphicsQueue);
	graphicsQueue.Wait();
}

void rv::ResourceAllocator::Release()
{
	allocator.Release();
	pool.Release();
}
