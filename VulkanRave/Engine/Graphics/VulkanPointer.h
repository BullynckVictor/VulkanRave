#pragma once
#include "Utilities/VulkanPointer.h"
#include "Graphics/Instance.h"
#include "Graphics/Device.h"

namespace rv
{
	template<>
	static void rv::destroy<VkSurfaceKHR>(VkSurfaceKHR surface) { vkDestroySurfaceKHR(static_instance->instance, surface, nullptr); }

	namespace vk
	{
		typedef VkPointer<VkSurfaceKHR> Surface;
	}
}