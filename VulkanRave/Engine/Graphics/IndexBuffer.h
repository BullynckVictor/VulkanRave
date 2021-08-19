#pragma once
#include "Engine/Graphics/OptimalBuffer.h"

namespace rv
{
	struct IndexBuffer : public OptimalBuffer
	{
		IndexBuffer() = default;
		IndexBuffer(Device & device, ResourceAllocator& allocator, VkIndexType type, u64 size, const void* indices = nullptr, bool staged = true) : OptimalBuffer(device, allocator, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, size, indices, staged), type(type) {}
		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer(IndexBuffer&& rhs) noexcept : OptimalBuffer(std::move(rhs)), type(rhs.type) { rhs.type = {}; }

		IndexBuffer& operator= (const IndexBuffer&) = delete;
		IndexBuffer& operator= (IndexBuffer&& rhs) noexcept { detail::move_buffers(*this, rhs); type = rhs.type; rhs.type = {}; }

		void Release() { OptimalBuffer::Release(); type = {}; }

		VkIndexType type = {};
	};

	template<typename I>
	struct IndexBufferT : public IndexBuffer
	{
		IndexBufferT() = default;
		IndexBufferT(Device & device, ResourceAllocator& allocator, const std::vector<I>& indices, bool staged = true) : IndexBuffer(device, allocator, GetType(), (u32)indices.size() * sizeof(I), indices.data(), staged), indices(indices) {}
		IndexBufferT(Device & device, ResourceAllocator& allocator, std::vector<I>&& indices, bool staged = true) : IndexBuffer(device, allocator, GetType(), (u32)indices.size() * sizeof(I), indices.data(), staged), indices(std::move(indices)) {}
		IndexBufferT(const IndexBufferT&) = delete;
		IndexBufferT(IndexBufferT&& rhs) noexcept : IndexBuffer(rhs), indices(std::move(rhs.indices)) {}
		~IndexBufferT() { Release(); }

		IndexBufferT& operator= (const IndexBufferT&) = delete;
		IndexBufferT& operator= (IndexBufferT&& rhs) noexcept { Release(); detail::move_buffers(*this, rhs); indices = std::move(rhs.indices); return *this; };

		I& operator[] (size_t index) { return indices[index]; }
		const I& operator[] (size_t index) const { return indices[index]; }

		void Release() { IndexBuffer::Release(); indices.clear(); }

		void Write(Device & device) { Copy(device, indices.data(), byte_size()); }
		void Stage(Device & device, ResourceAllocator & allocator) { IndexBuffer::Stage(device, allocator, indices.data(), byte_size()); }

		u64 size() const { return (u32)indices.size(); }
		u64 byte_size() const { return size() * sizeof(I); }

		static constexpr VkIndexType GetType() 
		{ 
			if constexpr (std::is_same_v<I, u32>) return VK_INDEX_TYPE_UINT32;
			else if constexpr (std::is_same_v<I, u16>) return VK_INDEX_TYPE_UINT16;
			else static_assert(false, "Only unsigned 16 or 32 bit integers allowed as IndexBufferT<> argument");
		}

		std::vector<I> indices;
	};

	typedef IndexBufferT<u32> IndexBuffer32;
	typedef IndexBufferT<u16> IndexBuffer16;
}