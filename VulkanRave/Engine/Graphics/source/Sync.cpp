#include "Engine/Graphics/Sync.h"
#include "Engine/Graphics/VulkanPointer.h"
#include "Engine/Utilities/Exception.h"

template<>
void rv::destroy(VkSemaphore semaphore)
{
	vkDestroySemaphore(static_device->device, semaphore, nullptr);
}

template<>
void rv::destroy(VkFence fence)
{
	vkDestroyFence(static_device->device, fence, nullptr);
}

rv::Semaphore::Semaphore(Device& device)
{
	VkSemaphoreCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	rv_check_vkr(vkCreateSemaphore(device.device, &createInfo, nullptr, &semaphore));
}

rv::Semaphore::Semaphore(Semaphore&& rhs) noexcept
	:
	semaphore(move(rhs.semaphore))
{
}

rv::Semaphore::~Semaphore()
{
	Release();
}

rv::Semaphore& rv::Semaphore::operator=(Semaphore&& rhs) noexcept
{
	Release();
	semaphore = move(rhs.semaphore);
	return *this;
}

void rv::Semaphore::Release()
{
	release(semaphore);
}

rv::Fence::Fence(Device& device, bool signaled)
{
	VkFenceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
	rv_check_vkr(vkCreateFence(device.device, &createInfo, nullptr, &fence));
}

rv::Fence::Fence(Fence&& rhs) noexcept
	:
	fence(move(rhs.fence))
{
}

rv::Fence::~Fence()
{
	Release();
}

rv::Fence& rv::Fence::operator=(Fence&& rhs) noexcept
{
	Release();
	fence = move(rhs.fence);
	return *this;
}

void rv::Fence::Release()
{
	release(fence);
}

void rv::Fence::Wait(Device& device, uint64 timeout)
{
	rv_assert(fence);
	rv_check_vkr(vkWaitForFences(device.device, 1, &fence, VK_TRUE, timeout));
}

void rv::Fence::Reset(Device& device, uint64 timeout)
{
	rv_assert(fence);
	rv_check_vkr(vkResetFences(device.device, 1, &fence));
}
