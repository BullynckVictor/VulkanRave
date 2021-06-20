#include "Engine/Engine.h"

void main()
{
	rv::Timer timer;
	rv::WindowManager manager;
	rv::Window& window = manager.Create("Hello World!", 600, 400, true);
	rv::EventListener listener = window.Listen();

	rv::Instance instance;
	#ifdef RV_DEBUG
	rv::DebugMessenger messenger(instance);
	rv::EventListener errorListener = messenger.Listen();
	#endif

	rv::vk::Surface surface = window.Surface(instance);

	rv::Device device(instance, rv::GraphicsRequirements(surface.get()));
	rv::DeviceQueue graphicsQueue = device.Queue({ rv::QueueContainsFlag, VK_QUEUE_GRAPHICS_BIT });
	rv::DeviceQueue presentQueue = device.Queue({ rv::QueueCanPresent, surface.get() });

	rv::uint frames = 0;
	rv::debug.Log(rv::str("init time: ", timer.Mark(), "s"));
	while (window.Open())
	{
		if constexpr (rv::build.debug)
			while (auto e = listener.Get())
			{
				if (auto event = e->opt_cast<rv::VulkanDebugEvent>())
				{
					window.SetTitle(event->message);
					if (event->messageType == rv::RV_MT_ERROR || event->messageType == rv::RV_MT_WARNING)
						rv_throw(rv::str("Debug Callback: ", event->message));
				}
			}
		frames++;
	}
	rv::debug.Log(rv::str((float)frames / timer.Peek(), " fps"));
}