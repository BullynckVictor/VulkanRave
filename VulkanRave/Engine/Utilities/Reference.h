#pragma once
#include <functional>

namespace rv
{
	namespace detail
	{
		void assert_ptr(const void* ptr);
	}

	template<typename T>
	struct OptionalReference
	{
	public:
		OptionalReference() = default;
		OptionalReference(T& object) : ref(&object) {}
		OptionalReference(OptionalReference&& rhs) noexcept : ref(rhs.ref) { rhs.ref = nullptr; }
		OptionalReference& operator= (OptionalReference&& rhs) noexcept { ref = rhs.ref; rhs.ref = nullptr; return *this; }

		void invalidate() { ref = nullptr; }

		bool valid() const { return ref; }
		bool null() const { return !ref; }

		void assert()	{ detail::assert_ptr(ref); }

		operator T& ()	{ assert(); return *ref; }
		T& get()		{ assert(); return *ref; }

		operator const T& () const	{ assert(); return *ref; }
		const T& get() const		{ assert(); return *ref; }

	private:
		T* ref = nullptr;
	};

	template<typename T>
	using ORef = OptionalReference<T>;
}