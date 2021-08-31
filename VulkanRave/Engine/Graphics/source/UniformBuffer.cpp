#include "Engine/Graphics/UniformBuffer.h"
#include "Engine/Graphics/OptimalBuffer.h"

rv::UniformBuffer::UniformBuffer(Device& device, ResourceAllocator& allocator, const u64& size, const void* source)
	:
	Buffer(device, allocator.allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, size, source)
{
}

rv::UniformBuffer::UniformBuffer(UniformBuffer&& rhs) noexcept
	:
	Buffer(std::move(rhs))
{
}

rv::UniformBuffer& rv::UniformBuffer::operator=(UniformBuffer&& rhs) noexcept
{
	detail::move_buffers(*this, rhs);
	return *this;
}
