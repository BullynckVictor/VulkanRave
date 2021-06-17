#pragma once
#include "General/System.h"
#include <chrono>

#ifdef RV_PLATFORM_WINDOWS
#	include "Graphics/Window/Win32Window.h"
#endif

namespace rv
{
	using namespace std::chrono_literals;

	class WindowManager
	{
	public:
		WindowManager();
		~WindowManager();
		Window& Create(const char* title, int width, int height);

		std::chrono::duration<long long, std::milli> sleep_dur = 16ms;

	private:
		void Task();

		std::thread workerThread;
		std::list<Window> windows;
		std::mutex mutex;

		bool shouldClose = false;
		std::condition_variable cond;

		Window* current = nullptr;
		bool creating = false;

		struct
		{
			const char* title = nullptr;
			int width = 0;
			int height = 0;
		} creationVars;
	};
}