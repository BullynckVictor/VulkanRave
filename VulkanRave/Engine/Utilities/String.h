#pragma once
#include <sstream>

namespace rv
{
	namespace detail
	{
		template<typename F, typename... Args>
		void str(std::ostringstream& ss, const F& first, const Args&... args)
		{
			ss << first;
			if constexpr (sizeof...(Args) != 0)
				str(ss, args...);
		}
	}

	template<typename... Args>
	std::string str(const Args&... args)
	{
		if constexpr (sizeof...(Args) != 0)
		{
			std::ostringstream ss;
			detail::str(ss, args...);
			return ss.str();
		}
		else
			return {};
	}
}