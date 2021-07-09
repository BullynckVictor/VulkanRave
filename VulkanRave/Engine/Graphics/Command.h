#pragma once
#include "Graphics/Device.h"
#include "Graphics/RenderPass.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Pipeline.h"
#include "Graphics/Sync.h"
#include "Graphics/VertexBuffer.h"
#include "Utilities/Color.h"

namespace rv
{
	struct CommandPool
	{
		CommandPool() = default;
		CommandPool(Device& device, const DeviceQueue& graphicsQueue);
		CommandPool(const CommandPool&) = delete;
		CommandPool(CommandPool&& rhs) noexcept;
		~CommandPool();

		CommandPool& operator= (const CommandPool&) = delete;
		CommandPool& operator= (CommandPool&& rhs) noexcept;

		void Release();

		VkCommandPool pool = VK_NULL_HANDLE;
	};

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
		void Draw(u32 nVertices, u32 nInstances = 1, u32 vertexOffset = 0, u32 instanceOffset = 0);
		void EndPass();
		void End();

		void Submit(
			DeviceQueue& queue,
			const VkSemaphore* wait,
			const VkPipelineStageFlags* stages,
			u32 nWait,
			const VkSemaphore* signals,
			u32 nSignals,
			Fence* fence = nullptr
		);

		VkCommandBuffer buffer = VK_NULL_HANDLE;
	};
}