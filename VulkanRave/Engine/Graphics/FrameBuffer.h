#pragma once
#include "Graphics/Device.h"
#include "Graphics/ImageView.h"
#include "Graphics/RenderPass.h"
#include "Utilities/Vector.h"

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