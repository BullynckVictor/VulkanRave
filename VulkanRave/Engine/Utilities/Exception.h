#pragma once
#include <exception>
#include <stdexcept>
#include "General/SystemInclude.h"

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

#	ifdef RV_PLATFORM_WINDOWS

	class HrException : public Exception
	{
	public:
		HrException() = default;
		HrException(const HRESULT hr, const char* source, int line);
	};

	const char* HrToString(const HRESULT hr);
	void rv_check_hr_func(HRESULT hr, const char* source, int line);
	template<typename T>
	T rv_not_null_win32_func(const T& val, const char* source, int line) { if (!val) throw HrException((HRESULT)GetLastError(), source, line); return val; }

#	endif

	void rv_assert_func(bool condition, const char* str_condition, const char* source, int line);
	void rv_assert_func(bool condition, const char* str_condition, const std::string& message, const char* source, int line);

	template<typename T>
	T rv_not_null_func(const T& ret, const char* source, int line) { if (!ret) throw InfoException("Value was null", source, line); return ret; }
	template<typename T>
	T rv_assert_not_null_func(const T& ret, const char* cond, const char* source, int line) { rv_assert_func(ret, cond, source, line); return ret; }

	bool FileExists(const char* filename);

	void rv_assert_file_func(const char* file, const char* source, int line);
}

#define rv_throw(info) throw rv::InfoException(info, __FILE__, __LINE__)
#define rv_assert(cond) rv::rv_assert_func(cond, #cond, __FILE__, __LINE__)
#define rv_assert_file(file) rv::rv_assert_file_func(file, __FILE__, __LINE__)
#define rv_not_null(ptr) rv::rv_not_null_func(ptr, __FILE__, __LINE__)
#define rv_assert_not_null(ptr) rv::rv_assert_not_null_func(ptr, #ptr " != NULL", __FILE__, __LINE__)

#ifdef RV_PLATFORM_WINDOWS
#define rv_check_hr(hr) rv::rv_check_hr_func(hr, __FILE__, __LINE__)
#define rv_not_null_win32(ptr) rv::rv_not_null_win32_func(ptr, __FILE__, __LINE__)
#endif