#include "Engine/Graphics/Image.h"
#include "Engine/Graphics/VulkanPointer.h"
#include "Engine/Utilities/Exception.h"

template<>
void rv::destroy(VkImage image)
{
	vkDestroyImage(static_device->device, image, nullptr);
}

rv::Image::Image(
	Device& device,
	MemoryAllocator& allocator,
	const Size& size,
	VkFormat format,
	VkImageUsageFlags usage,
	VmaMemoryUsage memoryUsage,
	VkImageTiling tiling
)
	:
	format(format),
	memory(allocator)
{
	VkImageCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.extent.width = size.x;
	createInfo.extent.height = size.y;
	createInfo.extent.depth = 1;
	createInfo.mipLevels = 1;
	createInfo.arrayLayers = 1;
	createInfo.format = format;
	createInfo.tiling = tiling;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	createInfo.usage = usage;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.flags = 0; // Optional

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = memoryUsage;

	rv_check_vkr(vmaCreateImage(
		allocator.allocator,
		&createInfo,
		&allocInfo,
		&image,
		&memory.allocation,
		nullptr
	));
}

rv::Image::Image(Image&& rhs) noexcept
	:
	view(std::move(rhs.view)),
	image(move(rhs.image)),
	format(rhs.format),
	memory(std::move(rhs.memory))
{
	rhs.format = {};
}

rv::Image::~Image()
{
	Release();
}

rv::Image& rv::Image::operator=(Image&& rhs) noexcept
{
	Release();
	view = std::move(rhs.view);
	memory = std::move(rhs.memory);
	image = move(rhs.image);
	format = rhs.format;
	rhs.format = {};
	return *this;
}

void rv::Image::Release()
{
	view.Release();
	memory.Release();
	release(image);
	format = {};
}