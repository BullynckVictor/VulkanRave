#include "Graphics/Command.h"
#include "Graphics/VulkanPointer.h"
#include "Utilities/Exception.h"

template<>
void rv::destroy(VkCommandPool pool)
{
	vkDestroyCommandPool(static_device->device, pool, nullptr);
}

rv::CommandPool::CommandPool(Device& device, const DeviceQueue& graphicsQueue)
{
	VkCommandPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = graphicsQueue.index;
	createInfo.flags = 0;
	rv_check_vkr(vkCreateCommandPool(device.device, &createInfo, nullptr, &pool));
}

rv::CommandPool::CommandPool(CommandPool&& rhs) noexcept
	:
	pool(move(pool))
{
}

rv::CommandPool::~CommandPool()
{
	Release();
}

rv::CommandPool& rv::CommandPool::operator=(CommandPool&& rhs) noexcept
{
	Release();
	pool = move(rhs.pool);
	return *this;
}

void rv::CommandPool::Release()
{
	release(pool);
}

rv::CommandBuffer::CommandBuffer(Device& device, CommandPool& pool)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = pool.pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	rv_check_vkr(vkAllocateCommandBuffers(device.device, &allocInfo, &buffer));
}

rv::CommandBuffer::CommandBuffer(CommandBuffer&& rhs) noexcept
	:
	buffer(move(rhs.buffer))
{
}

rv::CommandBuffer::~CommandBuffer()
{
	Release();
}

rv::CommandBuffer& rv::CommandBuffer::operator=(CommandBuffer&& rhs) noexcept
{
	Release();
	buffer = move(rhs.buffer);
	return *this;
}

void rv::CommandBuffer::Release()
{
	buffer = VK_NULL_HANDLE;
}

void rv::CommandBuffer::Free(Device& device, CommandPool& pool)
{
	vkFreeCommandBuffers(device.device, pool.pool, 1, &buffer);
}

void rv::CommandBuffer::Begin()
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	rv_check_vkr(vkBeginCommandBuffer(buffer, &beginInfo));
}

void rv::CommandBuffer::BeginPass(RenderPass& pass, FrameBuffer& frameBuffer, const Size& size, std::optional<FColor> color)
{
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = pass.pass;
	renderPassInfo.framebuffer = frameBuffer.frameBuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = { size.width, size.height };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = color.has_value() ? reinterpret_cast<const VkClearValue*>(&color.value()) : nullptr;
	vkCmdBeginRenderPass(buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void rv::CommandBuffer::BindPipeline(Pipeline& pipeline, PipelineType type)
{
	VkPipelineBindPoint bind{};
	if (type == RV_PT_GRAPHICS)
		bind = VK_PIPELINE_BIND_POINT_GRAPHICS;
	else if (type == RV_PT_COMPUTE)
		bind = VK_PIPELINE_BIND_POINT_COMPUTE;
	vkCmdBindPipeline(buffer, bind, pipeline.pipeline);
}

void rv::CommandBuffer::BindVertexBuffer(VertexBuffer& vertices, u64 offset)
{
	vkCmdBindVertexBuffers(buffer, 0, 1, &vertices.buffer, &offset);
}

void rv::CommandBuffer::Draw(u32 nVertices, u32 nInstances, u32 vertexOffset, u32 instanceOffset)
{
	vkCmdDraw(buffer, nVertices, nInstances, vertexOffset, instanceOffset);
}

void rv::CommandBuffer::EndPass()
{
	vkCmdEndRenderPass(buffer);
}

void rv::CommandBuffer::End()
{
	rv_check_vkr(vkEndCommandBuffer(buffer));
}

void rv::CommandBuffer::Submit(
	DeviceQueue& queue, 
	const VkSemaphore* wait, 
	const VkPipelineStageFlags* stages, 
	u32 nWait, 
	const VkSemaphore* signals, 
	u32 nSignals,
	Fence* fence
)
{
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = nWait;
	submitInfo.pWaitSemaphores = wait;
	submitInfo.pWaitDstStageMask = stages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &buffer;
	submitInfo.signalSemaphoreCount = nSignals;
	submitInfo.pSignalSemaphores = signals;
	rv_check_vkr(vkQueueSubmit(queue.queue, 1, &submitInfo, fence ? fence->fence : VK_NULL_HANDLE));
}
