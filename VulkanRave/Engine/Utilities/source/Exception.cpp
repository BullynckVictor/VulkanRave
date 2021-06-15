#include "Utilities/Exception.h"
#include "General/SystemInclude.h"

const char* rv::Exception::what() const
{
	return message.c_str();
}
