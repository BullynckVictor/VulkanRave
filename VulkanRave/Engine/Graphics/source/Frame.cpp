#include "Engine/Graphics/Frame.h"
#include "Engine/Graphics/VulkanPointer.h"

rv::Frame::Frame(Device& device)
	:
	imageAvailable(device),
	renderFinished(device),
	inFlight(device, true)
{
}

rv::Frame::Frame(Frame&& rhs) noexcept
	:
	imageAvailable(std::move(rhs.imageAvailable)),
	renderFinished(std::move(rhs.renderFinished)),
	inFlight(std::move(rhs.inFlight)),
	image(rhs.image)
{
	rhs.image = 0;
}

rv::Frame::~Frame()
{
	Release();
}

rv::Frame& rv::Frame::operator=(Frame&& rhs) noexcept
{
	Release();
	imageAvailable = std::move(rhs.imageAvailable);
	renderFinished = std::move(rhs.renderFinished);
	inFlight = std::move(rhs.inFlight);
	image = rhs.image;
	rhs.image = 0;
	return *this;
}

void rv::Frame::Release()
{
	if (inFlight.fence)
		inFlight.Wait(*static_device);
	imageAvailable.Release();
	renderFinished.Release();
	inFlight.Release();
}

bool rv::Frame::Begin(Device& device, SwapChain& swap, VkFence* flight)
{
	bool recreate = false;
	inFlight.Wait(device);
	image = swap.AcquireNextImage(device, imageAvailable, {}, recreate);
	if (!recreate)
	{
		if (flight[image] != VK_NULL_HANDLE)
			reinterpret_cast<Fence&>(flight[image]).Wait(device);
		flight[image] = inFlight.fence;
	}
	return recreate;
}

void rv::Frame::Submit(Device& device, DeviceQueue& graphicsQueue, CommandBuffer& command)
{
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	inFlight.Reset(device);
	command.Submit(graphicsQueue, &imageAvailable.semaphore, waitStages, 1, &renderFinished.semaphore, 1, &inFlight);
}

bool rv::Frame::Present(SwapChain& swap, DeviceQueue& presentQueue)
{
	return swap.Present(presentQueue, image, &renderFinished.semaphore, 1);
}
