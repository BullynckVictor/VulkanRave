#include "Engine/Graphics/ImageView.h"
#include "Engine/Graphics/VulkanPointer.h"
#include "Engine/Utilities/Exception.h"

template<>
void rv::destroy<VkImageView>(VkImageView view)
{
	vkDestroyImageView(static_device->device, view, nullptr);
}

rv::ImageView::ImageView(Device& device, VkImage image, VkFormat format)
{
	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;
	rv_check_vkr(vkCreateImageView(device.device, &createInfo, nullptr, &view));
}

rv::ImageView::ImageView(ImageView&& rhs) noexcept
	:
	view(move(rhs.view))
{
}

rv::ImageView::~ImageView()
{
	Release();
}

rv::ImageView& rv::ImageView::operator=(ImageView&& rhs) noexcept
{
	Release();
	view = move(rhs.view);
	return *this;
}

void rv::ImageView::Release()
{
	release(view);
}
