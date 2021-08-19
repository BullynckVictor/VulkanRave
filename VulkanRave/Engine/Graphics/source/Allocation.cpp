#include "Engine/Graphics/Allocation.h"
#include "Engine/Graphics/VulkanPointer.h"

rv::Allocation::Allocation(VmaAllocator allocator)
	:
	allocator(allocator)
{
}

rv::Allocation::Allocation(Allocation&& rhs) noexcept
	:
	allocation(move(rhs.allocation)),
	allocator(move(rhs.allocator))
{
}

rv::Allocation::~Allocation()
{
	Release();
}

rv::Allocation& rv::Allocation::operator=(Allocation&& rhs) noexcept
{
	Release();
	allocation = move(rhs.allocation);
	allocator = move(rhs.allocator);
	return *this;
}

VmaAllocationInfo rv::Allocation::Info() const
{
	VmaAllocationInfo info{};
	vmaGetAllocationInfo(allocator, allocation, &info);
	return info;
}

void rv::Allocation::Release()
{
	if (allocator && allocation)
		vmaFreeMemory(allocator, allocation);
	allocator = nullptr;
	allocation = nullptr;
}
