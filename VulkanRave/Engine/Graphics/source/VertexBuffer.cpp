#include "Engine/Graphics/VertexBuffer.h"
#include "Engine/Graphics/VulkanPointer.h"

rv::VertexBuffer::VertexBuffer(Device& device, ResourceAllocator& allocator, u32 size, const void* vertices, bool staged)
	:
	OptimalBuffer(device, allocator, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, size, vertices, staged)
{
}

rv::VertexBuffer::VertexBuffer(VertexBuffer&& rhs) noexcept
	:
	OptimalBuffer(std::move(rhs))
{
}

rv::VertexBuffer& rv::VertexBuffer::operator=(VertexBuffer&& rhs) noexcept
{
	Release(); 
	detail::move_buffers(*this, rhs);
	return *this;
}