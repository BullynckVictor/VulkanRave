#include "General/Main.h"
#include "General/SystemInclude.h"
#include "Utilities/Exception.h"
#include "General/Logger.h"
#include <cstdlib>

void error_box(const char* title, const char* text)
{
	rv::debug.Log(rv::RV_MT_ERROR, text);
	if constexpr (rv::sys.windows)
		MessageBoxA(nullptr, text, title, MB_ICONWARNING | MB_OK);
}

int rv::rave_main()
{
	try
	{
		main();
		return EXIT_SUCCESS;
	}
	catch (const std::exception& e)
	{
		error_box("std::exception", e.what());
	}
	catch (...)
	{
		error_box("Unknown exception", "Something went wrong!");
	}
	return EXIT_FAILURE;
}