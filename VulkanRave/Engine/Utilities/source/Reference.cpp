#include "Utilities/Reference.h"
#include "Utilities/Exception.h"

void rv::detail::assert_ptr(const void* ptr)
{
	if (!ptr)
		throw std::runtime_error("bruh");
}
