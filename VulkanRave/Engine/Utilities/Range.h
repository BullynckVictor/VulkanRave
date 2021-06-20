#pragma once

namespace rv
{
	template<typename T>
	class Range
	{
	public:
		class Iterator
		{
		public:
			Iterator() = default;
			Iterator(const T& value) : value(value) {}

			T operator* () const { return value; }
			T operator++ () { return ++value; }
			T operator++ (int) { return value++; }
			T operator-- () { return --value; }
			T operator-- (int) { return value--; }
			
			bool operator== (const Iterator& rhs) const { return value == rhs.value; }
			bool operator!= (const Iterator& rhs) const { return value != rhs.value; }
			bool operator<= (const Iterator& rhs) const { return value <= rhs.value; }
			bool operator>= (const Iterator& rhs) const { return value >= rhs.value; }
			bool operator<  (const Iterator& rhs) const { return value <  rhs.value; }
			bool operator>  (const Iterator& rhs) const { return value >  rhs.value; }

			Iterator& operator+= (const T& dif) { value += dif; return *this; }
			Iterator& operator-= (const T& dif) { value -= dif; return *this; }
			Iterator  operator+  (const T& dif) const { return value + dif; }
			Iterator  operator-  (const T& dif) const { return value - dif; }

			T operator[] (const T& offset) { return value + offset; }

		private:
			T value;
		};

		Range() = default;
		Range(const T& b, const T& e) : m_begin(b), m_end(e) {}
		Iterator begin() const { return m_begin; }
		Iterator end() const { return m_end; }
		
	private:
		T m_begin;
		T m_end;
	};

	template<typename T>
	Range<T> range(const T& begin, const T& end)
	{
		return Range<T>(begin, end);
	}
	template<typename C>
	Range<typename C::size_type> range(const C& container)
	{
		return Range<typename C::size_type>(0, container.size());
	}
}