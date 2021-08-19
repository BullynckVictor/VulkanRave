#pragma once
#include "Engine/Graphics/ResourceAllocator.h"

namespace rv
{
	namespace detail
	{
		void move_buffers(Buffer& a, Buffer& b);
	}

	struct OptimalBuffer : public Buffer
	{
		OptimalBuffer() = default;
		OptimalBuffer(Device& device, ResourceAllocator& allocator, VkBufferUsageFlags usage, u64 size, const void* source = nullptr, bool staged = true);
		OptimalBuffer(const OptimalBuffer&) = delete;
		OptimalBuffer(OptimalBuffer&& rhs) noexcept;

		OptimalBuffer& operator= (const OptimalBuffer&) = delete;
		OptimalBuffer& operator= (OptimalBuffer&& rhs) noexcept;

		void Stage(Device& device, ResourceAllocator& allocator, const void* source, u64 size);
	};
}