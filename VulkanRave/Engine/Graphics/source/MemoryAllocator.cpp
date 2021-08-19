#define VMA_IMPLEMENTATION
#include "Engine/Graphics/MemoryAllocator.h"
#include "Engine/Graphics/VulkanPointer.h"

rv::MemoryAllocator::MemoryAllocator(Instance& instance, Device& device)
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.vulkanApiVersion = instance.app.info.apiVersion;
	allocatorInfo.physicalDevice = device.physicalDevice.device;
	allocatorInfo.device = device.device;
	allocatorInfo.instance = instance.instance;

	vmaCreateAllocator(&allocatorInfo, &allocator);
}

rv::MemoryAllocator::MemoryAllocator(MemoryAllocator&& rhs) noexcept
	:
	allocator(move(rhs.allocator))
{
}

rv::MemoryAllocator::~MemoryAllocator()
{
	Release();
}

rv::MemoryAllocator& rv::MemoryAllocator::operator=(MemoryAllocator&& rhs) noexcept
{
	Release();
	allocator = move(rhs.allocator);
	return *this;
}

rv::MemoryAllocator::operator VmaAllocator()
{
	return allocator;
}

rv::MemoryAllocator::operator const VmaAllocator() const
{
	return allocator;
}

void rv::MemoryAllocator::Release()
{
	if (allocator)
		vmaDestroyAllocator(allocator);
	allocator = nullptr;
}