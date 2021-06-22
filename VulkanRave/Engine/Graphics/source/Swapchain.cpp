#include "Graphics/Swapchain.h"
#include "Graphics/VulkanPointer.h"
#include "Utilities/Exception.h"
#include "General/Logger.h"
#include <algorithm>

template<>
void rv::destroy<VkSwapchainKHR>(VkSwapchainKHR swap)
{
	vkDestroySwapchainKHR(static_device->device, swap, nullptr);
}

rv::SwapChainSupportDetails::SwapChainSupportDetails(const PhysicalDevice& device, VkSurfaceKHR surface)
	:
	surface(surface)
{
	rv_check_vkr(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.device, surface, &capabilities));

	uint32 nFormats;
	rv_check_vkr(vkGetPhysicalDeviceSurfaceFormatsKHR(device.device, surface, &nFormats, nullptr));
	if (nFormats)
	{
		formats.resize(nFormats);
		rv_check_vkr(vkGetPhysicalDeviceSurfaceFormatsKHR(device.device, surface, &nFormats, formats.data()));
	}

	uint32 nPresentModes;
	rv_check_vkr(vkGetPhysicalDeviceSurfacePresentModesKHR(device.device, surface, &nPresentModes, nullptr));
	if (nPresentModes)
	{
		presentModes.resize(nPresentModes);
		rv_check_vkr(vkGetPhysicalDeviceSurfacePresentModesKHR(device.device, surface, &nPresentModes, presentModes.data()));
	}
}

rv::SwapChain::SwapChain(Device& device, VkSurfaceKHR surface, const SwapChainSettings& settings, const Size& windowSize)
	:
	surface(surface)
{
	SwapChainSupportDetails support(device.physicalDevice, surface);

	//format
	{
		rv_assert(!support.formats.empty());
		bool format_found = false;
		for (const auto& availableFormat : support.formats)
			if (availableFormat.colorSpace == settings.format.colorSpace && availableFormat.format == settings.format.format)
			{
				format = availableFormat;
				format_found = true;
				break;
			}
		if (!format_found) format = support.formats[0];
	}

	//present mode
	{
		rv_assert(!support.presentModes.empty());
		bool found = false;
		for (const auto& pmode : support.presentModes)
			if (pmode == settings.presentMode)
			{
				presentMode = pmode;
				found = true;
				break;
			}
		if (!found) presentMode = VK_PRESENT_MODE_FIFO_KHR;
	}

	//extent
	{
		if (support.capabilities.currentExtent.width != UINT32_MAX)
			extent = support.capabilities.currentExtent;
		else
		{
			extent.width = std::clamp(windowSize.width, support.capabilities.minImageExtent.width, support.capabilities.maxImageExtent.width);
			extent.height = std::clamp(windowSize.height, support.capabilities.minImageExtent.height, support.capabilities.maxImageExtent.height);
		}
	}

	uint nImages = std::max(settings.nImages, support.capabilities.minImageCount);
	if (support.capabilities.maxImageCount != 0)
		nImages = std::min(nImages, support.capabilities.maxImageCount);

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = nImages;
	createInfo.imageFormat = format.format;
	createInfo.imageColorSpace = format.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform = support.capabilities.currentTransform;
	createInfo.compositeAlpha = settings.alpha;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE; 
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	OIndex indices[] = { 
		device.physicalDevice.QueueFamilyIndex({ QueueContainsFlag, VK_QUEUE_GRAPHICS_BIT }),
		device.physicalDevice.QueueFamilyIndex({ QueueCanPresent, surface }),
	};

	rv_assert(indices[0].valid());
	rv_assert(indices[1].valid());

	if (indices[0] != indices[1]) 
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = reinterpret_cast<const u32*>(indices);
	}
	else 
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	rv_check_vkr(vkCreateSwapchainKHR(device.device, &createInfo, nullptr, &swapchain));

	debug.Log(str("Created Swapchain with ", nImages, " image", nImages == 1 ? "" : "s"));

	rv_check_vkr(vkGetSwapchainImagesKHR(device.device, swapchain, &nImages, nullptr));
	images.resize(nImages);
	rv_check_vkr(vkGetSwapchainImagesKHR(device.device, swapchain, &nImages, images.data()));

	views.resize(nImages);
	std::transform(images.begin(), images.end(), views.begin(), [&device, this](VkImage image) { return ImageView(device, image, format.format); });
}

rv::SwapChain::SwapChain(SwapChain&& rhs) noexcept
	:
	swapchain(move(rhs.swapchain)),
	surface(rhs.surface),
	images(std::move(rhs.images)),
	views(std::move(rhs.views)),
	format(rhs.format),
	presentMode(rhs.presentMode),
	extent(rhs.extent)
{
}

rv::SwapChain::~SwapChain()
{
	Release();
}

rv::SwapChain& rv::SwapChain::operator=(SwapChain&& rhs) noexcept
{
	Release();
	swapchain = move(rhs.swapchain);
	images = std::move(rhs.images);
	views = std::move(rhs.views);
	surface = rhs.surface;
	format = rhs.format;
	presentMode = rhs.presentMode;
	extent = rhs.extent;
	rhs.surface = VK_NULL_HANDLE;
	rhs.format = {};
	rhs.presentMode = {};
	rhs.extent = {};
	images.clear();
	views.clear();
	return *this;
}

void rv::SwapChain::Release()
{
	release(swapchain);
	surface = VK_NULL_HANDLE;
	format = {};
	presentMode = {};
	extent = {};
	images.clear();
	views.clear();
}

rv::SwapChainSettings rv::DefaultSwap(bool vsync)
{
	SwapChainSettings settings;
	settings.alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	settings.format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	settings.format.format = VK_FORMAT_B8G8R8A8_SRGB;
	settings.presentMode = vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR;
	settings.nImages = 2;
	return settings;
}
