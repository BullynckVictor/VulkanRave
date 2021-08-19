#pragma once
#include "Engine/Graphics/Device.h"
#include "Engine/Graphics/ImageView.h"
#include "Engine/Graphics/Sync.h"
#include "Engine/Utilities/Vector.h"
#include "Engine/Utilities/Reference.h"

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
		SwapChain(Device& device, VkSurfaceKHR surface, const SwapChainSettings& settings = {}, const Size& windowSize = { 0, 0 });
		SwapChain(const SwapChain&) = delete;
		SwapChain(SwapChain&& rhs) noexcept;
		~SwapChain();

		SwapChain& operator= (const SwapChain&) = delete;
		SwapChain& operator= (SwapChain&& rhs) noexcept;

		void Release();

		uint32 AcquireNextImage(Device& device, ORef<Semaphore> semaphore, ORef<Fence> fence, ORef<bool> recreate, uint64 timeout = std::numeric_limits<u64>::max());
		bool Present(DeviceQueue& presentQueue, u32 index, const VkSemaphore* wait, u32 nWait);

		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		std::vector<VkImage> images;
		std::vector<ImageView> views;
		VkSurfaceFormatKHR format {};
		VkPresentModeKHR presentMode {};
		VkExtent2D extent {};
	};
}