#pragma once
#include "General/SystemInclude.h"
#ifdef RV_PLATFORM_WINDOWS
#	define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan/vulkan.h"