#pragma once
#include "Engine/Graphics/Device.h"

namespace rv
{
	struct ImageView
	{
		ImageView() = default;
		ImageView(Device& device, VkImage image, VkFormat format);
		ImageView(const ImageView&) = delete;
		ImageView(ImageView&& rhs) noexcept;
		~ImageView();

		ImageView& operator= (const ImageView&) = delete;
		ImageView& operator= (ImageView&& rhs) noexcept;

		void Release();

		VkImageView view = VK_NULL_HANDLE;
	};
}