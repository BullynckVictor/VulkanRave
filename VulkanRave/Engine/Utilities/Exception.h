#pragma once
#include <exception>
#include <stdexcept>

namespace rv
{
	class Exception : public std::exception
	{
	public:
		const char* what() const override;

	private:
		std::string message;
	};
}