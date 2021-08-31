#pragma once
#include "Engine/Graphics/CommandPool.h"
#include "Engine/Graphics/RenderPass.h"
#include "Engine/Graphics/FrameBuffer.h"
#include "Engine/Graphics/Pipeline.h"
#include "Engine/Graphics/Sync.h"
#include "Engine/Graphics/VertexBuffer.h"
#include "Engine/Graphics/IndexBuffer.h"
#include "Engine/Graphics/DescriptorSet.h"
#include "Engine/Utilities/Color.h"

namespace rv
{
	struct CommandBuffer
	{
		CommandBuffer() = default;
		CommandBuffer(Device& device, CommandPool& pool);
		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer(CommandBuffer&& rhs) noexcept;
		~CommandBuffer();

		CommandBuffer& operator= (const CommandBuffer&) = delete;
		CommandBuffer& operator= (CommandBuffer&& rhs) noexcept;

		void Release();
		void Free(Device& device, CommandPool& pool);

		void Begin();
		void BeginPass(RenderPass& pass, FrameBuffer& frameBuffer, const Size& size, std::optional<FColor> color);
		void BindPipeline(Pipeline& pipeline, PipelineType type = RV_PT_GRAPHICS);
		void BindVertexBuffer(VertexBuffer& vertices, u64 offset = 0);
		void BindIndexBuffer(IndexBuffer& indices, u64 offset = 0);
		void BindDescriptorSet(PipelineLayout& layout, DescriptorSet& set, PipelineType type = RV_PT_GRAPHICS);
		void Draw(u32 nVertices, u32 nInstances = 1, u32 vertexOffset = 0, u32 instanceOffset = 0);
		void DrawIndexed(u32 nIndices, u32 nInstances = 1, u32 indexOffset = 0, u32 vertexOffset = 0, u32 instanceOffset = 0);
		void EndPass();
		void End();

		void CopyBuffers(Buffer& source, Buffer& dest, u64 size);

		void Submit(
			DeviceQueue& queue,
			const VkSemaphore* wait,
			const VkPipelineStageFlags* stages,
			u32 nWait,
			const VkSemaphore* signals,
			u32 nSignals,
			Fence* fence = nullptr
		);
		void Submit(DeviceQueue& queue);

		VkCommandBuffer buffer = VK_NULL_HANDLE;
	};
}