#include "Engine/Engine.h"

void main()
{
	rv::WindowManager manager;
	rv::Window& window = manager.Create("Hello World!", 600, 400, true);

	rv::Timer timer;

	rv::EventListener listener = window.Listen();

	rv::debug.Log(rv::str(rv::LogMessageEvent::static_event));
	rv::debug.Log(rv::str(rv::WindowResizeEvent::static_event));

	while (window.Open())
	{
		while (auto e = listener.Get())
		{
			if (e->equals<rv::KeyPressedEvent>())
				window.SetTitle(rv::str("Key ", int(e->cast<rv::KeyPressedEvent>().key), " pressed"));
			else if (e->equals<rv::KeyReleasedEvent>())
				window.SetTitle(rv::str("Key ", int(e->cast<rv::KeyReleasedEvent>().key), " released"));
		}
		rv::debug.Log(rv::str(int( 1.0f / timer.Mark() )));
	}
}