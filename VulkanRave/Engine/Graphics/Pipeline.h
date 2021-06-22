#pragma once
#include "Graphics/Device.h"
#include "Graphics/Shader.h"
#include "Graphics/RenderPass.h"
#include "Utilities/Vector.h"

namespace rv
{
	struct PipelineLayout
	{
		PipelineLayout();
		PipelineLayout(const PipelineLayout&) = delete;
		PipelineLayout(PipelineLayout&& rhs) noexcept;
		~PipelineLayout();

		PipelineLayout& operator= (const PipelineLayout&) = delete;
		PipelineLayout& operator= (PipelineLayout&& rhs) noexcept;

		void Finalize(Device& device);

		void Release();
		void Clear();

		void SetSize(const Size& size);
		void SetTopology(const VkPrimitiveTopology& topology);
		void SetBlending(bool blend, uint nAttachments = 1);
		void AddShader(const Shader& shader);
		void AddDynamicState(const VkDynamicState& state);

		VkPipelineVertexInputStateCreateInfo vertexInput{};
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		VkViewport viewport {};
		VkRect2D scissor {};
		VkPipelineViewportStateCreateInfo viewportState {};
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		VkPipelineMultisampleStateCreateInfo multisampling{};
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
		std::vector<VkPipelineShaderStageCreateInfo> shaders;
		VkPipelineColorBlendStateCreateInfo colorBlending{};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		std::vector<VkDynamicState> dynamicStates;
		RenderPass pass;

		VkPipelineLayout layout = VK_NULL_HANDLE;
	};

	struct Pipeline
	{
		Pipeline() = default;
		Pipeline(Device& device, const PipelineLayout& layout, u32 subpass = 0);
		Pipeline(const Pipeline&) = delete;
		Pipeline(Pipeline&& rhs) noexcept;
		~Pipeline();

		Pipeline& operator= (const Pipeline&) = delete;
		Pipeline& operator= (Pipeline&& rhs) noexcept;

		void Release();

		VkPipeline pipeline = VK_NULL_HANDLE;
	};
}