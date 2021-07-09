#include "Graphics/Buffer.h"
#include "Graphics/VulkanPointer.h"
#include "Utilities/Exception.h"

template<>
void rv::destroy(VkBuffer buffer)
{
	vkDestroyBuffer(static_device->device, buffer, nullptr);
}
template<>
void rv::destroy(VkDeviceMemory memory)
{
	vkFreeMemory(static_device->device, memory, nullptr);
}

rv::Buffer::Buffer(Device& device, const VkBufferUsageFlags& usage, const u32& size, const void* source)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	rv_check_vkr(vkCreateBuffer(device.device, &bufferInfo, nullptr, &buffer));

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device.device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = detail::FindMemoryType(device, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT).get();
	rv_check_vkr(vkAllocateMemory(device.device, &allocInfo, nullptr, &memory));

	rv_check_vkr(vkBindBufferMemory(device.device, buffer, memory, 0));

	if (source)
		Copy(device, source, size);
}

rv::Buffer::Buffer(Buffer&& rhs) noexcept
	:
	buffer(move(rhs.buffer)),
	memory(move(rhs.memory))
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
	memory = move(rhs.memory);
	return *this;
}

void* rv::Buffer::Map(Device& device, u32 size, u32 offset)
{
	void* data;
	rv_check_vkr(vkMapMemory(device.device, memory, offset, size, 0, &data));
	return data;
}

void rv::Buffer::Unmap(Device& device)
{
	vkUnmapMemory(device.device, memory);
}

void rv::Buffer::Copy(Device& device, const void* source, u32 size, u32 offset)
{
	void* data = Map(device, size, offset);
	memcpy(data, source, size);
	Unmap(device);
}

void rv::Buffer::Release()
{
	release(buffer);
	release(memory);
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