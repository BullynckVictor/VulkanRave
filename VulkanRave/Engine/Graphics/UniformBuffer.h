#pragma once
#include "Engine/Graphics/ResourceAllocator.h"

namespace rv
{
	struct UniformBuffer : public Buffer
	{
		UniformBuffer() = default;
		template<typename T>
		UniformBuffer(Device& device, ResourceAllocator& allocator, const T& source) : Buffer(device, allocator.allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, sizeof(T), &source) {}
		UniformBuffer(Device& device, ResourceAllocator& allocator, const u64& size, const void* source = nullptr);
		UniformBuffer(const UniformBuffer&) = delete;
		UniformBuffer(UniformBuffer&& rhs) noexcept;

		UniformBuffer& operator= (const UniformBuffer&) = delete;
		UniformBuffer& operator= (UniformBuffer&& rhs) noexcept;
	};
}