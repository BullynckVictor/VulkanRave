#include "Engine/Engine.h"

void main()
{
	rv::WindowManager manager;
	rv::Window& window = manager.Create("Hello World!", 600, 400, true);

	rv::Timer timer;

	rv::EventListener listener = window.Listen();

	rv::debug.Log(rv::str(rv::LogMessageEvent::static_event));
	rv::debug.Log(rv::str(rv::WindowResizeEvent::static_event));

	rv::uint frames = 0;

	while (window.Open())
	{
		while (auto e = listener.Get())
		{
			if (auto event = e->opt_cast<rv::KeyEvent>())
				window.SetTitle(rv::str("Key ", int(event->key), " ", event->press == rv::RV_PRESSED ? "pressed" : "released"));
			else if (auto event = e->opt_cast<rv::MouseButtonEvent>())
				if (event->button == rv::RV_MB_LEFT)
					window.SetTitle(rv::str("Left mouse button ", event->press == rv::RV_PRESSED ? "pressed" : "released"));
		}
		frames++;
	}
	rv::debug.Log(rv::str((float)frames / timer.Peek(), " fps"));
}