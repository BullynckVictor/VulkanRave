#pragma once
#include "Engine/Utilities/Color.h"
#include "Engine/Utilities/Vector.h"
#include <string>

namespace rv
{
	class ImageBuffer
	{
	public:
		ImageBuffer() = default;
		ImageBuffer(const Size& size);
		ImageBuffer(const Size& size, const Color& color);
		ImageBuffer(const char* path);
		ImageBuffer(const std::string& path);
		ImageBuffer(const ImageBuffer& rhs);
		ImageBuffer(ImageBuffer&& rhs) noexcept;
		~ImageBuffer();

		ImageBuffer& operator= (const ImageBuffer& rhs);
		ImageBuffer& operator= (ImageBuffer&& rhs) noexcept;

		void Release();

		void Load(const char* path);
		void Load(const std::string& path);
		void Write(const char* path);
		void Write(const std::string& path);

				Color*	begin();
		const	Color*	begin()		const;
		const	Color*	cbegin()	const;
				Color*	rbegin();
		const	Color*	crbegin()	const;
				Color*	end();
		const	Color*	end()		const;
		const	Color*	cend()		const;
				Color*	rend();
		const	Color*	crend()		const;

		Size GetSize() const { return size; }
		u32 GetLength() const { return size.x * size.y; }

		void Fill(const Color& color);

		Color* data();
		const Color* data() const;

		bool empty() const;

	public:
		static int jpegQuality;

	private:
		Size size = Size(0);
		Color* pixels = nullptr;
	};
}