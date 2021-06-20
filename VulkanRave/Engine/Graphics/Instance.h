#pragma once
#include "Graphics/Vulkan.h"
#include "Utilities/Types.h"
#include <vector>

namespace rv
{
	struct AppInfo
	{
		AppInfo(const char* name = "VulkanRave Application", uint32 version = VK_MAKE_VERSION(1, 0, 0));

		VkApplicationInfo info;
		std::vector<const char*> layers;
		std::vector<const char*> extensions;
	};

	struct Instance
	{
		Instance(const AppInfo& app = {});
		Instance(const Instance&) = delete;
		Instance(Instance&& rhs) noexcept;
		~Instance() noexcept;

		Instance& operator= (const Instance&) = delete;
		Instance& operator= (Instance&& rhs) noexcept;

		void Release();

		VkInstance instance = VK_NULL_HANDLE;
		std::vector<const char*> validation;
	};

	extern Instance* static_instance;
}