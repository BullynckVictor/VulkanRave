#pragma once
#include "Graphics/Vulkan.h"

namespace rv
{
	template<typename T>
	void destroy(T pointer) { static_assert(false, "rv::destroy<T>() must be implemented"); }

	template<typename T>
	void release(T& pointer)
	{
		if (pointer)
			destroy(pointer);
		pointer = VK_NULL_HANDLE;
	}

	template<typename T>
	T&& move(T&& pointer)
	{
		T temp = pointer;
		pointer = VK_NULL_HANDLE;
		return temp;
	}

	template<typename T>
	class VkPointer
	{
	public:
		VkPointer(T value = VK_NULL_HANDLE) : value(value) {}
		VkPointer(const VkPointer&) = delete;
		VkPointer(VkPointer&& rhs) noexcept : value(move(rhs.value)) {}
		~VkPointer() { Release(); }

		VkPointer& operator= (const VkPointer&) = delete;
		VkPointer& operator= (VkPointer&& rhs) noexcept { Release(); move(rhs); return *this; }

		T& get() { return value; }
		const T& get() const { return value; }

		void Release() { release(value); }
		void Clear() { value = VK_NULL_HANDLE; }

	private:
		T value;
	};
}