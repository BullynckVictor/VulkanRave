#pragma once
#include "Engine/Graphics/Buffer.h"

namespace rv
{
	namespace detail
	{
		void move_buffers(Buffer& a, Buffer& b);
	}

	struct VertexBuffer : public Buffer
	{
		VertexBuffer() = default;
		VertexBuffer(Device& device, u32 size, const void* vertices = nullptr);
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&& rhs) noexcept;

		VertexBuffer& operator= (const VertexBuffer&) = delete;
		VertexBuffer& operator= (VertexBuffer&& rhs) noexcept;
	};

	template<typename V>
	struct VertexBufferT : public VertexBuffer
	{
		VertexBufferT() = default;
		VertexBufferT(Device& device, const std::vector<V>& vertices) : VertexBuffer(device, (u32)vertices.size() * sizeof(V), vertices.data()), vertices(vertices) {}
		VertexBufferT(Device& device, std::vector<V>&& vertices) : VertexBuffer(device, (u32)vertices.size() * sizeof(V), vertices.data()), vertices(std::move(vertices)) {}
		VertexBufferT(const VertexBufferT&) = delete;
		VertexBufferT(VertexBufferT&& rhs) noexcept : VertexBuffer(rhs), vertices(std::move(rhs.vertices)) {}
		~VertexBufferT() { Release(); }

		VertexBufferT& operator= (const VertexBufferT&) = delete;
		VertexBufferT& operator= (VertexBufferT&& rhs) noexcept { Release(); detail::move_buffers(*this, rhs); vertices = std::move(rhs.vertices); return *this; };

		void Release() { VertexBuffer::Release(); vertices.clear(); }

		void Write(Device& device) { Copy(device, vertices.data(), byte_size()); }

		u32 size() const { return (u32)vertices.size(); }
		u32 byte_size() const { return size() * sizeof(V); }

		std::vector<V> vertices;
	};
}