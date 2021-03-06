#pragma once
#include "Engine/Graphics/Device.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/RenderPass.h"
#include "Engine/Graphics/Vertex.h"
#include "Engine/Graphics/DescriptorSet.h"
#include "Engine/Utilities/Vector.h"

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
		void ReleaseExceptSet();
		void Clear();
		void ClearExceptSet();

		void SetSize(const Size& size);
		void SetTopology(const VkPrimitiveTopology& topology);
		void SetBlending(bool blend, uint nAttachments = 1);
		void SetCulling(bool cull);
		void AddShader(const Shader& shader);
		void AddDynamicState(const VkDynamicState& state);
		template<typename V>
		void SetVertexType()
		{
			static constexpr auto bindingDescription = VertexInfo<V>::binding;
			static constexpr auto attributeDescriptions = VertexInfo<V>::attributes;

			vertexInput.vertexBindingDescriptionCount = 1;
			vertexInput.vertexAttributeDescriptionCount = static_cast<u32>(attributeDescriptions.size());
			vertexInput.pVertexBindingDescriptions = &bindingDescription;
			vertexInput.pVertexAttributeDescriptions = attributeDescriptions.data();
		}

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
		DescriptorSetLayout descriptorSet;

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

	enum PipelineType
	{
		RV_PT_GRAPHICS,
		RV_PT_COMPUTE
	};
}