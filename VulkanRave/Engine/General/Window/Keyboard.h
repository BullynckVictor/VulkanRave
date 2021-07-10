#pragma once
#include "Engine/Utilities/Flag.h"
#include "Engine/Utilities/Types.h"
#include <bitset>
#include <array>
#include <string>
#include <mutex>

namespace rv
{
	class Keyboard
	{
	public:
		using Key = u8;

		bool Pressed(Key key) const;
		bool Released(Key key) const;
		bool Flagged(Key key);

		std::string Input();

		void Clear();

	private:
		std::bitset<256> pressed;
		std::array<Flag<bool>, 256> flagged;
		Flag<std::string> inputChars;

		std::mutex mutex;

		friend class Window;
	};
}