#pragma once
#include <limits>
#include <type_traits>
#include "Utilities/Types.h"

namespace rv
{
	namespace detail
	{
		void assert_oindex(bool valid);
		void check_oindex(bool valid);
	}

	template<typename T>
	struct OptionalIndex
	{
		static_assert(std::is_integral_v<T>, "OptionalIndex can only be used with integral types");
		static constexpr T invalid_index = std::is_signed_v<T> ? std::numeric_limits<T>::min() : std::numeric_limits<T>::max();

		constexpr OptionalIndex(const T& index = invalid_index) : index(index) {}
		constexpr bool valid() const { return index != invalid_index; }
		constexpr bool invalid() const { return index == invalid_index; }
		constexpr operator T& () { return index; }
		constexpr operator const T& () const { return index; }
		void assert() const { detail::assert_oindex(valid()); }
		void check() const { detail::check_oindex(valid()); }
		T& get() { check(); return index; }
		const T& get() const { check(); return index; }

		T index;
	};

	typedef OptionalIndex<uint32> OIndex;
}