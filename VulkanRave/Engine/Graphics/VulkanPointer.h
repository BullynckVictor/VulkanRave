#pragma once
#include "Engine/Utilities/VulkanPointer.h"
#include "Engine/Graphics/Instance.h"
#include "Engine/Graphics/Device.h"

namespace rv
{
	template<>
	static void rv::destroy<VkSurfaceKHR>(VkSurfaceKHR surface) { vkDestroySurfaceKHR(static_instance->instance, surface, nullptr); }

	namespace vk
	{
		typedef VkPointer<VkSurfaceKHR> Surface;
	}
}