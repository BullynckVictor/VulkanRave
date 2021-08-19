#include "Engine/Graphics/RenderPass.h"
#include "Engine/Graphics/VulkanPointer.h"
#include "Engine/Utilities/Exception.h"
#include <algorithm>

template<>
void rv::destroy(VkRenderPass pass)
{
	vkDestroyRenderPass(static_device->device, pass, nullptr);
}

rv::RenderPass::RenderPass(Device& device, const RenderPassDescriptor& descriptor)
{
	std::vector<VkSubpassDescription> subpasses(descriptor.subpasses.size());
	std::transform(descriptor.subpasses.begin(), descriptor.subpasses.end(), subpasses.begin(), 
		[] (const Subpass& pass) {
			return pass.Finalized();
		}
	);

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = (u32)descriptor.attachments.size();
	renderPassInfo.pAttachments = descriptor.attachments.data();
	renderPassInfo.subpassCount = (u32)subpasses.size();
	renderPassInfo.pSubpasses = subpasses.data();
	renderPassInfo.dependencyCount = (u32)descriptor.dependencies.size();
	renderPassInfo.pDependencies = descriptor.dependencies.data();

	rv_check_vkr(vkCreateRenderPass(device.device, &renderPassInfo, nullptr, &pass));
}

rv::RenderPass::RenderPass(RenderPass&& rhs) noexcept
	:
	pass(move(rhs.pass))
{
}

rv::RenderPass::~RenderPass()
{
	Release();
}

rv::RenderPass& rv::RenderPass::operator=(RenderPass&& rhs) noexcept
{
	Release();
	pass = move(rhs.pass);
	return *this;
}

void rv::RenderPass::Release()
{
	release(pass);
}

void rv::Subpass::AddColor(const VkAttachmentDescription& attachment)
{
	VkAttachmentReference ref{};
	ref.attachment = (u32)attachments.get().size();
	ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	color.push_back(ref);
	attachments.get().push_back(attachment);
}

void rv::Subpass::AddInput(const VkAttachmentDescription& attachment)
{
	VkAttachmentReference ref{};
	ref.attachment = (u32)attachments.get().size();
	ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	input.push_back(ref);
	attachments.get().push_back(attachment);
}

void rv::Subpass::AddResolve(const VkAttachmentDescription& attachment)
{
	VkAttachmentReference ref{};
	ref.attachment = (u32)attachments.get().size();
	ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	resolve.push_back(ref);
	attachments.get().push_back(attachment);
}

void rv::Subpass::AddPreserve(const VkAttachmentDescription& attachment)
{
	VkAttachmentReference ref{};
	ref.attachment = (u32)attachments.get().size();
	ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	preserve.push_back(ref);
	attachments.get().push_back(attachment);
}

void rv::Subpass::SetDepth(const VkAttachmentDescription& attachment)
{
	depth = VkAttachmentReference{};
	depth.value().attachment = (u32)attachments.get().size();
	depth.value().layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachments.get().push_back(attachment);
}

void rv::Subpass::Finalize()
{
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = (u32)color.size();
	subpass.pColorAttachments = color.data();
	subpass.inputAttachmentCount = (u32)input.size();
	subpass.pInputAttachments = input.data();
	subpass.pResolveAttachments = resolve.data();
	subpass.pDepthStencilAttachment = depth ? &depth.value() : nullptr;
}

VkSubpassDescription rv::Subpass::Finalized() const
{
	VkSubpassDescription desc{};
	desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	desc.colorAttachmentCount = (u32)color.size();
	desc.pColorAttachments = color.data();
	desc.inputAttachmentCount = (u32)input.size();
	desc.pInputAttachments = input.data();
	desc.pResolveAttachments = resolve.data();
	desc.pDepthStencilAttachment = depth ? &depth.value() : nullptr;
	return desc;
}

rv::uint32 rv::RenderPassDescriptor::CreateSubpass()
{
	uint32 size = (u32)subpasses.size();
	subpasses.emplace_back().attachments = attachments;
	return size;
}

VkAttachmentDescription rv::attachments::Color(const VkFormat& format, const VkImageLayout& layout)
{
	VkAttachmentDescription attachment{};
	attachment.format = format;
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment.finalLayout = layout;
	return attachment;
}

VkAttachmentDescription rv::attachments::Clear(const VkFormat& format, const VkImageLayout& layout)
{
	VkAttachmentDescription attachment{};
	attachment.format = format;
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment.finalLayout = layout;
	return attachment;
}

VkSubpassDependency rv::dependencies::Color(u32 index)
{
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = index;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	return dependency;
}
