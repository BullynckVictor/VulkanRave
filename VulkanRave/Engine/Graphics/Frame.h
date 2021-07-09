#pragma once
#include "Graphics/Swapchain.h"
#include "Graphics/Command.h"

namespace rv
{
	struct Frame
	{
		Frame() = default;
		Frame(Device& device);
		Frame(const Frame&) = delete;
		Frame(Frame&& rhs) noexcept;
		~Frame();

		Frame& operator= (const Frame&) = delete;
		Frame& operator= (Frame&& rhs) noexcept;

		void Release();

		bool Begin(Device& device, SwapChain& swap, VkFence* flight);
		void Submit(Device& device, DeviceQueue& graphicsQueue, CommandBuffer& command);
		bool Present(SwapChain& swap, DeviceQueue& presentQueue);

		Semaphore imageAvailable;
		Semaphore renderFinished;
		Fence inFlight;
		u32 image = 0;
	};
}