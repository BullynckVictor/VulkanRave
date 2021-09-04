#pragma once
#include "Engine/Graphics/ImageView.h"
#include "Engine/Graphics/ImageBuffer.h"
#include "Engine/Graphics/MemoryAllocator.h"
#include "Engine/Graphics/Allocation.h"

namespace rv
{
	struct Image
	{
		Image() = default;
		Image(
			Device& device,
			MemoryAllocator& allocator,
			const Size& size,
			VkFormat format,
			VkImageUsageFlags usage,
			VmaMemoryUsage memoryUsage,
			VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL
		);
		Image(const Image&) = delete;
		Image(Image&& rhs) noexcept;
		~Image();

		Image& operator= (const Image&) = delete;
		Image& operator= (Image&& rhs) noexcept;

		void Release();

		VkImage image = VK_NULL_HANDLE;
		Allocation memory{};
		VkFormat format{};
		ImageView view{};
	};
}