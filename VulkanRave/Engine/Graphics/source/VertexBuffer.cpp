#include "Graphics/VertexBuffer.h"
#include "Graphics/VulkanPointer.h"

rv::VertexBuffer::VertexBuffer(Device& device, u32 size, const void* vertices)
	:
	Buffer(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, size, vertices)
{
}

rv::VertexBuffer::VertexBuffer(VertexBuffer&& rhs) noexcept
	:
	Buffer(std::move(rhs))
{
}

rv::VertexBuffer& rv::VertexBuffer::operator=(VertexBuffer&& rhs) noexcept
{
	Release(); 
	buffer = move(buffer); 
	memory = move(rhs.memory);
	return *this;
}

void rv::detail::move_buffers(Buffer& a, Buffer& b)
{
	a = std::move(b);
}
