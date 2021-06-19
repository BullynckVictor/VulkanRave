#pragma once
#include "Utilities/Flag.h"
#include "Utilities/Types.h"
#include <bitset>
#include <array>
#include <string>

namespace rv
{
	class Keyboard
	{
	public:
		using Key = u8;

		bool Pressed(Key key) const;
		bool Released(Key key) const;
		bool Flagged(Key key);

		const std::string& PeekInput() const;
		std::string Input();

		void Clear();

	private:
		std::bitset<256> pressed;
		std::array<Flag<bool>, 256> flagged;
		Flag<std::string> inputChars;

		friend class Window;
	};
}