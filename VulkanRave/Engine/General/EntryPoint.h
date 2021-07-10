#pragma once
#include "Engine/General/SystemInclude.h"
#include "Engine/General/Main.h"

#ifdef RV_PLATFORM_WINDOWS
int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
#endif
{
	return rv::rave_main();
}