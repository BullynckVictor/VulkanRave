#include "Graphics/Pipeline.h"
#include "Graphics/VulkanPointer.h"
#include "Utilities/Exception.h"
#include "General/Logger.h"

template<>
void rv::destroy(VkPipelineLayout layout)
{
	vkDestroyPipelineLayout(static_device->device, layout, nullptr);
}
template<>
void rv::destroy(VkPipeline pipeline)
{
	vkDestroyPipeline(static_device->device, pipeline, nullptr);
}

rv::PipelineLayout::PipelineLayout()
{
	vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInput.vertexBindingDescriptionCount = 0;
	vertexInput.pVertexBindingDescriptions = nullptr; // Optional
	vertexInput.vertexAttributeDescriptionCount = 0;
	vertexInput.pVertexAttributeDescriptions = nullptr; // Optional

	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = 0.0f;
	viewport.height = 0.0f;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor.offset = { 0, 0 };
	scissor.extent = { 0, 0 };

	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_FALSE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = (u32)colorBlendAttachments.size();
	colorBlending.pAttachments = colorBlendAttachments.data();
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
}

rv::PipelineLayout::PipelineLayout(PipelineLayout&& rhs) noexcept
	:
	layout(move(rhs.layout)),
	vertexInput(std::move(rhs.vertexInput)),
	inputAssembly(std::move(rhs.inputAssembly)),
	viewport(std::move(rhs.viewport)),
	scissor(std::move(rhs.scissor)),
	viewportState(std::move(rhs.viewportState)),
	rasterizer(std::move(rhs.rasterizer)),
	multisampling(std::move(rhs.multisampling)),
	depthStencil(std::move(rhs.depthStencil)),
	colorBlendAttachments(std::move(rhs.colorBlendAttachments)),
	colorBlending(std::move(rhs.colorBlending)),
	shaders(std::move(rhs.shaders)),
	dynamicState(std::move(rhs.dynamicState)),
	dynamicStates(std::move(rhs.dynamicStates))
{
}

rv::PipelineLayout::~PipelineLayout()
{
	Release();
}

rv::PipelineLayout& rv::PipelineLayout::operator=(PipelineLayout&& rhs) noexcept
{
	Release();
	layout = move(rhs.layout);
	vertexInput = std::move(rhs.vertexInput);
	inputAssembly = std::move(rhs.inputAssembly);
	viewport = std::move(rhs.viewport);
	scissor = std::move(rhs.scissor);
	viewportState = std::move(rhs.viewportState);
	rasterizer = std::move(rhs.rasterizer);
	multisampling = std::move(rhs.multisampling);
	depthStencil = std::move(rhs.depthStencil);
	colorBlendAttachments = std::move(rhs.colorBlendAttachments);
	colorBlending = std::move(rhs.colorBlending);
	shaders = std::move(rhs.shaders);
	dynamicState = std::move(rhs.dynamicState);
	dynamicStates = std::move(rhs.dynamicStates);
	rhs.Clear();

	return *this;
}

void rv::PipelineLayout::Finalize(Device& device)
{
	Release();

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	rv_check_vkr(vkCreatePipelineLayout(device.device, &pipelineLayoutInfo, nullptr, &layout));
}

void rv::PipelineLayout::Release()
{
	release(layout);
}

void rv::PipelineLayout::Clear()
{
	vertexInput = {};
	inputAssembly = {};
	viewport = {};
	scissor = {};
	viewportState = {};
	rasterizer = {};
	multisampling = {};
	depthStencil = {};
	colorBlending = {};
	dynamicState = {};
	colorBlendAttachments.clear();
	shaders.clear();
	dynamicStates.clear();
}

void rv::PipelineLayout::SetSize(const Size& size)
{
	viewport.width = (float)size.width;
	viewport.height = (float)size.height;
	scissor.extent = { size.width, size.height };
}

void rv::PipelineLayout::SetTopology(const VkPrimitiveTopology& topology)
{
	inputAssembly.topology = topology;
}

void rv::PipelineLayout::SetBlending(bool blend, uint nAttachments)
{
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	if (blend)
	{
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	}
	else
	{
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
	}
	colorBlendAttachments.insert(colorBlendAttachments.end(), nAttachments, colorBlendAttachment);
	colorBlending.attachmentCount = (u32)colorBlendAttachments.size();
	colorBlending.pAttachments = colorBlendAttachments.data();
}

void rv::PipelineLayout::AddShader(const Shader& shader)
{
	shaders.push_back(shader.PipelineStage());
}

void rv::PipelineLayout::AddDynamicState(const VkDynamicState& state)
{
	dynamicStates.push_back(state);
	dynamicState.dynamicStateCount = (u32)dynamicStates.size();
	dynamicState.pDynamicStates = dynamicStates.data();
}

rv::Pipeline::Pipeline(Device& device, const PipelineLayout& layout, u32 subpass)
{
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = (u32)layout.shaders.size();
	pipelineInfo.pStages = layout.shaders.data();
	pipelineInfo.pVertexInputState = &layout.vertexInput;
	pipelineInfo.pInputAssemblyState = &layout.inputAssembly;
	pipelineInfo.pViewportState = &layout.viewportState;
	pipelineInfo.pRasterizationState = &layout.rasterizer;
	pipelineInfo.pMultisampleState = &layout.multisampling;
	pipelineInfo.pDepthStencilState = &layout.depthStencil;
	pipelineInfo.pColorBlendState = &layout.colorBlending;
	pipelineInfo.pDynamicState = &layout.dynamicState;
	pipelineInfo.layout = layout.layout;
	pipelineInfo.renderPass = layout.pass.pass;
	pipelineInfo.subpass = subpass;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional
	rv_check_vkr(vkCreateGraphicsPipelines(device.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));
	debug.Log("Created Pipeline");
}

rv::Pipeline::Pipeline(Pipeline&& rhs) noexcept
	:
	pipeline(move(rhs.pipeline))
{
}

rv::Pipeline::~Pipeline()
{
	Release();
}

rv::Pipeline& rv::Pipeline::operator=(Pipeline&& rhs) noexcept
{
	Release();
	pipeline = move(rhs.pipeline);
	return *this;
}

void rv::Pipeline::Release()
{
	release(pipeline);
}
