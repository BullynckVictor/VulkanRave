#pragma once
#include "Engine/Graphics/Device.h"

namespace rv
{
	struct CommandPool
	{
		CommandPool() = default;
		CommandPool(Device& device, const DeviceQueue& graphicsQueue);
		CommandPool(const CommandPool&) = delete;
		CommandPool(CommandPool&& rhs) noexcept;
		~CommandPool();

		CommandPool& operator= (const CommandPool&) = delete;
		CommandPool& operator= (CommandPool&& rhs) noexcept;

		void Release();

		VkCommandPool pool = VK_NULL_HANDLE;
	};
}