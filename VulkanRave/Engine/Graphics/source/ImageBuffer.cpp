#include "Engine/Graphics/ImageBuffer.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define __STDC_LIB_EXT1__
#include "Engine/Libraries/stbi/stbi_image_write.h"
#undef __STDC_LIB_EXT1__
#include "Engine/Libraries/stbi/stbi_image.h"
#include "Engine/Utilities/Exception.h"
#include "Engine/Utilities/String.h"
#include <algorithm>
#include <filesystem>

int rv::ImageBuffer::jpegQuality = 85;

rv::ImageBuffer::ImageBuffer(const Size& size)
	:
	size(size),
	pixels(new Color[GetLength()])
{
}

rv::ImageBuffer::ImageBuffer(const Size& size, const Color& color)
	:
	size(size),
	pixels(new Color[GetLength()])
{
}

rv::ImageBuffer::ImageBuffer(const char* path)
{
	int channels = 0;
	rv_assert_file(path);
	pixels = reinterpret_cast<Color*>(rv_assert_not_null(stbi_load(path, reinterpret_cast<int*>(&size.x), reinterpret_cast<int*>(&size.y), &channels, STBI_rgb_alpha)));
}

rv::ImageBuffer::ImageBuffer(const std::string& path)
{
	int channels = 0;
	rv_assert_file(path.c_str());
	pixels = reinterpret_cast<Color*>(rv_assert_not_null(stbi_load(path.c_str(), reinterpret_cast<int*>(&size.x), reinterpret_cast<int*>(&size.y), &channels, STBI_rgb_alpha)));
}

rv::ImageBuffer::ImageBuffer(const ImageBuffer& rhs)
	:
	size(rhs.size)
{
	std::copy(rhs.begin(), rhs.end(), begin());
}

rv::ImageBuffer::ImageBuffer(ImageBuffer&& rhs) noexcept
	:
	size(rhs.size),
	pixels(rhs.pixels)
{
	rhs.size = Size(0);
	rhs.pixels = nullptr;
}

rv::ImageBuffer::~ImageBuffer()
{
	Release();
}

rv::ImageBuffer& rv::ImageBuffer::operator=(const ImageBuffer& rhs)
{
	Release();
	size = rhs.size;
	pixels = new Color[GetLength()];
	std::copy(rhs.begin(), rhs.end(), begin());

	return *this;
}

rv::ImageBuffer& rv::ImageBuffer::operator=(ImageBuffer&& rhs) noexcept
{
	Release();
	size = rhs.size;
	pixels = rhs.pixels;
	rhs.size = Size(0);
	rhs.pixels = nullptr;

	return *this;
}

void rv::ImageBuffer::Release()
{
	if (pixels)
		delete[] pixels;
	size = Size(0);
	pixels = nullptr;
}

void rv::ImageBuffer::Load(const char* path)
{
	Release();
	int channels = 0;
	rv_assert_file(path);
	pixels = reinterpret_cast<Color*>(rv_assert_not_null(stbi_load(path, reinterpret_cast<int*>(&size.x), reinterpret_cast<int*>(&size.y), &channels, STBI_rgb_alpha)));
}

void rv::ImageBuffer::Load(const std::string& path)
{
	Load(path.c_str());
}

namespace fs = std::filesystem;

void rv::ImageBuffer::Write(const char* path)
{
	std::string extension = fs::path(path).extension().string();
	for (char& c : extension)
		c = (char)tolower((int)c);

	if (extension == ".png")
		rv_assert_not_null(stbi_write_png(path, size.x, size.y, 4, pixels, size.x * sizeof(Color)));

	else if (extension == ".jpg" || extension == ".jpeg" || extension == ".jfif" || extension == ".pjpeg" || extension == ".pjp")
		rv_assert_not_null(stbi_write_jpg(path, size.x, size.y, 4, pixels, jpegQuality));

	else if (extension == ".bmp" || extension == ".dib")
		rv_assert_not_null(stbi_write_bmp(path, size.x, size.y, 4, pixels));

	else if (extension == ".tga")
		rv_assert_not_null(stbi_write_tga(path, size.x, size.y, 4, pixels));

	else if (extension == ".hdr")
	{
		std::vector<FColor> hdr(GetLength());
		std::transform(begin(), end(), hdr.begin(), [](const Color& in) { return (FColor)in; });
		rv_assert_not_null(stbi_write_hdr(path, size.x, size.y, 4, reinterpret_cast<float*>(hdr.data())));
	}

	else
		rv_throw(str("Unable to write image with extension \"", extension, "\" (", path, ")"));
}

void rv::ImageBuffer::Write(const std::string& path)
{
}

rv::Color* rv::ImageBuffer::begin()
{
	return pixels;
}

const rv::Color* rv::ImageBuffer::begin() const
{
	return pixels;
}

const rv::Color* rv::ImageBuffer::cbegin() const
{
	return pixels;
}

rv::Color* rv::ImageBuffer::rbegin()
{
	return pixels + GetLength() - 1;
}

const rv::Color* rv::ImageBuffer::crbegin() const
{
	return pixels + GetLength() - 1;
}

rv::Color* rv::ImageBuffer::end()
{
	return pixels + GetLength();
}

const rv::Color* rv::ImageBuffer::end() const
{
	return pixels + GetLength();
}

const rv::Color* rv::ImageBuffer::cend() const
{
	return pixels + GetLength();
}

rv::Color* rv::ImageBuffer::rend()
{
	return pixels - 1;
}

const rv::Color* rv::ImageBuffer::crend() const
{
	return pixels - 1;
}

void rv::ImageBuffer::Fill(const Color& color)
{
	std::fill(begin(), end(), color);
}

rv::Color* rv::ImageBuffer::data()
{
	return pixels;
}

const rv::Color* rv::ImageBuffer::data() const
{
	return pixels;
}

bool rv::ImageBuffer::empty() const
{
	return pixels;
}
