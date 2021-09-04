#pragma once
#include "Engine/Graphics/Image.h"
#include "Engine/Graphics/ImageBuffer.h"
#include "Engine/Graphics/ResourceAllocator.h"

namespace rv
{
	struct Texture : public Image
	{
		Texture() = default;
		Texture(Device& device, ResourceAllocator& allocator, ImageBuffer&& image);
		Texture(const Texture&) = delete;
		Texture(Texture&& rhs) noexcept;
		~Texture();

		Texture& operator= (const Texture&) = delete;
		Texture& operator= (Texture&& rhs) noexcept;

		void Release();

		ImageBuffer buffer;
	};
}