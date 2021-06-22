#pragma once
#include "Graphics/Device.h"

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

	struct Image : public ImageView
	{
		Image() = default;
		Image(Device & device, VkImage image, VkFormat format);
		Image(const Image&) = delete;
		Image(Image&& rhs) noexcept;
		~Image();

		Image& operator= (const Image&) = delete;
		Image& operator= (Image && rhs) noexcept;

		void Release();

		VkFormat format{};
		VkImage image = VK_NULL_HANDLE;
	};
}