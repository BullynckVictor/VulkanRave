#pragma once
#include <map>

namespace rv
{
	namespace detail
	{
		struct cmp_str
		{
			bool operator()(const char* a, const char* b) const
			{
				return std::strcmp(a, b) < 0;
			}
		};
	}

	template<typename T>
	using strmap = std::map<const char*, T, detail::cmp_str>;
}