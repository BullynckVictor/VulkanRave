#include "Utilities/File.h"
#include "Utilities/Exception.h"
#include <filesystem>
#include <fstream>

bool rv::FileExists(const char* filename)
{
	return std::filesystem::exists(filename);
}

std::uintmax_t rv::FileSize(const char* filename)
{
	return std::filesystem::file_size(filename);
}

void rv::ReadFileBuffer(const char* filename, void* buffer, const size_t size)
{
	rv_assert_file(filename);
	std::ifstream file(filename, std::ios::binary);
	rv_assert_info(file.is_open(), "Unable to open file");
	file.read(reinterpret_cast<char*>(buffer), size);
}
