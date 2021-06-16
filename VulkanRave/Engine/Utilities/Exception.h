#pragma once
#include <exception>
#include <stdexcept>

namespace rv
{
	class Exception : public std::exception
	{
	public:
		Exception() = default;
		Exception(const char* exceptionName, const char* source, int line, const std::string& message);
		const char* what() const override;

	private:
		std::string message;
	};

	class InfoException : public Exception
	{
	public:
		InfoException() = default;
		InfoException(const std::string& message, const char* source, int line);
	};

	class FailedAssertion : public Exception
	{
	public:
		FailedAssertion() = default;
		FailedAssertion(const char* condition, const char* source, int line);
		FailedAssertion(const char* condition, const std::string& message, const char* source, int line);
	};

	class ElementNotFoundException : public Exception
	{
	public:
		ElementNotFoundException() = default;
		ElementNotFoundException(const char* type, const char* name, const char* source, int line);
		ElementNotFoundException(const char* type, const char* name, const char* codexType, const char* codexName, const char* source, int line);
	};

	void rv_assert_func(bool condition, const char* str_condition, const char* source, int line);
	void rv_assert_func(bool condition, const char* str_condition, const std::string& message, const char* source, int line);

	bool FileExists(const char* filename);

	void rv_assert_file_func(const char* file, const char* source, int line);
}

#define rv_throw(info) throw rv::InfoException(info, __FILE__, __LINE__)
#define rv_assert(cond) rv::rv_assert_func(cond, #cond, __FILE__, __LINE__)
#define rv_assert_file(file) rv::rv_assert_file_func(file, __FILE__, __LINE__)