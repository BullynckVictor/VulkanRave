#pragma once

namespace rv
{
	template<typename T>
	class Flag
	{
	public:
		Flag(const T& value = {}) : value(value) {}

		const T& Peek() const { return value; }
		T Get() { T temp = value; value = {}; return temp; }
		void Reset() { value = {}; }

		T& Expose() { return value; }
		const T& Expose() const { return value; }

	private:
		T value;
	};
}