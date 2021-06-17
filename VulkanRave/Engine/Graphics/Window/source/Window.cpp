#include "Graphics/Window.h"
#include <condition_variable>

rv::WindowManager::WindowManager()
	:
	workerThread(std::bind(&WindowManager::Task, this))
{
}

rv::WindowManager::~WindowManager()
{
	shouldClose = true;
	workerThread.join();
}

rv::Window& rv::WindowManager::Create(const char* title, int width, int height)
{
	std::mutex m;
	std::unique_lock<std::mutex> lock(m);
	creationVars = { title, width, height };
	creating = true;
	cond.wait(lock);
	auto& ref = *current;
	current = nullptr;
	return ref;
}

void rv::WindowManager::Task()
{
	while (!shouldClose)
	{
		auto begin = std::chrono::steady_clock::now();
		{
			std::lock_guard<std::mutex> guard(mutex);
			for (std::list<Window>::iterator window = windows.begin(); window != windows.end();)
			{
				window->HandleMessages();
				if (!window->Open())
					windows.erase(window++);
				else
					++window;
			}
			if (creating)
			{
				current = &windows.emplace_back(creationVars.title, creationVars.width, creationVars.height);
				creating = false;
				cond.notify_one();
			}
		}
		std::this_thread::sleep_until(begin + sleep_dur);
	}
	for (auto& window : windows)
		window.Close();
}