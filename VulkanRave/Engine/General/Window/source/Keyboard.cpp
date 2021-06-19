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

const std::string& rv::Keyboard::PeekInput() const
{
	return inputChars.Peek();
}

std::string rv::Keyboard::Input()
{
	return inputChars.Get();
}

void rv::Keyboard::Clear()
{
	pressed.reset();
	flagged.fill({});
}
