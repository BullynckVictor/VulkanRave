#include "Utilities/Exception.h"
#include "Utilities/String.h"
#include "General/System.h"
#include <filesystem>

rv::Exception::Exception(const char* exceptionName, const char* source, int line, const std::string& message)
	:
	message(str(
		exceptionName, " occurred!\n\n"
		"File: ", source, "\n"
		"Line: ", line, "\n\n",
		message
	))
{
}

const char* rv::Exception::what() const
{
	return message.c_str();
}

rv::InfoException::InfoException(const std::string& message, const char* source, int line)
	:
	Exception("rv::InfoException", source, line, message)
{
}

rv::FailedAssertion::FailedAssertion(const char* condition, const char* source, int line)
	:
	Exception("rv::Assertion", source, line, str("Assertion \"", condition, "\" failed"))
{
}

rv::FailedAssertion::FailedAssertion(const char* condition, const std::string& message, const char* source, int line)
	:
	Exception("rv::Assertion", source, line, str("Assertion \"", condition, "\" failed\n", message))
{
}

rv::ElementNotFoundException::ElementNotFoundException(const char* type, const char* name, const char* source, int line)
	:
	Exception("rv::ElementNotFoundException", source, line, str(type, " \"", name, "\" not found"))
{
}

rv::ElementNotFoundException::ElementNotFoundException(const char* type, const char* name, const char* codexType, const char* codexName, const char* source, int line)
	:
	Exception("rv::ElementNotFoundException", source, line, str(type, " \"", name, "\" not found in ", codexType, " \"", codexName, "\""))
{
}

void rv::rv_assert_func(bool condition, const char* str_condition, const char* source, int line)
{
	if constexpr (sys.debug)
		if (!condition)
			throw FailedAssertion(str_condition, source, line);
}

void rv::rv_assert_func(bool condition, const char* str_condition, const std::string& message, const char* source, int line)
{
	if constexpr (sys.debug)
		if (!condition)
			throw FailedAssertion(str_condition, message, source, line);
}

bool rv::FileExists(const char* filename)
{
	return std::filesystem::exists(filename);
}

void rv::rv_assert_file_func(const char* file, const char* source, int line)
{
	if (!FileExists(file))
		throw ElementNotFoundException("File", file, source, line);
}

#ifdef RV_PLATFORM_WINDOWS
#include <comdef.h>
rv::HrException::HrException(const HRESULT hr, const char* source, int line)
	:
	Exception("rv::HrException", source, line, str("HRESULT: 0x", std::hex, hr, " [", HrToString(hr), "]\n", _com_error(hr).ErrorMessage()))
{
}

void rv::rv_check_hr_func(HRESULT hr, const char* source, int line)
{
	if (FAILED(hr))
		throw HrException(hr, source, line);
}

#endif