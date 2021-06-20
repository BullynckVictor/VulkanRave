#pragma once
#include "Utilities/Types.h"
#include <limits>

namespace rv
{
	template<typename I, typename T = u32>
	class ID
	{
	public:
		static constexpr T invalid_id = std::numeric_limits<T>::max();

		ID(const T& id = invalid_id) : id(id) {}

		operator T& () noexcept { return id; }
		operator const T& () const noexcept { return id; }

		bool valid() const noexcept { return id != invalid_id; }
		bool invalid() const noexcept { return id == invalid_id; }
		T get() const noexcept { return id; }

	private:
		T id;
	};

	template<typename I, typename T = u32>
	static ID<I, T> unique_id() { static T id = 0; return ++id; }
}