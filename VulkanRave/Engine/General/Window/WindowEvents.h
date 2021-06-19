#pragma once
#include "General/Event.h"
#include "Utilities/Vector.h"

namespace rv
{
	struct WindowResizeEvent : public Event
	{
		static const EventID static_event;

		WindowResizeEvent() : Event(static_event) {}
		WindowResizeEvent(const Size& size) : Event(static_event), size(size) {}

		Size size;
	};

	struct KeyPressedEvent : public Event
	{
		static const EventID static_event;

		KeyPressedEvent() : Event(static_event) {}
		KeyPressedEvent(uchar key) : Event(static_event), key(key) {}

		uchar key = 0;
	};

	struct KeyReleasedEvent : public Event
	{
		static const EventID static_event;

		KeyReleasedEvent() : Event(static_event) {}
		KeyReleasedEvent(uchar key) : Event(static_event), key(key) {}

		uchar key = 0;
	};

	struct InputCharEvent : public Event
	{
		static const EventID static_event;

		InputCharEvent() : Event(static_event) {}
		InputCharEvent(char key) : Event(static_event), key(key) {}

		char key = 0;
	};
}