#include "Engine/General/Main.h"
#include "Engine/General/SystemInclude.h"
#include "Engine/Utilities/Exception.h"
#include "Engine/General/Logger.h"
#include <cstdlib>

void error_box(const char* title, const char* text)
{
	rv::debug.Log(rv::RV_MT_ERROR, text);
	if constexpr (rv::sys.windows)
		MessageBox(nullptr, text, title, MB_ICONWARNING | MB_OK);
}

int rv::rave_main()
{
	try
	{
		main();
		if constexpr (sys.windows)
			PostQuitMessage(EXIT_SUCCESS);
		return EXIT_SUCCESS;
	}
	catch (const Exception& e)
	{
		error_box("rv::Exception", e.what());
	}
	catch (const std::exception& e)
	{
		error_box("std::exception", e.what());
	}
	catch (...)
	{
		error_box("Unknown exception", "Something went wrong!");
	}
	if constexpr (sys.windows)
		PostQuitMessage(EXIT_FAILURE);
	return EXIT_FAILURE;
}