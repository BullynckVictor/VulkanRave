#pragma once
#include "Engine/Graphics/Device.h"
#include "Engine/Utilities/Reference.h"
#include <optional>

namespace rv
{
	struct Subpass
	{
		void AddColor(const VkAttachmentDescription& attachment);
		void AddInput(const VkAttachmentDescription& attachment);
		void AddResolve(const VkAttachmentDescription& attachment);
		void AddPreserve(const VkAttachmentDescription& attachment);
		void SetDepth(const VkAttachmentDescription& attachment);

		void Finalize();
		VkSubpassDescription Finalized() const;

		VkSubpassDescription subpass = {};
		std::vector<VkAttachmentReference> color;
		std::vector<VkAttachmentReference> input;
		std::vector<VkAttachmentReference> resolve;
		std::vector<VkAttachmentReference> preserve;
		std::optional<VkAttachmentReference> depth = {};
		ORef<std::vector<VkAttachmentDescription>> attachments;
	};

	struct RenderPassDescriptor
	{
		uint32 CreateSubpass();
		std::vector<VkAttachmentDescription> attachments;
		std::vector<Subpass> subpasses;
		std::vector<VkSubpassDependency> dependencies;
	};

	struct RenderPass
	{
		RenderPass() = default;
		RenderPass(Device& device, const RenderPassDescriptor& descriptor = {});
		RenderPass(const RenderPass&) = delete;
		RenderPass(RenderPass&& rhs) noexcept;
		~RenderPass();

		RenderPass& operator= (const RenderPass&) = delete;
		RenderPass& operator= (RenderPass&& rhs) noexcept;

		void Release();

		VkRenderPass pass = VK_NULL_HANDLE;
	};

	namespace attachments
	{
		VkAttachmentDescription Color(const VkFormat& format, const VkImageLayout& layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		VkAttachmentDescription Clear(const VkFormat& format, const VkImageLayout& layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	}
	namespace dependencies
	{
		VkSubpassDependency Color(u32 index);
	}
}