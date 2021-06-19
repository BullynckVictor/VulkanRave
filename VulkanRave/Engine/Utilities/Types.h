#pragma once
#include <stdint.h>

namespace rv
{
	typedef unsigned int	uint;
	typedef unsigned char	uchar;

	typedef int8_t			int8;
	typedef int16_t			int16;
	typedef int32_t			int32;
	typedef int64_t			int64;

	typedef uint8_t			uint8;
	typedef uint16_t		uint16;
	typedef uint32_t		uint32;
	typedef uint64_t		uint64;

	typedef int8			i8;
	typedef int16			i16;
	typedef int32			i32;
	typedef int64			i64;

	typedef uint8			u8;
	typedef uint16			u16;
	typedef uint32			u32;
	typedef uint64			u64;
}

#ifdef RV_EXPOSE_TYPES

typedef unsigned int	uint;
typedef unsigned char	uchar;

typedef rv::int8	int8;
typedef rv::int16	int16;
typedef rv::int32	int32;
typedef rv::int64	int64;

typedef rv::uint8	uint8;
typedef rv::uint16	uint16;
typedef rv::uint32	uint32;
typedef rv::uint64	uint64;

typedef rv::i8		i8;
typedef rv::i16		i16;
typedef rv::i32		i32;
typedef rv::i64		i64;

typedef rv::u8		u8;
typedef rv::u16		u16;
typedef rv::u32		u32;
typedef rv::u64		u64;

#endif