#include <iostream>
#include <cstdlib>
#include <sstream>
#include <filesystem>
#include <cassert>

namespace fs = std::filesystem;

int main()
{
	const char* vulkan = nullptr;
	vulkan = getenv("VULKAN_SDK");
	assert(vulkan);

	for (const auto& p : fs::recursive_directory_iterator("source"))
	{
		fs::path source = p.path();
		fs::path binary = fs::path("bin\\" + fs::relative(source, "source").generic_u8string() + ".spv");
		if (!p.is_directory())
		{
			std::ostringstream ss;
			ss << vulkan << "\\bin32\\glslc.exe " << source << " -o " << binary;
			std::cout << "Compiling " << fs::relative(source, "source") << std::endl;
			int val = system(ss.str().c_str());
			if (val != EXIT_SUCCESS)
				return val;
		}
		else
			fs::create_directory("bin\\" + fs::relative(source, "source").generic_u8string());
	}
}