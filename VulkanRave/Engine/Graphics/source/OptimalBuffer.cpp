#include "Engine/Graphics/OptimalBuffer.h"

rv::OptimalBuffer::OptimalBuffer(Device& device, ResourceAllocator& allocator, VkBufferUsageFlags usage, u32 size, const void* source, bool staged)
	:
	Buffer(device, allocator.allocator, usage | (staged ? VK_BUFFER_USAGE_TRANSFER_DST_BIT : 0), staged ? VMA_MEMORY_USAGE_GPU_ONLY : VMA_MEMORY_USAGE_CPU_TO_GPU, size, staged ? nullptr : source)
{
	if (staged)
		Stage(device, allocator, source, size);
}

rv::OptimalBuffer::OptimalBuffer(OptimalBuffer&& rhs) noexcept
	:
	Buffer(std::move(rhs))
{
}

rv::OptimalBuffer& rv::OptimalBuffer::operator=(OptimalBuffer&& rhs) noexcept
{
	detail::move_buffers(*this, rhs);
	return *this;
}

void rv::OptimalBuffer::Stage(Device& device, ResourceAllocator& allocator, const void* source, u64 size)
{
	allocator.StageBuffer(device, *this, source, size);
}

void rv::detail::move_buffers(Buffer& a, Buffer& b)
{
	a = std::move(b);
}