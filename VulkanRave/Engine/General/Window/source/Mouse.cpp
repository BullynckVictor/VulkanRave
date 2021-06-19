#include "General/Window/Mouse.h"

bool rv::Mouse::Button::Pressed() const
{
    return pressed;
}

bool rv::Mouse::Button::Flagged()
{
	return flag.Get();
}

bool rv::Mouse::Button::Peek() const
{
	return flag.Peek();
}

const rv::Point& rv::Mouse::Position() const
{
	return pos;
}

rv::Point rv::Mouse::Delta()
{
	return delta.Get();
}

const rv::Point& rv::Mouse::PeekDelta() const
{
	return delta.Peek();
}

const int& rv::Mouse::PeekScroll() const
{
	return scroll;
}

int rv::Mouse::Scroll()
{
	int temp = scroll / 120;
	scroll -= temp * 120;
	return temp;
}
