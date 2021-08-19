#pragma once
#include "Engine/Graphics/MemoryAllocator.h"

namespace rv
{
	struct Allocation
	{
		Allocation() = default;
		Allocation(VmaAllocator allocator);
		Allocation(const Allocation&) = delete;
		Allocation(Allocation&& rhs) noexcept;
		~Allocation();

		Allocation& operator= (const Allocation&) = delete;
		Allocation& operator= (Allocation&& rhs) noexcept;

		VmaAllocationInfo Info() const;

		void Release();

		VmaAllocation allocation = nullptr;
		VmaAllocator allocator = nullptr;
	};
}