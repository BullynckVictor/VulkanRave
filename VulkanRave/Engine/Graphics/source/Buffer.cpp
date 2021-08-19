#include "Engine/Graphics/Buffer.h"
#include "Engine/Graphics/VulkanPointer.h"
#include "Engine/Utilities/Exception.h"

template<>
void rv::destroy(VkBuffer buffer)
{
	vkDestroyBuffer(static_device->device, buffer, nullptr);
}

rv::Buffer::Buffer(Device& device, VmaAllocator allocator, const VkBufferUsageFlags& usage, const VmaMemoryUsage& mem, const u64& size, const void* source)
	:
	memory(allocator)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = mem;
	vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &memory.allocation, nullptr);

	if (source)
		Copy(device, source, size);
}

rv::Buffer::Buffer(Buffer&& rhs) noexcept
	:
	buffer(move(rhs.buffer)),
	memory(std::move(rhs.memory))
{
}

rv::Buffer::~Buffer()
{
	Release();
}

rv::Buffer& rv::Buffer::operator=(Buffer&& rhs) noexcept
{
	Release();
	buffer = move(rhs.buffer);
	memory = std::move(rhs.memory);
	return *this;
}

void* rv::Buffer::Map(Device& device)
{
	void* data;
	rv_check_vkr(vmaMapMemory(memory.allocator, memory.allocation, &data));
	return data;
}

void rv::Buffer::Unmap(Device& device)
{
	vmaUnmapMemory(memory.allocator, memory.allocation);
}

void rv::Buffer::Copy(Device& device, const void* source, u64 size, u64 offset)
{
	void* data = Map(device);
	memcpy(data, reinterpret_cast<const char*>(source) + offset, size ? size : memory.Info().size);
	Unmap(device);
}

void rv::Buffer::Release()
{
	release(buffer);
	memory.Release();
}

rv::OIndex rv::detail::FindMemoryType(Device& device, u32 typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(device.physicalDevice.device, &memProperties);

	for (u32 i = 0; i < memProperties.memoryTypeCount; i++)
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	return {};
}