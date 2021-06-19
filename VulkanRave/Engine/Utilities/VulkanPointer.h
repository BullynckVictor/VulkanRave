#pragma once
#include "Graphics/Vulkan.h"

namespace rv
{
	template<typename T>
	void destroy(T pointer) { static_assert(false, "rv::destroy<T>() must be implemented"); }

	template<typename T>
	void release(T& pointer)
	{
		if (pointer)
			destroy(pointer);
		pointer = VK_NULL_HANDLE;
	}

	template<typename T>
	T&& move(T&& pointer)
	{
		release(pointer);
		return pointer;
	}
}