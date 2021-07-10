#pragma once

namespace rv
{
	struct BakedResource
	{
		BakedResource() = default;
		BakedResource(size_t size, const char* data) : size(size), data(reinterpret_cast<const void*>(data)) {}

		template<typename T>
		const T& as() const { return *reinterpret_cast<const T*>(data); }

		size_t size = 0;
		const void* data = nullptr;
	};
}