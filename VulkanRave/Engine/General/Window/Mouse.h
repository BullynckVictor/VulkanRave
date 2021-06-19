#pragma once
#include "Utilities/Flag.h"
#include "Utilities/Vector.h"

namespace rv
{
	class Mouse
	{
	public:
		class Button
		{
		public:
			bool Pressed() const;
			bool Flagged();
			bool Peek() const;

		private:
			bool pressed = false;
			Flag<bool> flag;

			friend class Window;
		};

		const Point& Position() const;
		Point Delta();
		const Point& PeekDelta() const;

		const int& PeekScroll() const;
		int Scroll();

		Button left;
		Button middle;
		Button right;
		Button x1;
		Button x2;

	private:
		Point pos;
		Flag<Point> delta;

		int scroll = 0;

		friend class Window;
	};
}