#include "Engine/Engine.h"

void main()
{
	rv::WindowManager manager;
	rv::Window& window = manager.Create("Hello World!", 600, 400);

	int i = 0;
	while (window.Open()) window.SetTitle(rv::str(++i));
}