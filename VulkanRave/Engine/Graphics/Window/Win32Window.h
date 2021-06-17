#pragma once
#include "General/SystemInclude.h"
#include "Utilities/Event.h"
#include <mutex>

#ifndef RV_PLATFORM_WINDOWS
#	error Detected platform is not Windows, please include "Engine/Graphics/Window.h" instead
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
		Window(const char* title, int width, int height);
		~Window();

		void HandleMessages();

		void Close();
		bool Open();

		const std::string& GetTitle() const;
		void SetTitle(const std::string& title);

		bool Minimized() const;

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