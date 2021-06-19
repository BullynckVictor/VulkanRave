#pragma once
#include "Utilities/Types.h"
#include <array>
#include <limits>

namespace rv
{
	namespace detail
	{
		template<typename T>
		static constexpr T sqrtNewtonRaphson(T x, T curr, T prev)
		{
			return curr == prev
				? curr
				: sqrtNewtonRaphson(x, (T)0.5 * (curr + x / curr), curr);
		}

		template<typename T>
		static constexpr T MID(T lo, T hi) { return (lo + hi + 1) / 2; }

		template<typename T>
		static constexpr T sqrt_helper(T x, T lo, T hi)
		{
			return lo == hi ? lo : ((x / MID(lo, hi) < MID(lo, hi))
				? sqrt_helper(x, lo, MID(lo, hi) - 1) : sqrt_helper(x, MID(lo, hi), hi));
		}
	}

	template<typename T>
	constexpr T sqrt(const T& x)
	{
		if constexpr (std::is_integral_v<T>)
		{
			return x < 0 ? 0 : detail::sqrt_helper<T>(x, 0, x / 2 + 1);
		}
		else
		{
			return x >= 0 && x < std::numeric_limits<T>::infinity()
				? detail::sqrtNewtonRaphson<T>(x, x, 0)
				: std::numeric_limits<T>::quiet_NaN();
		}
	}

	template<size_t S, typename T>
	struct Vector
	{
	public:
		constexpr Vector() = default;
		constexpr Vector(const T& value) { array.fill(value); }
		constexpr Vector(const std::array<T, S>& array) : array(array) {}
		constexpr Vector(std::array<T, S>&& array) : array(std::move(array)) {}

		template<typename T2>
		constexpr explicit operator Vector<S, T2>() const { Vector<S, T2> temp; meta_cast<0, T2>(temp.array.data(), array.data()); return temp; }

		constexpr bool operator== (const Vector& rhs) { return array == rhs.array; }
		constexpr bool operator!= (const Vector& rhs) { return array != rhs.array; }

		constexpr Vector& operator+= (const Vector& rhs) { meta_operation<0, add>(array.data(), rhs.array.data());  return *this; };
		constexpr Vector& operator-= (const Vector& rhs) { meta_operation<0, sub>(array.data(), rhs.array.data());  return *this; };
		constexpr Vector& operator*= (const Vector& rhs) { meta_operation<0, mul>(array.data(), rhs.array.data());  return *this; };
		constexpr Vector& operator/= (const Vector& rhs) { meta_operation<0, dev>(array.data(), rhs.array.data());  return *this; };

		constexpr Vector operator+ (const Vector& rhs) { Vector temp = *this; return temp += rhs; }
		constexpr Vector operator- (const Vector& rhs) { Vector temp = *this; return temp -= rhs; }
		constexpr Vector operator* (const Vector& rhs) { Vector temp = *this; return temp *= rhs; }
		constexpr Vector operator/ (const Vector& rhs) { Vector temp = *this; return temp /= rhs; }

		constexpr Vector& operator+= (const T& rhs) { meta_operation_const<0, addc>(array.data(), rhs);  return *this; };
		constexpr Vector& operator-= (const T& rhs) { meta_operation_const<0, subc>(array.data(), rhs);  return *this; };
		constexpr Vector& operator*= (const T& rhs) { meta_operation_const<0, mulc>(array.data(), rhs);  return *this; };
		constexpr Vector& operator/= (const T& rhs) { meta_operation_const<0, devc>(array.data(), rhs);  return *this; };

		constexpr Vector operator+ (const T& rhs) { Vector temp = *this; return temp += rhs; }
		constexpr Vector operator- (const T& rhs) { Vector temp = *this; return temp -= rhs; }
		constexpr Vector operator* (const T& rhs) { Vector temp = *this; return temp *= rhs; }
		constexpr Vector operator/ (const T& rhs) { Vector temp = *this; return temp /= rhs; }

		typename std::array<T, S>::iterator					begin()				{ return array.begin(); }
		typename std::array<T, S>::iterator					end()				{ return array.end(); }
		typename std::array<T, S>::const_iterator			begin()		const	{ return array.begin(); }
		typename std::array<T, S>::const_iterator			end()		const	{ return array.end(); }
		typename std::array<T, S>::const_iterator			cbegin()	const	{ return array.cbegin(); }
		typename std::array<T, S>::const_iterator			cend()		const	{ return array.cend(); }
		typename std::array<T, S>::reverse_iterator			rbegin()			{ return array.rbegin(); }
		typename std::array<T, S>::reverse_iterator			rend()				{ return array.rend(); }
		typename std::array<T, S>::const_reverse_iterator	rbegin()	const	{ return array.rbegin(); }
		typename std::array<T, S>::const_reverse_iterator	rend()		const	{ return array.rend(); }
		typename std::array<T, S>::const_reverse_iterator	crbegin()	const	{ return array.crbegin(); }
		typename std::array<T, S>::const_reverse_iterator	crend()		const	{ return array.crend(); }

		static constexpr size_t size() { return S; }

		constexpr T LengthSq() const { T length = 0; meta_length_sq<0>(length, array.data()); return length; }
		constexpr T Length() const { return sqrt(LengthSq()); }

		constexpr Vector& Normalize() { meta_normal<0>(Length(), array.data()); return *this; }
		constexpr Vector Normal() const { Vector temp = *this; return temp.Normal(); }

		std::array<T, S> array;

	private:
		template<size_t D, void(*op)(T*, const T*)>
		static constexpr void meta_operation(T* a, const T* b) { op(a, b); if constexpr (D < S - 1) meta_operation<D + 1, op>(a + 1, b + 1); }
		template<size_t D, void(*op)(T*, const T&)>
		static constexpr void meta_operation_const(T* a, const T& b) { op(a, b); if constexpr (D < S - 1) meta_operation_const<D + 1, op>(a + 1, b); }
		template<size_t D>
		static constexpr void meta_length_sq(T& len, const T* a) { len += *a * *a; if constexpr (D < S - 1) meta_length_sq<D + 1>(len, a + 1); }
		template<size_t D>
		static constexpr void meta_normal(const T& len, T* a) { *a /= len; if constexpr (D < S - 1) meta_normal<D + 1>(len, a + 1); }
		template<size_t D, typename T2>
		static constexpr void meta_cast(T2* a, const T* b) { *a = static_cast<T2>(*b); if constexpr (D < S - 1) meta_cast<D + 1, T2>(a + 1, b + 1); }

		static constexpr void add(T* a, const T* b) { *a += *b; }
		static constexpr void sub(T* a, const T* b) { *a -= *b; }
		static constexpr void mul(T* a, const T* b) { *a *= *b; }
		static constexpr void dev(T* a, const T* b) { *a /= *b; }

		static constexpr void addc(T* a, const T& b) { *a += *b; }
		static constexpr void subc(T* a, const T& b) { *a -= *b; }
		static constexpr void mulc(T* a, const T& b) { *a *= *b; }
		static constexpr void devc(T* a, const T& b) { *a /= *b; }
	};

	template<typename T>
	struct Vector<2, T>
	{
		constexpr Vector(const T& value = 0) : x(value), y(value) {}
		constexpr Vector(const T& x, const T& y) : x(x), y(y) {}

		template<typename T2>
		constexpr explicit operator Vector<2, T2>() const { return { static_cast<T2>(x), static_cast<T2>(y) }; }

		constexpr bool operator== (const Vector& rhs) { return x == rhs.x && y == rhs.y; }
		constexpr bool operator!= (const Vector& rhs) { return x != rhs.x || y != rhs.y; }

		constexpr Vector& operator+= (const Vector& rhs) { x += rhs.x; y += rhs.y; return *this; };
		constexpr Vector& operator-= (const Vector& rhs) { x -= rhs.x; y -= rhs.y; return *this; };
		constexpr Vector& operator*= (const Vector& rhs) { x *= rhs.x; y *= rhs.y; return *this; };
		constexpr Vector& operator/= (const Vector& rhs) { x /= rhs.x; y /= rhs.y; return *this; };

		constexpr Vector operator+ (const Vector& rhs) { Vector temp = *this; return temp += rhs; }
		constexpr Vector operator- (const Vector& rhs) { Vector temp = *this; return temp -= rhs; }
		constexpr Vector operator* (const Vector& rhs) { Vector temp = *this; return temp *= rhs; }
		constexpr Vector operator/ (const Vector& rhs) { Vector temp = *this; return temp /= rhs; }

		constexpr Vector& operator+= (const T& rhs) { x += rhs; y += rhs; return *this; };
		constexpr Vector& operator-= (const T& rhs) { x -= rhs; y -= rhs; return *this; };
		constexpr Vector& operator*= (const T& rhs) { x *= rhs; y *= rhs; return *this; };
		constexpr Vector& operator/= (const T& rhs) { x /= rhs; y /= rhs; return *this; };

		constexpr Vector operator+ (const T& rhs) { Vector temp = *this; return temp += rhs; }
		constexpr Vector operator- (const T& rhs) { Vector temp = *this; return temp -= rhs; }
		constexpr Vector operator* (const T& rhs) { Vector temp = *this; return temp *= rhs; }
		constexpr Vector operator/ (const T& rhs) { Vector temp = *this; return temp /= rhs; }

				T* begin()				{ return &x; }
				T* end()				{ return &y + 1; }
		const	T* begin()		const	{ return &x; }
		const	T* end()		const	{ return &y + 1; }
		const	T* cbegin()		const	{ return &x; }
		const	T* cend()		const	{ return &y + 1; }
				T* rbegin()				{ return &x - 1; }
				T* rend()				{ return &y; }
		const	T* rbegin()		const	{ return &x - 1; }
		const	T* rend()		const	{ return &y; }
		const	T* crbegin()	const	{ return &x - 1; }
		const	T* crend()		const	{ return &y; }

		static constexpr size_t size() { return 2; }

		constexpr T LengthSq() const { return x * x + y * y; }
		constexpr T Length() const { return sqrt(LengthSq()); }

		constexpr Vector& Normalize() { T len = Length(); x /= len; y /= len; return *this; }
		constexpr Vector Normal() const { Vector temp = *this; return temp.Normal(); }

		union
		{
			struct
			{
				T x; 
				T y;
			};
			struct
			{
				T width;
				T height;
			};
			struct
			{
				T u;
				T v;
			};
		};
	};

		template<typename T>
	struct Vector<3, T>
	{
		constexpr Vector(const T& value = 0) : x(value), y(value), z(value) {}
		constexpr Vector(const T& x, const T& y, const T& z) : x(x), y(y), z(z) {}

		template<typename T2>
		constexpr explicit operator Vector<2, T2>() const { return { static_cast<T2>(x), static_cast<T2>(y), static_cast<T2>(z) }; }

		constexpr bool operator== (const Vector& rhs) { return x == rhs.x && y == rhs.y && z == rhs.z; }
		constexpr bool operator!= (const Vector& rhs) { return x != rhs.x || y != rhs.y || z != rhs.z; }

		constexpr Vector& operator+= (const Vector& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; };
		constexpr Vector& operator-= (const Vector& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; };
		constexpr Vector& operator*= (const Vector& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; };
		constexpr Vector& operator/= (const Vector& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; };

		constexpr Vector operator+ (const Vector& rhs) { Vector temp = *this; return temp += rhs; }
		constexpr Vector operator- (const Vector& rhs) { Vector temp = *this; return temp -= rhs; }
		constexpr Vector operator* (const Vector& rhs) { Vector temp = *this; return temp *= rhs; }
		constexpr Vector operator/ (const Vector& rhs) { Vector temp = *this; return temp /= rhs; }

		constexpr Vector& operator+= (const T& rhs) { x += rhs; y += rhs; z += rhs; return *this; };
		constexpr Vector& operator-= (const T& rhs) { x -= rhs; y -= rhs; z -= rhs; return *this; };
		constexpr Vector& operator*= (const T& rhs) { x *= rhs; y *= rhs; z *= rhs; return *this; };
		constexpr Vector& operator/= (const T& rhs) { x /= rhs; y /= rhs; z /= rhs; return *this; };

		constexpr Vector operator+ (const T& rhs) { Vector temp = *this; return temp += rhs; }
		constexpr Vector operator- (const T& rhs) { Vector temp = *this; return temp -= rhs; }
		constexpr Vector operator* (const T& rhs) { Vector temp = *this; return temp *= rhs; }
		constexpr Vector operator/ (const T& rhs) { Vector temp = *this; return temp /= rhs; }

				T* begin()				{ return &x; }
				T* end()				{ return &z + 1; }
		const	T* begin()		const	{ return &x; }
		const	T* end()		const	{ return &z + 1; }
		const	T* cbegin()		const	{ return &x; }
		const	T* cend()		const	{ return &z + 1; }
				T* rbegin()				{ return &x - 1; }
				T* rend()				{ return &z; }
		const	T* rbegin()		const	{ return &x - 1; }
		const	T* rend()		const	{ return &z; }
		const	T* crbegin()	const	{ return &x - 1; }
		const	T* crend()		const	{ return &z; }

		static constexpr size_t size() { return 3; }

		constexpr T LengthSq() const { return x * x + y * y + z * z; }
		constexpr T Length() const { return sqrt(LengthSq()); }

		constexpr Vector& Normalize() { T len = Length(); x /= len; y /= len; z /= len; return *this; }
		constexpr Vector Normal() const { Vector temp = *this; return temp.Normal(); }

		union
		{
			struct
			{
				T x; 
				T y;
				T z;
			};
			struct
			{
				T u;
				T v;
				T w;
			};
		};
	};

	namespace detail
	{
		template<size_t D, typename T>
		static constexpr void meta_dot(T& d, const T* a, const T* b)
		{
			d += *a * *b;
			if constexpr (D > 1)
				meta_dot<D - 1>(d, a + 1, b + 1);
		}
	}

	template<size_t S, typename T>
	static constexpr T dot(const Vector<S, T>& a, const Vector<S, T>& b) 
	{
		T d = 0;
		detail::meta_dot<S, T>(d, a.array.data(), b.array.data());
		return d;
	}

	template<typename T>
	static constexpr T dot(const Vector<2, T>& a, const Vector<2, T>& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	template<typename T>
	static constexpr T dot(const Vector<3, T>& a, const Vector<3, T>& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	typedef Vector<2, float>	Vector2;
	typedef Vector<3, float>	Vector3;
	typedef Vector<2, int>		Point;
	typedef Vector<2, uint>		Size;
}