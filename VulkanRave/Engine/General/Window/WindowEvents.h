#pragma once
#include "Engine/General/Event.h"
#include "Engine/Utilities/Vector.h"

namespace rv
{
	struct WindowResizeEvent : public Event
	{
		static const EventID static_event;

		WindowResizeEvent() : Event(static_event) {}
		WindowResizeEvent(const Size& size) : Event(static_event), size(size) {}

		Size size;
	};

	enum Press
	{
		RV_PRESSED,
		RV_RELEASED
	};

	struct KeyEvent : public Event
	{
		static const EventID static_event;

		KeyEvent() : Event(static_event) {}
		KeyEvent(uchar key, Press press) : Event(static_event), key(key), press(press) {}

		uchar key = 0;
		Press press = RV_RELEASED;
	};

	struct InputCharEvent : public Event
	{
		static const EventID static_event;

		InputCharEvent() : Event(static_event) {}
		InputCharEvent(char key) : Event(static_event), key(key) {}

		char key = 0;
	};

	struct MouseMoveEvent : public Event
	{
		static const EventID static_event;

		MouseMoveEvent() : Event(static_event) {}
		MouseMoveEvent(Point position) : Event(static_event), position(position) {}

		Point position;
	};

	enum MouseButton
	{
		RV_MB_LEFT,
		RV_MB_MIDDLE,
		RV_MB_RIGHT,
		RV_MB_X1,
		RV_MB_X2
	};

	struct MouseButtonEvent : public Event
	{
		static const EventID static_event;

		MouseButtonEvent() : Event(static_event) {}
		MouseButtonEvent(Point position, Press press, MouseButton button) : Event(static_event), position(position), press(press), button(button) {}

		Press press = RV_RELEASED;
		MouseButton button = RV_MB_LEFT;
		Point position;
	};
}