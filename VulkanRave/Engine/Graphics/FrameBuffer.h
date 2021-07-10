#pragma once
#include "Engine/Graphics/Device.h"
#include "Engine/Graphics/ImageView.h"
#include "Engine/Graphics/RenderPass.h"
#include "Engine/Utilities/Vector.h"

namespace rv
{
	struct FrameBuffer
	{
		FrameBuffer() = default;
		FrameBuffer(Device& device, RenderPass& pass, const ImageView& view, const Size& size);
		FrameBuffer(const FrameBuffer&) = delete;
		FrameBuffer(FrameBuffer&& rhs) noexcept;
		~FrameBuffer();

		FrameBuffer& operator= (const FrameBuffer&) = delete;
		FrameBuffer& operator= (FrameBuffer&& rhs) noexcept;

		void Release();

		VkFramebuffer frameBuffer = VK_NULL_HANDLE;
	};
}