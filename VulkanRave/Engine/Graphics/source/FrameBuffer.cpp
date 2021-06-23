#include "Graphics/FrameBuffer.h"
#include "Graphics/VulkanPointer.h"
#include "Utilities/Exception.h"

template<>
void rv::destroy(VkFramebuffer frameBuffer)
{
	vkDestroyFramebuffer(static_device->device, frameBuffer, nullptr);
}

rv::FrameBuffer::FrameBuffer(Device& device, RenderPass& pass, const ImageView& view, const Size& size)
{

	VkFramebufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.renderPass = pass.pass;
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &view.view;
	createInfo.width = size.width;
	createInfo.height = size.height;
	createInfo.layers = 1;
	rv_check_vkr(vkCreateFramebuffer(device.device, &createInfo, nullptr, &frameBuffer));
}

rv::FrameBuffer::FrameBuffer(FrameBuffer&& rhs) noexcept
	:
	frameBuffer(move(rhs.frameBuffer))
{
}

rv::FrameBuffer::~FrameBuffer()
{
	Release();
}

rv::FrameBuffer& rv::FrameBuffer::operator=(FrameBuffer&& rhs) noexcept
{
	Release();
	frameBuffer = move(rhs.frameBuffer);
	return *this;
}

void rv::FrameBuffer::Release()
{
	release(frameBuffer);
}