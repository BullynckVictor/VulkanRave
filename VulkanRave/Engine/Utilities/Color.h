#pragma once

namespace rv
{
	namespace detail
	{
		template<typename T1, typename T2>
		struct precedence
		{
//			static_assert(false, "rv::detail::precedence<T1, T2> must be implemented");
		};

		template<> struct precedence<float, unsigned char> { using high = float; using low = unsigned char; };
		template<> struct precedence<unsigned char, float> { using high = float; using low = unsigned char; };

		template<typename T>
		struct default_value
		{
//			static_assert(false, "rv::detail::default_value<T> must be implemented");
		};

		template<> struct default_value<unsigned char>	{ static constexpr unsigned char value = 255; };
		template<> struct default_value<float>			{ static constexpr float value = 1.0f; };

		static constexpr bool isdigit(char c)
		{
			return c <= '9' && c >= '0';
		}
		static constexpr char tolower(char c)
		{
			if (c <= 'Z' && c >= 'A')
				return c - ('A' - 'a');
			return c;
		}
		static constexpr unsigned char byte(const char* hex)
		{
			unsigned char c = 0;
			char lower0 = detail::tolower(hex[0]);
			char lower1 = detail::tolower(hex[1]);
			if (detail::isdigit(hex[0]))
				c += (hex[0] - '0') * 16;
			else if (lower0 >= 'a' && lower0 <= 'f')
				c += (lower0 - 'a' + 10) * 16;
			else
				return 0;

			if (detail::isdigit(hex[1]))
				c += (hex[1] - '0');
			else if (lower1 >= 'a' && lower1 <= 'f')
				c += (lower1 - 'a' + 10);
			else
				return 0;

			return c;
		}
	}

	template<typename T>
	struct ColorT
	{
		constexpr ColorT() = default;
		constexpr ColorT(T r, T g, T b, T a = detail::default_value<T>::value) : r(r), g(g), b(b), a(a) {}

		template<typename T2>
		constexpr explicit operator ColorT<T2>() const 
		{
			ColorT<T2> color;
			color.r = (T2)((typename detail::precedence<T, T2>::high)r / (typename detail::precedence<T, T2>::high)detail::default_value<T>::value * (typename detail::precedence<T, T2>::high)detail::default_value<T2>::value);
			color.g = (T2)((typename detail::precedence<T, T2>::high)g / (typename detail::precedence<T, T2>::high)detail::default_value<T>::value * (typename detail::precedence<T, T2>::high)detail::default_value<T2>::value);
			color.b = (T2)((typename detail::precedence<T, T2>::high)b / (typename detail::precedence<T, T2>::high)detail::default_value<T>::value * (typename detail::precedence<T, T2>::high)detail::default_value<T2>::value);
			color.a = (T2)((typename detail::precedence<T, T2>::high)a / (typename detail::precedence<T, T2>::high)detail::default_value<T>::value * (typename detail::precedence<T, T2>::high)detail::default_value<T2>::value);
			return color;
		}

		constexpr bool operator== (const ColorT& rhs) const { return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a; }
		constexpr bool operator!= (const ColorT& rhs) const { return r != rhs.r || g != rhs.g || b != rhs.b || a != rhs.a; }

		constexpr ColorT& operator+= (const ColorT& rhs) { r += rhs.r; g += rhs.g; b += rhs.b; a += rhs.a; return *this; };
		constexpr ColorT& operator-= (const ColorT& rhs) { r -= rhs.r; g -= rhs.g; b -= rhs.b; a -= rhs.a; return *this; };
		constexpr ColorT& operator*= (const ColorT& rhs) { r *= rhs.r; g *= rhs.g; b *= rhs.b; a *= rhs.a; return *this; };
		constexpr ColorT& operator/= (const ColorT& rhs) { r /= rhs.r; g /= rhs.g; b /= rhs.b; a /= rhs.a; return *this; };

		constexpr ColorT operator+ (const ColorT& rhs) { ColorT temp = *this; return temp += rhs; }
		constexpr ColorT operator- (const ColorT& rhs) { ColorT temp = *this; return temp -= rhs; }
		constexpr ColorT operator* (const ColorT& rhs) { ColorT temp = *this; return temp *= rhs; }
		constexpr ColorT operator/ (const ColorT& rhs) { ColorT temp = *this; return temp /= rhs; }

		constexpr ColorT& operator+= (const T& rhs) { r += rhs; g += rhs; b += rhs; a += rhs; return *this; };
		constexpr ColorT& operator-= (const T& rhs) { r -= rhs; g -= rhs; b -= rhs; a -= rhs; return *this; };
		constexpr ColorT& operator*= (const T& rhs) { r *= rhs; g *= rhs; b *= rhs; a *= rhs; return *this; };
		constexpr ColorT& operator/= (const T& rhs) { r /= rhs; g /= rhs; b /= rhs; a /= rhs; return *this; };

		constexpr ColorT operator+ (const T& rhs) { ColorT temp = *this; return temp += rhs; }
		constexpr ColorT operator- (const T& rhs) { ColorT temp = *this; return temp -= rhs; }
		constexpr ColorT operator* (const T& rhs) { ColorT temp = *this; return temp *= rhs; }
		constexpr ColorT operator/ (const T& rhs) { ColorT temp = *this; return temp /= rhs; }

				T* begin()				{ return &r; }
				T* end()				{ return &a + 1; }
		const	T* begin()		const	{ return &r; }
		const	T* end()		const	{ return &a + 1; }
		const	T* cbegin()		const	{ return &r; }
		const	T* cend()		const	{ return &a + 1; }
				T* rbegin()				{ return &r - 1; }
				T* rend()				{ return &a; }
		const	T* rbegin()		const	{ return &r - 1; }
		const	T* rend()		const	{ return &a; }
		const	T* crbegin()	const	{ return &r - 1; }
		const	T* crend()		const	{ return &a; }

		static constexpr size_t size() { return 4; }

		T r = (T)0;
		T g = (T)0;
		T b = (T)0;
		T a = detail::default_value<T>::value;
	};

	typedef ColorT<unsigned char>	Color;
	typedef ColorT<float>			FColor;

	namespace Colors
	{
		static constexpr Color Make(unsigned char r, unsigned char g, unsigned char b, unsigned char a = detail::default_value<unsigned char>::value)
		{
			Color color;
			color.r = r;
			color.g = g;
			color.b = b;
			color.a = a;
			return color;
		}
		static constexpr Color Make(const char(&string)[7])
		{
			Color color{};
			color.r = detail::byte(string + 0);
			color.g = detail::byte(string + 2);
			color.b = detail::byte(string + 4);
			return color;
		}
		static constexpr Color Make(const char(&string)[9])
		{
			Color color{};
			color.r = detail::byte(string + 0);
			color.g = detail::byte(string + 2);
			color.b = detail::byte(string + 4);
			color.a = detail::byte(string + 6);
			return color;
		}
		static constexpr Color Transparent	= Make(   0,   0,   0,   0 );
		static constexpr Color White		= Make( 255, 255, 255 );
		static constexpr Color Black		= Make(   0,   0,   0 );
		static constexpr Color Red			= Make( 255,   0,   0 );
		static constexpr Color Green		= Make(   0, 255,   0 );
		static constexpr Color Blue			= Make(   0,   0, 255 );
		static constexpr Color Yellow		= Make( 255, 255,   0 );
		static constexpr Color Cyan			= Make(   0, 255, 255 );
		static constexpr Color Magenta		= Make( 255,   0, 255 );
		static constexpr Color Gray			= Make( 128, 128, 128 );
		static constexpr Color LightGray	= Make( 211, 211, 211 );
	}
	namespace FColors
	{
		static constexpr FColor Make(unsigned char r, unsigned char g, unsigned char b, unsigned char a = detail::default_value<unsigned char>::value)
		{
			return static_cast<FColor>(Colors::Make(r, g, b, a));
		}
		static constexpr FColor Make(const char(&string)[7])
		{
			return static_cast<FColor>(Colors::Make(string));
		}
		static constexpr FColor Make(const char(&string)[9])
		{
			return static_cast<FColor>(Colors::Make(string));
		}
		static constexpr FColor Transparent	= Make(   0,   0,   0,   0 );
		static constexpr FColor White		= Make( 255, 255, 255 );
		static constexpr FColor Black		= Make(   0,   0,   0 );
		static constexpr FColor Red			= Make( 255,   0,   0 );
		static constexpr FColor Green		= Make(   0, 255,   0 );
		static constexpr FColor Blue		= Make(   0,   0, 255 );
		static constexpr FColor Yellow		= Make( 255, 255,   0 );
		static constexpr FColor Cyan		= Make(   0, 255, 255 );
		static constexpr FColor Magenta		= Make( 255,   0, 255 );
		static constexpr FColor Gray		= Make( 128, 128, 128 );
		static constexpr FColor LightGray	= Make( 211, 211, 211 );
	}
}