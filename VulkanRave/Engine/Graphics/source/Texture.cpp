#include "Engine/Graphics/Texture.h"
#include "Engine/Graphics/Buffer.h"
#include "Engine/Utilities/Exception.h"

rv::Texture::Texture(Device& device, ResourceAllocator& allocator, ImageBuffer&& _image)
	:
	Image(device, allocator.allocator, _image.GetSize(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_GPU_ONLY),
	buffer(std::move(_image))
{
	Buffer stageBuffer(device, allocator.allocator, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, (u64)buffer.GetLength() * sizeof(Color), buffer.data());
	allocator.TransitionImageLayout(device, *this, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	allocator.CopyBufferToImage(device, stageBuffer, *this, buffer.GetSize());
}

rv::Texture::Texture(Texture&& rhs) noexcept
	:
	Image(std::move(rhs)),
	buffer(std::move(rhs.buffer))
{
}

rv::Texture::~Texture()
{
	Release();
}

void move_img(rv::Image& a, rv::Image& b)
{
	a = std::move(b);
}

rv::Texture& rv::Texture::operator=(Texture&& rhs) noexcept
{
	move_img(*this, rhs);
	buffer = std::move(rhs.buffer);
	return *this;
}

void rv::Texture::Release()
{
	Image::Release();
	buffer.Release();
}
