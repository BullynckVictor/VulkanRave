#pragma once

#ifdef NDEBUG
#	define RV_RELEASE
#else
#	define RV_DEBUG
#endif

#ifdef _WIN32
#	define RV_PLATFORM_WINDOWS 1
#endif
#ifdef __APPLE__
#	define RV_PLATFORM_APPLE 1
#endif
#ifdef __linux__
#	define RV_PLATFORM_LINUX 1
#endif
#ifdef __ANDROID__
#	define RV_PLATFORM_ANDROID 1
#endif

#ifdef _RV_TARGET_32
#	define RV_TARGET_32
#	define RV_TARGET_x86
#endif
#ifdef _RV_TARGET_64
#	define RV_TARGET_64
#	define RV_TARGET_x64
#endif

#if not defined RV_PLATFORM_WINDOWS
#	error Platform not supported by Rave Engine
#endif

namespace rv
{
	struct SystemInfo
	{
		bool windows	= false;
		bool apple		= false;
		bool linux		= false;
		bool android	= false;

		bool x86		= false;
		bool x64		= false;

		bool debug		= false;
		bool release	= false;
	};

	static constexpr SystemInfo sys = {
		#ifdef RV_PLATFORM_WINDOWS
			true,
		#else
			false,
		#endif
		#ifdef RV_PLATFORM_APPLE
			true,
		#else
			false,
		#endif
		#ifdef RV_PLATFORM_LINUX
			true,
		#else
			false,
		#endif
		#ifdef RV_PLATFORM_ANDROID
			true,
		#else
			false,
		#endif
		#ifdef RV_TARGET_x86
			true,
		#else
			false,
		#endif
		#ifdef RV_TARGET_x64
			true,
		#else
			false,
		#endif
		#ifdef RV_DEBUG
			true,
		#else
			false,
		#endif		
		#ifdef RV_RELEASE
			true
		#else
			false
		#endif

	};
}