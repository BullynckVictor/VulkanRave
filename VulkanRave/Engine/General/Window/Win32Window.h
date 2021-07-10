#pragma once
#include "Engine/General/SystemInclude.h"
#include "Engine/General/Event.h"
#include "Engine/General/Window/Keyboard.h"
#include "Engine/General/Window/Mouse.h"
#include "Engine/Graphics/VulkanPointer.h"
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

		bool HandleMessages();

		void Close();
		bool Open();

		const std::string& GetTitle() const;
		void SetTitle(const std::string& title);

		vk::Surface Surface(Instance& instance) const;

		bool Minimized() const;
		void Minimize();

		const Size& GetSize() const;
		const uint& DPI() const;

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
		uint dpi = 0;
		Size size;

		bool minimized = false;

		static u64 nWindows;
	};
}