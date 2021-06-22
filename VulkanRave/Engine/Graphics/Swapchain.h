#pragma once
#include "Graphics/Device.h"
#include "Graphics/ImageView.h"
#include "Utilities/Vector.h"

namespace rv
{
	struct SwapChainSupportDetails
	{
		SwapChainSupportDetails() = default;
		SwapChainSupportDetails(const PhysicalDevice& device, VkSurfaceKHR surface);

		VkSurfaceCapabilitiesKHR capabilities{};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
	};

	struct SwapChainSettings
	{
		VkSurfaceFormatKHR format{};
		VkPresentModeKHR presentMode{};
		uint nImages = 0;
		VkCompositeAlphaFlagBitsKHR alpha{};
	};

	SwapChainSettings DefaultSwap(bool vsync = false);

	struct SwapChain
	{
		SwapChain() = default;
		SwapChain(Device& device, VkSurfaceKHR surface, const SwapChainSettings& settings = {}, const Size& windowSize = 0);
		SwapChain(const SwapChain&) = delete;
		SwapChain(SwapChain&& rhs) noexcept;
		~SwapChain();

		SwapChain& operator= (const SwapChain&) = delete;
		SwapChain& operator= (SwapChain&& rhs) noexcept;

		void Release();

		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		std::vector<VkImage> images;
		std::vector<ImageView> views;
		VkSurfaceFormatKHR format {};
		VkPresentModeKHR presentMode {};
		VkExtent2D extent {};
	};
}