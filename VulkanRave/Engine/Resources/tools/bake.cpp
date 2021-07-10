#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <cstring>

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "argc must be 2\n";
		return EXIT_FAILURE;
	}

	std::ifstream input(argv[1], std::ios::binary);
	if (!input.is_open())
	{
		std::cerr << "unable to open file\"" << argv[1] << "\"\n";
		return EXIT_FAILURE;
	}

	size_t size = 0;
	input.seekg(0, std::ios::end);
	size = input.tellg();
	input.seekg(0, std::ios::beg);

	std::vector<char> data(size);
	input.read(data.data(), size);
	
	std::string stem = std::filesystem::path(argv[1]).stem().string();
	std::string extension = std::filesystem::path(argv[1]).extension().string();
	extension.erase(extension.begin());

	std::string var = stem;
	for (char& c : var)
		if (c == '.')
			c = '_';
	var += "_" + extension;

	std::ofstream header("../" + stem + "." + extension + ".h");
	header << "#pragma once\n";
	header << "#include \"Engine/Resources/BakedResource.h\"\n\n";
	header << "namespace rv\n";
	header << "{\n";
	header << "\tnamespace Resources\n";
	header << "\t{\n";
	header << "\t\tconst extern BakedResource " << var << ";\n";
	header << "\t}\n";
	header << "}\n";

	
	std::ofstream source("../source/" + stem + "." + extension + ".cpp");
	source << "#include \"Engine/Resources/" << stem + "." + extension + ".h" << "\"\n\n";
	source << "const char data[] = { ";
	for (size_t i = 0; i < data.size(); ++i)
		source << (i == 0 ? "" : ", ") << (data[i] < 0 ? "(char)" : "") << "0x" << std::hex << (int)data[i];
	source << " };\n";

	source << "const rv::BakedResource rv::Resources::" << var << " = { " << std::dec << size << ", data };\n";

	std::cout << "Baked rv::Resources::" << var << " [\"" << stem << "." << extension << ".h\", \"source/" << stem << "." << extension << ".cpp\"]\n";
}