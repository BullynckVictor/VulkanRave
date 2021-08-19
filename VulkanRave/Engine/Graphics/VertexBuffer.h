#pragma once
#include "Engine/Graphics/OptimalBuffer.h"

namespace rv
{
	namespace detail
	{
		void move_buffers(Buffer& a, Buffer& b);
	}

	struct VertexBuffer : public OptimalBuffer
	{
		VertexBuffer() = default;
		VertexBuffer(Device& device, ResourceAllocator& allocator, u32 size, const void* vertices = nullptr, bool staged = true);
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&& rhs) noexcept;

		VertexBuffer& operator= (const VertexBuffer&) = delete;
		VertexBuffer& operator= (VertexBuffer&& rhs) noexcept;
	};

	template<typename V>
	struct VertexBufferT : public VertexBuffer
	{
		VertexBufferT() = default;
		VertexBufferT(Device& device, ResourceAllocator& allocator, const std::vector<V>& vertices, bool staged = true) : VertexBuffer(device, allocator,(u32)vertices.size() * sizeof(V), vertices.data(), staged), vertices(vertices) {}
		VertexBufferT(Device& device, ResourceAllocator& allocator, std::vector<V>&& vertices, bool staged = true) : VertexBuffer(device, allocator, (u32)vertices.size() * sizeof(V), vertices.data(), staged), vertices(std::move(vertices)) {}
		VertexBufferT(const VertexBufferT&) = delete;
		VertexBufferT(VertexBufferT&& rhs) noexcept : VertexBuffer(rhs), vertices(std::move(rhs.vertices)) {}
		~VertexBufferT() { Release(); }

		VertexBufferT& operator= (const VertexBufferT&) = delete;
		VertexBufferT& operator= (VertexBufferT&& rhs) noexcept { Release(); detail::move_buffers(*this, rhs); vertices = std::move(rhs.vertices); return *this; };

		V& operator[] (size_t index) { return vertices[index]; }
		const V& operator[] (size_t index) const { return vertices[index]; }

		void Release() { VertexBuffer::Release(); vertices.clear(); }

		void Write(Device& device) { Copy(device, vertices.data(), byte_size()); }
		void Stage(Device& device, ResourceAllocator& allocator) { VertexBuffer::Stage(device, allocator, vertices.data(), byte_size()); }

		u32 size() const { return (u32)vertices.size(); }
		u32 byte_size() const { return size() * sizeof(V); }

		std::vector<V> vertices;
	};
}