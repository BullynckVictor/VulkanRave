#pragma once
#include "Utilities/VulkanPointer.h"

namespace rv
{
	template<>
	void destroy<VkInstance>(VkInstance instance)
	{
		vkDestroyInstance(instance, nullptr);
	}
}