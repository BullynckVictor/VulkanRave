#pragma once
#include <cstdint>

namespace rv
{
	bool FileExists(const char* filename);
	std::uintmax_t FileSize(const char* filename);
	void ReadFileBuffer(const char* filename, void* buffer, const size_t size);
}