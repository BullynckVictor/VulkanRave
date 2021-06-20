#pragma once
#include "General/SystemInclude.h"
#include "General/Event.h"
#include "General/Window/Keyboard.h"
#include "General/Window/Mouse.h"
#include "Graphics/VulkanPointer.h"
#include <mutex>

#ifndef RV_PLATFORM_WINDOWS
#	error Detected platform is not Windows, please include "Engine/General/Window.h" instead
#endif

namespace rv
{
	struct WindowClass
	{
		WindowClass() = default;
		WindowClass(const WNDCLASSEX& windowClass);
		~WindowClass();

		LPTSTR GetIdentifier() const;

		ATOM atom;
		WNDCLASSEX windowClass;
	};

	class Window : public EventQueueInterface
	{
	public:
		Window() = default;
		Window(const char* title, int width, int height, bool resize);
		~Window();

		void HandleMessages();

		void Close();
		bool Open();

		const std::string& GetTitle() const;
		void SetTitle(const std::string& title);

		vk::Surface Surface(Instance& instance) const;

		bool Minimized() const;
		void Minimize();

		Keyboard keyboard;
		Mouse mouse;

	private:
		static const WindowClass CreateClass();
		static LRESULT CALLBACK StaticWindowProcSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		static const WindowClass wndClass;

		HWND hwnd = NULL;
		std::string title;

		bool minimized = false;

		std::mutex mutex;
	};
}