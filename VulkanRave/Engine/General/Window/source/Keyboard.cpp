#include "General/Window/Keyboard.h"

bool rv::Keyboard::Pressed(Key key) const
{
	return pressed[key];
}

bool rv::Keyboard::Released(Key key) const
{
	return !pressed[key];
}

bool rv::Keyboard::Flagged(Key key)
{
	return flagged[key].Get();
}

std::string rv::Keyboard::Input()
{
	std::lock_guard<std::mutex> guard(mutex);
	return inputChars.Get();
}

void rv::Keyboard::Clear()
{
	pressed.reset();
	flagged.fill({});
}
