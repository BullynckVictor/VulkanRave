#include "Engine/General/System.h"

#ifdef RV_PLATFORM_WINDOWS

#include "Engine/General/Window/Win32Window.h"
#include "Engine/General/Logger.h"
#include "Engine/General/Window/WindowEvents.h"
#include "Engine/Utilities/Exception.h"
#include "Engine/Utilities/String.h"
#include <hidusage.h>

const rv::WindowClass rv::Window::wndClass = rv::Window::CreateClass();
rv::u64 rv::Window::nWindows = 0;

rv::WindowClass::WindowClass(const WNDCLASSEX& windowClass)
	:
	windowClass(windowClass),
	atom(rv_not_null_win32(RegisterClassEx(&windowClass)))
{
}

rv::WindowClass::~WindowClass()
{
	UnregisterClass(GetIdentifier(), windowClass.hInstance);
}

LPTSTR rv::WindowClass::GetIdentifier() const
{
	return reinterpret_cast<LPTSTR>(atom);
}

typedef HRESULT(WINAPI* PGetDpiForMonitor)(HMONITOR hmonitor, int dpiType, UINT* dpiX, UINT* dpiY);
WORD GetWindowDPI(HWND hWnd);

rv::Window::Window(const char* title, int width, int height, bool resize)
	:
	title(title),
	size((uint)width * dpi / 96, (uint)height * dpi / 96),
	dpi(GetWindowDPI(NULL))
{
	DWORD style = (resize ? WS_OVERLAPPEDWINDOW : WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU);

	RECT wr;
	wr.left = 100;
	wr.right = size.x + wr.left;
	wr.top = 100;
	wr.bottom = size.y + wr.top;
	rv_assert(AdjustWindowRect(&wr, style, FALSE));

	hwnd = rv_not_null_win32(CreateWindowEx(
		0,                              // Optional window styles.
		wndClass.GetIdentifier(),       // Window class
		title,                          // Window text
		style,                          // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, 
		wr.right - wr.left, wr.bottom - wr.top,

		NULL,                            // Parent window    
		NULL,                            // Menu
		wndClass.windowClass.hInstance,  // Instance handle
		this		                     // Additional application data
	));

	ShowWindow(hwnd, SW_NORMAL);

	//raw input
	RAWINPUTDEVICE mouseInput;
	mouseInput.usUsagePage = HID_USAGE_PAGE_GENERIC;
	mouseInput.usUsage = HID_USAGE_GENERIC_MOUSE;
	mouseInput.dwFlags = 0;
	mouseInput.hwndTarget = nullptr;
	rv_not_null_win32(RegisterRawInputDevices(&mouseInput, 1, sizeof(RAWINPUTDEVICE)));

	dpi = GetWindowDPI(hwnd);

	nWindows++;
}

rv::Window::~Window()
{
	Close();
}

bool rv::Window::HandleMessages()
{
	MSG msg = {};
	while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return Open();
}

void rv::Window::Close()
{
	if (Open())
		rv_not_null(DestroyWindow(hwnd));
}

bool rv::Window::Open()
{
	return hwnd;
}

const std::string& rv::Window::GetTitle() const
{
	return title;
}

void rv::Window::SetTitle(const std::string& newtitle)
{
	title = newtitle;
	rv_not_null_win32(SetWindowText(hwnd, title.c_str()));
}

rv::vk::Surface rv::Window::Surface(Instance& instance) const
{
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = hwnd;
	createInfo.hinstance = wndClass.windowClass.hInstance;
	rv_check_vkr(vkCreateWin32SurfaceKHR(instance.instance, &createInfo, nullptr, &surface));
	return surface;
}

bool rv::Window::Minimized() const
{
	return minimized;
}

void rv::Window::Minimize()
{
	ShowWindow(hwnd, SW_HIDE);
}

const rv::Size& rv::Window::GetSize() const
{
	return size;
}

const rv::uint& rv::Window::DPI() const
{
	return dpi;
}

const rv::WindowClass rv::Window::CreateClass()
{
	WNDCLASSEX wnd {};

	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpfnWndProc = StaticWindowProcSetup;
	wnd.hInstance = GetModuleHandle(nullptr);
	wnd.lpszClassName = "Rave Window Class";

	return WindowClass(wnd);
}

LRESULT rv::Window::StaticWindowProcSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_CREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::StaticWindowProc));
		return pWnd->WindowProc(hwnd, msg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

LRESULT rv::Window::StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* window = rv_not_null_win32(reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA)));
	return window->WindowProc(hwnd, msg, wParam, lParam);
}

const char* WMToString(UINT wm);

LRESULT rv::Window::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED)
				minimized = true;
			else if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED)
				minimized = false;
			size = Size(LOWORD(lParam), HIWORD(lParam));
			queue.Push<WindowResizeEvent>(size);
		}
		return 0;
		case WM_DPICHANGED:
		{
			dpi = LOWORD(wParam);
		}
		return 0;

		//keyboard
		case WM_KEYDOWN:
		{
			keyboard.flagged[u8(wParam)] = true;
			keyboard.pressed[u8(wParam)] = true;
			queue.Push<KeyEvent>({ u8(wParam), RV_PRESSED });
		}
		return 0;
		case WM_KEYUP:
		{
			keyboard.flagged[u8(wParam)] = false;
			queue.Push<KeyEvent>({ u8(wParam), RV_RELEASED });
		}
		return 0;
		case WM_CHAR:
		{
			std::lock_guard<std::mutex> guard(keyboard.mutex);
			keyboard.inputChars.Expose().push_back(char(wParam));
			queue.Push<InputCharEvent>(char(wParam));
		}
		return 0;

		//mouse
		case WM_MOUSEMOVE:
		{
			POINTS p = MAKEPOINTS(lParam);
			mouse.pos = { p.x, p.y };
			queue.Push<MouseMoveEvent>( mouse.pos );
		}
		return 0;
		case WM_LBUTTONDOWN:
		{
			POINTS p = MAKEPOINTS(lParam);

			mouse.left.flag = true;
			mouse.left.pressed = true;

			queue.Push<MouseButtonEvent>(MouseButtonEvent({ p.x, p.y }, RV_PRESSED, RV_MB_LEFT));
		}
		return 0;
		case WM_MBUTTONDOWN:
		{
			POINTS p = MAKEPOINTS(lParam);

			mouse.middle.flag = true;
			mouse.middle.pressed = true;

			queue.Push<MouseButtonEvent>(MouseButtonEvent({ p.x, p.y }, RV_PRESSED, RV_MB_MIDDLE));
		}
		return 0;
		case WM_RBUTTONDOWN:
		{
			POINTS p = MAKEPOINTS(lParam);

			mouse.right.flag = true;
			mouse.right.pressed = true;

			queue.Push<MouseButtonEvent>(MouseButtonEvent({ p.x, p.y }, RV_PRESSED, RV_MB_RIGHT));
		}
		return 0;
		case WM_XBUTTONDOWN:
		{
			POINTS p = MAKEPOINTS(lParam);

			if (HIWORD(wParam) == XBUTTON1)
			{
				mouse.x1.flag = true;
				mouse.x1.pressed = true;
				queue.Push<MouseButtonEvent>(MouseButtonEvent({ p.x, p.y }, RV_PRESSED, RV_MB_X1));
			}
			else
			{
				mouse.x2.flag = true;
				mouse.x2.pressed = true;
				queue.Push<MouseButtonEvent>(MouseButtonEvent({ p.x, p.y }, RV_PRESSED, RV_MB_X2));
			}
		}
		return 0;
		case WM_LBUTTONUP:
		{
			POINTS p = MAKEPOINTS(lParam);
			mouse.left.pressed = false;
			queue.Push<MouseButtonEvent>(MouseButtonEvent({ p.x, p.y }, RV_RELEASED, RV_MB_LEFT));
		}
		return 0;
		case WM_MBUTTONUP:
		{
			POINTS p = MAKEPOINTS(lParam);
			mouse.middle.pressed = false;
			queue.Push<MouseButtonEvent>(MouseButtonEvent({ p.x, p.y }, RV_RELEASED, RV_MB_MIDDLE));
		}
		return 0;
		case WM_RBUTTONUP:
		{
			POINTS p = MAKEPOINTS(lParam);
			mouse.right.pressed = false;
			queue.Push<MouseButtonEvent>(MouseButtonEvent({ p.x, p.y }, RV_RELEASED, RV_MB_RIGHT));
		}
		return 0;
		case WM_XBUTTONUP:
		{
			POINTS p = MAKEPOINTS(lParam);

			if (HIWORD(wParam) == XBUTTON1)
			{
				mouse.x1.pressed = false;
				queue.Push<MouseButtonEvent>(MouseButtonEvent({ p.x, p.y }, RV_RELEASED, RV_MB_X1));
			}
			else
			{
				mouse.x2.pressed = false;
				queue.Push<MouseButtonEvent>(MouseButtonEvent({ p.x, p.y }, RV_RELEASED, RV_MB_X2));
			}
		}
		return 0;
		case WM_INPUT:
		{
			if (GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT)
				return DefWindowProc(hwnd, msg, wParam, lParam);
			if (GET_RAWINPUT_CODE_WPARAM(wParam) != RIM_INPUTSINK)
			{
				UINT size = 0;
				rv_assert(GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size, sizeof(RAWINPUT)) != (UINT)-1);
				std::vector<unsigned char> buffer(size);
				rv_assert(GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer.data(), &size, sizeof(RAWINPUT)) != (UINT)-1);
				RAWINPUT& input = *reinterpret_cast<RAWINPUT*>(buffer.data());

				if (input.header.dwType == RIM_TYPEMOUSE)
				{
					mouse.delta.Expose() += Vector2( input.data.mouse.lLastX, input.data.mouse.lLastY );
					if (input.data.mouse.usButtonFlags == RI_MOUSE_WHEEL)
					{
						SHORT delta = (SHORT)input.data.mouse.usButtonData;
						mouse.scroll += (int)delta;
					}
				}
			}
		}
		return 0;

		case WM_CLOSE:
		{
			Close();
		}
		return 0;
		case WM_DESTROY:
		{
			this->hwnd = nullptr;
		}
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

WORD GetWindowDPI(HWND hWnd)
{
	// Try to get the DPI setting for the monitor where the given window is located.
	// This API is Windows 8.1+.
	if (hWnd)
	{
		HMODULE hShcore = LoadLibrary("shcore");
		if (hShcore)
		{
			PGetDpiForMonitor pGetDpiForMonitor =
				reinterpret_cast<PGetDpiForMonitor>(GetProcAddress(hShcore, "GetDpiForMonitor"));
			if (pGetDpiForMonitor)
			{
				HMONITOR hMonitor = rv_not_null_win32(MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY));
				UINT uiDpiX;
				UINT uiDpiY;
				rv_check_hr(pGetDpiForMonitor(hMonitor, 0, &uiDpiX, &uiDpiY));
				return static_cast<WORD>(uiDpiX);
			}
		}
	}

	// We couldn't get the window's DPI above, so get the DPI of the primary monitor
	// using an API that is available in all Windows versions.
	HDC hScreenDC = GetDC(0);
	int iDpiX = GetDeviceCaps(hScreenDC, LOGPIXELSX);
	ReleaseDC(0, hScreenDC);

	return static_cast<WORD>(iDpiX);
}

const char* WMToString(UINT wm)
{
	switch (wm)
	{
	case WM_NULL: return "WM_NULL";
	case WM_CREATE: return "WM_CREATE";
	case WM_DESTROY: return "WM_DESTROY";
	case WM_MOVE: return "WM_MOVE";
	case WM_SIZE: return "WM_SIZE";
	case WM_ACTIVATE: return "WM_ACTIVATE";
	case WM_SETFOCUS: return "WM_SETFOCUS";
	case WM_KILLFOCUS: return "WM_KILLFOCUS";
	case WM_ENABLE: return "WM_ENABLE";
	case WM_SETREDRAW: return "WM_SETREDRAW";
	case WM_SETTEXT: return "WM_SETTEXT";
	case WM_GETTEXT: return "WM_GETTEXT";
	case WM_GETTEXTLENGTH: return "WM_GETTEXTLENGTH";
	case WM_PAINT: return "WM_PAINT";
	case WM_CLOSE: return "WM_CLOSE";
	case WM_QUERYENDSESSION: return "WM_QUERYENDSESSION";
	case WM_QUIT: return "WM_QUIT";
	case WM_QUERYOPEN: return "WM_QUERYOPEN";
	case WM_ERASEBKGND: return "WM_ERASEBKGND";
	case WM_SYSCOLORCHANGE: return "WM_SYSCOLORCHANGE";
	case WM_ENDSESSION: return "WM_ENDSESSION";
	case WM_SHOWWINDOW: return "WM_SHOWWINDOW";
	case WM_WININICHANGE: return "WM_WININICHANGE";
	case WM_DEVMODECHANGE: return "WM_DEVMODECHANGE";
	case WM_ACTIVATEAPP: return "WM_ACTIVATEAPP";
	case WM_FONTCHANGE: return "WM_FONTCHANGE";
	case WM_TIMECHANGE: return "WM_TIMECHANGE";
	case WM_CANCELMODE: return "WM_CANCELMODE";
	case WM_SETCURSOR: return "WM_SETCURSOR";
	case WM_MOUSEACTIVATE: return "WM_MOUSEACTIVATE";
	case WM_CHILDACTIVATE: return "WM_CHILDACTIVATE";
	case WM_QUEUESYNC: return "WM_QUEUESYNC";
	case WM_GETMINMAXINFO: return "WM_GETMINMAXINFO";
	case WM_PAINTICON: return "WM_PAINTICON";
	case WM_ICONERASEBKGND: return "WM_ICONERASEBKGND";
	case WM_NEXTDLGCTL: return "WM_NEXTDLGCTL";
	case WM_SPOOLERSTATUS: return "WM_SPOOLERSTATUS";
	case WM_DRAWITEM: return "WM_DRAWITEM";
	case WM_MEASUREITEM: return "WM_MEASUREITEM";
	case WM_DELETEITEM: return "WM_DELETEITEM";
	case WM_VKEYTOITEM: return "WM_VKEYTOITEM";
	case WM_CHARTOITEM: return "WM_CHARTOITEM";
	case WM_SETFONT: return "WM_SETFONT";
	case WM_GETFONT: return "WM_GETFONT";
	case WM_SETHOTKEY: return "WM_SETHOTKEY";
	case WM_GETHOTKEY: return "WM_GETHOTKEY";
	case WM_QUERYDRAGICON: return "WM_QUERYDRAGICON";
	case WM_COMPAREITEM: return "WM_COMPAREITEM";
	case WM_GETOBJECT: return "WM_GETOBJECT";
	case WM_COMPACTING: return "WM_COMPACTING";
	case WM_COMMNOTIFY: return "WM_COMMNOTIFY";
	case WM_WINDOWPOSCHANGING: return "WM_WINDOWPOSCHANGING";
	case WM_WINDOWPOSCHANGED: return "WM_WINDOWPOSCHANGED";
	case WM_POWER: return "WM_POWER";
	case WM_COPYDATA: return "WM_COPYDATA";
	case WM_CANCELJOURNAL: return "WM_CANCELJOURNAL";
	case WM_NOTIFY: return "WM_NOTIFY";
	case WM_INPUTLANGCHANGEREQUEST: return "WM_INPUTLANGCHANGEREQUEST";
	case WM_INPUTLANGCHANGE: return "WM_INPUTLANGCHANGE";
	case WM_TCARD: return "WM_TCARD";
	case WM_HELP: return "WM_HELP";
	case WM_USERCHANGED: return "WM_USERCHANGED";
	case WM_NOTIFYFORMAT: return "WM_NOTIFYFORMAT";
	case WM_CONTEXTMENU: return "WM_CONTEXTMENU";
	case WM_STYLECHANGING: return "WM_STYLECHANGING";
	case WM_STYLECHANGED: return "WM_STYLECHANGED";
	case WM_DISPLAYCHANGE: return "WM_DISPLAYCHANGE";
	case WM_GETICON: return "WM_GETICON";
	case WM_SETICON: return "WM_SETICON";
	case WM_NCCREATE: return "WM_NCCREATE";
	case WM_NCDESTROY: return "WM_NCDESTROY";
	case WM_NCCALCSIZE: return "WM_NCCALCSIZE";
	case WM_NCHITTEST: return "WM_NCHITTEST";
	case WM_NCPAINT: return "WM_NCPAINT";
	case WM_NCACTIVATE: return "WM_NCACTIVATE";
	case WM_GETDLGCODE: return "WM_GETDLGCODE";
	case WM_SYNCPAINT: return "WM_SYNCPAINT";
	case WM_NCMOUSEMOVE: return "WM_NCMOUSEMOVE";
	case WM_NCLBUTTONDOWN: return "WM_NCLBUTTONDOWN";
	case WM_NCLBUTTONUP: return "WM_NCLBUTTONUP";
	case WM_NCLBUTTONDBLCLK: return "WM_NCLBUTTONDBLCLK";
	case WM_NCRBUTTONDOWN: return "WM_NCRBUTTONDOWN";
	case WM_NCRBUTTONUP: return "WM_NCRBUTTONUP";
	case WM_NCRBUTTONDBLCLK: return "WM_NCRBUTTONDBLCLK";
	case WM_NCMBUTTONDOWN: return "WM_NCMBUTTONDOWN";
	case WM_NCMBUTTONUP: return "WM_NCMBUTTONUP";
	case WM_NCMBUTTONDBLCLK: return "WM_NCMBUTTONDBLCLK";
	case WM_NCXBUTTONDOWN: return "WM_NCXBUTTONDOWN";
	case WM_NCXBUTTONUP: return "WM_NCXBUTTONUP";
	case WM_NCXBUTTONDBLCLK: return "WM_NCXBUTTONDBLCLK";
	case EM_GETSEL: return "EM_GETSEL";
	case EM_SETSEL: return "EM_SETSEL";
	case EM_GETRECT: return "EM_GETRECT";
	case EM_SETRECT: return "EM_SETRECT";
	case EM_SETRECTNP: return "EM_SETRECTNP";
	case EM_SCROLL: return "EM_SCROLL";
	case EM_LINESCROLL: return "EM_LINESCROLL";
	case EM_SCROLLCARET: return "EM_SCROLLCARET";
	case EM_GETMODIFY: return "EM_GETMODIFY";
	case EM_SETMODIFY: return "EM_SETMODIFY";
	case EM_GETLINECOUNT: return "EM_GETLINECOUNT";
	case EM_LINEINDEX: return "EM_LINEINDEX";
	case EM_SETHANDLE: return "EM_SETHANDLE";
	case EM_GETHANDLE: return "EM_GETHANDLE";
	case EM_GETTHUMB: return "EM_GETTHUMB";
	case EM_LINELENGTH: return "EM_LINELENGTH";
	case EM_REPLACESEL: return "EM_REPLACESEL";
	case EM_GETLINE: return "EM_GETLINE";
	case EM_LIMITTEXT: return "EM_LIMITTEXT";
	case EM_CANUNDO: return "EM_CANUNDO";
	case EM_UNDO: return "EM_UNDO";
	case EM_FMTLINES: return "EM_FMTLINES";
	case EM_LINEFROMCHAR: return "EM_LINEFROMCHAR";
	case EM_SETTABSTOPS: return "EM_SETTABSTOPS";
	case EM_SETPASSWORDCHAR: return "EM_SETPASSWORDCHAR";
	case EM_EMPTYUNDOBUFFER: return "EM_EMPTYUNDOBUFFER";
	case EM_GETFIRSTVISIBLELINE: return "EM_GETFIRSTVISIBLELINE";
	case EM_SETREADONLY: return "EM_SETREADONLY";
	case EM_SETWORDBREAKPROC: return "EM_SETWORDBREAKPROC";
	case EM_GETWORDBREAKPROC: return "EM_GETWORDBREAKPROC";
	case EM_GETPASSWORDCHAR: return "EM_GETPASSWORDCHAR";
	case EM_SETMARGINS: return "EM_SETMARGINS";
	case EM_GETMARGINS: return "EM_GETMARGINS";
	case EM_GETLIMITTEXT: return "EM_GETLIMITTEXT";
	case EM_POSFROMCHAR: return "EM_POSFROMCHAR";
	case EM_CHARFROMPOS: return "EM_CHARFROMPOS";
	case EM_SETIMESTATUS: return "EM_SETIMESTATUS";
	case EM_GETIMESTATUS: return "EM_GETIMESTATUS";
	case SBM_SETPOS: return "SBM_SETPOS";
	case SBM_GETPOS: return "SBM_GETPOS";
	case SBM_SETRANGE: return "SBM_SETRANGE";
	case SBM_GETRANGE: return "SBM_GETRANGE";
	case SBM_ENABLE_ARROWS: return "SBM_ENABLE_ARROWS";
	case SBM_SETRANGEREDRAW: return "SBM_SETRANGEREDRAW";
	case SBM_SETSCROLLINFO: return "SBM_SETSCROLLINFO";
	case SBM_GETSCROLLINFO: return "SBM_GETSCROLLINFO";
	case SBM_GETSCROLLBARINFO: return "SBM_GETSCROLLBARINFO";
	case BM_GETCHECK: return "BM_GETCHECK";
	case BM_SETCHECK: return "BM_SETCHECK";
	case BM_GETSTATE: return "BM_GETSTATE";
	case BM_SETSTATE: return "BM_SETSTATE";
	case BM_SETSTYLE: return "BM_SETSTYLE";
	case BM_CLICK: return "BM_CLICK";
	case BM_GETIMAGE: return "BM_GETIMAGE";
	case BM_SETIMAGE: return "BM_SETIMAGE";
	case BM_SETDONTCLICK: return "BM_SETDONTCLICK";
	case WM_INPUT: return "WM_INPUT";
	case WM_KEYDOWN: return "WM_KEYDOWN";
	case WM_KEYUP: return "WM_KEYUP";
	case WM_CHAR: return "WM_CHAR";
	case WM_DEADCHAR: return "WM_DEADCHAR";
	case WM_SYSKEYDOWN: return "WM_SYSKEYDOWN";
	case WM_SYSKEYUP: return "WM_SYSKEYUP";
	case WM_SYSCHAR: return "WM_SYSCHAR";
	case WM_SYSDEADCHAR: return "WM_SYSDEADCHAR";
	case WM_UNICHAR: return "WM_UNICHAR";
	case WM_IME_STARTCOMPOSITION: return "WM_IME_STARTCOMPOSITION";
	case WM_IME_ENDCOMPOSITION: return "WM_IME_ENDCOMPOSITION";
	case WM_IME_COMPOSITION: return "WM_IME_COMPOSITION";
	case WM_INITDIALOG: return "WM_INITDIALOG";
	case WM_COMMAND: return "WM_COMMAND";
	case WM_SYSCOMMAND: return "WM_SYSCOMMAND";
	case WM_TIMER: return "WM_TIMER";
	case WM_HSCROLL: return "WM_HSCROLL";
	case WM_VSCROLL: return "WM_VSCROLL";
	case WM_INITMENU: return "WM_INITMENU";
	case WM_INITMENUPOPUP: return "WM_INITMENUPOPUP";
	case WM_MENUSELECT: return "WM_MENUSELECT";
	case WM_MENUCHAR: return "WM_MENUCHAR";
	case WM_ENTERIDLE: return "WM_ENTERIDLE";
	case WM_MENURBUTTONUP: return "WM_MENURBUTTONUP";
	case WM_MENUDRAG: return "WM_MENUDRAG";
	case WM_MENUGETOBJECT: return "WM_MENUGETOBJECT";
	case WM_UNINITMENUPOPUP: return "WM_UNINITMENUPOPUP";
	case WM_MENUCOMMAND: return "WM_MENUCOMMAND";
	case WM_CHANGEUISTATE: return "WM_CHANGEUISTATE";
	case WM_UPDATEUISTATE: return "WM_UPDATEUISTATE";
	case WM_QUERYUISTATE: return "WM_QUERYUISTATE";
	case WM_CTLCOLORMSGBOX: return "WM_CTLCOLORMSGBOX";
	case WM_CTLCOLOREDIT: return "WM_CTLCOLOREDIT";
	case WM_CTLCOLORLISTBOX: return "WM_CTLCOLORLISTBOX";
	case WM_CTLCOLORBTN: return "WM_CTLCOLORBTN";
	case WM_CTLCOLORDLG: return "WM_CTLCOLORDLG";
	case WM_CTLCOLORSCROLLBAR: return "WM_CTLCOLORSCROLLBAR";
	case WM_CTLCOLORSTATIC: return "WM_CTLCOLORSTATIC";
	case WM_MOUSEFIRST: return "WM_MOUSEFIRST";
	case WM_LBUTTONDOWN: return "WM_LBUTTONDOWN";
	case WM_LBUTTONUP: return "WM_LBUTTONUP";
	case WM_LBUTTONDBLCLK: return "WM_LBUTTONDBLCLK";
	case WM_RBUTTONDOWN: return "WM_RBUTTONDOWN";
	case WM_RBUTTONUP: return "WM_RBUTTONUP";
	case WM_RBUTTONDBLCLK: return "WM_RBUTTONDBLCLK";
	case WM_MBUTTONDOWN: return "WM_MBUTTONDOWN";
	case WM_MBUTTONUP: return "WM_MBUTTONUP";
	case WM_MBUTTONDBLCLK: return "WM_MBUTTONDBLCLK";
	case WM_MOUSELAST: return "WM_MOUSELAST";
	case WM_MOUSEWHEEL: return "WM_MOUSEWHEEL";
	case WM_XBUTTONDOWN: return "WM_XBUTTONDOWN";
	case WM_XBUTTONUP: return "WM_XBUTTONUP";
	case WM_XBUTTONDBLCLK: return "WM_XBUTTONDBLCLK";
	case WM_PARENTNOTIFY: return "WM_PARENTNOTIFY";
	case WM_ENTERMENULOOP: return "WM_ENTERMENULOOP";
	case WM_EXITMENULOOP: return "WM_EXITMENULOOP";
	case WM_NEXTMENU: return "WM_NEXTMENU";
	case WM_SIZING: return "WM_SIZING";
	case WM_CAPTURECHANGED: return "WM_CAPTURECHANGED";
	case WM_MOVING: return "WM_MOVING";
	case WM_POWERBROADCAST: return "WM_POWERBROADCAST";
	case WM_DEVICECHANGE: return "WM_DEVICECHANGE";
	case WM_MDICREATE: return "WM_MDICREATE";
	case WM_MDIDESTROY: return "WM_MDIDESTROY";
	case WM_MDIACTIVATE: return "WM_MDIACTIVATE";
	case WM_MDIRESTORE: return "WM_MDIRESTORE";
	case WM_MDINEXT: return "WM_MDINEXT";
	case WM_MDIMAXIMIZE: return "WM_MDIMAXIMIZE";
	case WM_MDITILE: return "WM_MDITILE";
	case WM_MDICASCADE: return "WM_MDICASCADE";
	case WM_MDIICONARRANGE: return "WM_MDIICONARRANGE";
	case WM_MDIGETACTIVE: return "WM_MDIGETACTIVE";
	case WM_MDISETMENU: return "WM_MDISETMENU";
	case WM_ENTERSIZEMOVE: return "WM_ENTERSIZEMOVE";
	case WM_EXITSIZEMOVE: return "WM_EXITSIZEMOVE";
	case WM_DROPFILES: return "WM_DROPFILES";
	case WM_MDIREFRESHMENU: return "WM_MDIREFRESHMENU";
	case WM_IME_SETCONTEXT: return "WM_IME_SETCONTEXT";
	case WM_IME_NOTIFY: return "WM_IME_NOTIFY";
	case WM_IME_CONTROL: return "WM_IME_CONTROL";
	case WM_IME_COMPOSITIONFULL: return "WM_IME_COMPOSITIONFULL";
	case WM_IME_SELECT: return "WM_IME_SELECT";
	case WM_IME_CHAR: return "WM_IME_CHAR";
	case WM_IME_REQUEST: return "WM_IME_REQUEST";
	case WM_IME_KEYDOWN: return "WM_IME_KEYDOWN";
	case WM_IME_KEYUP: return "WM_IME_KEYUP";
	case WM_NCMOUSEHOVER: return "WM_NCMOUSEHOVER";
	case WM_MOUSEHOVER: return "WM_MOUSEHOVER";
	case WM_NCMOUSELEAVE: return "WM_NCMOUSELEAVE";
	case WM_MOUSELEAVE: return "WM_MOUSELEAVE";
	case WM_CUT: return "WM_CUT";
	case WM_COPY: return "WM_COPY";
	case WM_PASTE: return "WM_PASTE";
	case WM_CLEAR: return "WM_CLEAR";
	case WM_UNDO: return "WM_UNDO";
	case WM_RENDERFORMAT: return "WM_RENDERFORMAT";
	case WM_RENDERALLFORMATS: return "WM_RENDERALLFORMATS";
	case WM_DESTROYCLIPBOARD: return "WM_DESTROYCLIPBOARD";
	case WM_DRAWCLIPBOARD: return "WM_DRAWCLIPBOARD";
	case WM_PAINTCLIPBOARD: return "WM_PAINTCLIPBOARD";
	case WM_VSCROLLCLIPBOARD: return "WM_VSCROLLCLIPBOARD";
	case WM_SIZECLIPBOARD: return "WM_SIZECLIPBOARD";
	case WM_ASKCBFORMATNAME: return "WM_ASKCBFORMATNAME";
	case WM_CHANGECBCHAIN: return "WM_CHANGECBCHAIN";
	case WM_HSCROLLCLIPBOARD: return "WM_HSCROLLCLIPBOARD";
	case WM_QUERYNEWPALETTE: return "WM_QUERYNEWPALETTE";
	case WM_PALETTEISCHANGING: return "WM_PALETTEISCHANGING";
	case WM_PALETTECHANGED: return "WM_PALETTECHANGED";
	case WM_HOTKEY: return "WM_HOTKEY";
	case WM_PRINT: return "WM_PRINT";
	case WM_PRINTCLIENT: return "WM_PRINTCLIENT";
	case WM_APPCOMMAND: return "WM_APPCOMMAND";
	case WM_HANDHELDFIRST: return "WM_HANDHELDFIRST";
	case WM_HANDHELDLAST: return "WM_HANDHELDLAST";
	case WM_AFXFIRST: return "WM_AFXFIRST";
	case WM_AFXLAST: return "WM_AFXLAST";
	case WM_PENWINFIRST: return "WM_PENWINFIRST";
	case WM_PENWINLAST: return "WM_PENWINLAST";
	case DM_GETDEFID: return "DM_GETDEFID";
	case DM_SETDEFID: return "DM_SETDEFID";
	case PSM_SETWIZBUTTONS: return "PSM_SETWIZBUTTONS";
	case PSM_PRESSBUTTON: return "PSM_PRESSBUTTON";
	case PSM_SETCURSELID: return "PSM_SETCURSELID";
	case PSM_SETFINISHTEXTA: return "PSM_SETFINISHTEXTA";
	case PSM_GETTABCONTROL: return "PSM_GETTABCONTROL";
	case PSM_ISDIALOGMESSAGE: return "PSM_ISDIALOGMESSAGE";
	case PSM_GETCURRENTPAGEHWND: return "PSM_GETCURRENTPAGEHWND";
	case PSM_INSERTPAGE: return "PSM_INSERTPAGE";
	case PSM_SETTITLEW: return "PSM_SETTITLEW";
	case PSM_SETFINISHTEXTW: return "PSM_SETFINISHTEXTW";
	case PSM_SETHEADERTITLEA: return "PSM_SETHEADERTITLEA";
	case PSM_SETHEADERTITLEW: return "PSM_SETHEADERTITLEW";
	case PSM_SETHEADERSUBTITLEA: return "PSM_SETHEADERSUBTITLEA";
	case PSM_SETHEADERSUBTITLEW: return "PSM_SETHEADERSUBTITLEW";
	case PSM_HWNDTOINDEX: return "PSM_HWNDTOINDEX";
	case PSM_INDEXTOHWND: return "PSM_INDEXTOHWND";
	case PSM_PAGETOINDEX: return "PSM_PAGETOINDEX";
	case PSM_INDEXTOPAGE: return "PSM_INDEXTOPAGE";
	case PSM_IDTOINDEX: return "PSM_IDTOINDEX";
	case PSM_INDEXTOID: return "PSM_INDEXTOID";
	case PSM_GETRESULT: return "PSM_GETRESULT";
	case PSM_RECALCPAGESIZES: return "PSM_RECALCPAGESIZES";
	case WM_APP: return "WM_APP";
	}
	return "???";
}

#endif