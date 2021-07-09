#include "Utilities/Exception.h"
#include "Utilities/OptionalIndex.h"

void rv::detail::assert_oindex(bool index_valid)
{
	rv_assert_info(index_valid, "Optional index was invalid");
}

void rv::detail::check_oindex(bool valid)
{
	if (!valid)
		rv_throw("Optional index was invalid");
}
