#pragma once
#include <array>

namespace rv
{
	template<size_t S>
	class Typeless
	{
	public:
		Typeless()
		{}
		template<typename T>
		Typeless(const T& value)
		{
			static_assert(sizeof(T) <= S);
			memcpy(data(), &value, sizeof(T));
		}
		template<typename T>
		Typeless(T&& value)
		{
			static_assert(sizeof(T) <= S);
			memcpy(data(), &value, sizeof(T));
		}
		template<typename T>
		T& as()
		{
			static_assert(sizeof(T) <= S);
			return *(reinterpret_cast<T*>(data()));
		}
		template<typename T>
		const T& as() const
		{
			static_assert(sizeof(T) <= S);
			return *(reinterpret_cast<const T*>(data()));
		}
		void* data()
		{
			return m_data.data();
		}
		const void* data() const
		{
			return m_data.data();
		}

		bool operator== (const Typeless& rhs) const
		{
			return m_data == rhs.m_data;
		}
		bool operator!= (const Typeless& rhs) const
		{
			return m_data != rhs.m_data;
		}

	private:
		std::array<unsigned char, S> m_data;
	};

	typedef Typeless<1>				Raw8;
	typedef Typeless<2>				Raw16;
	typedef Typeless<4>				Raw32;
	typedef Typeless<8>				Raw64;
	typedef Typeless<sizeof(void*)>	RawT;
}