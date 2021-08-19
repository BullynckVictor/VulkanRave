#pragma once
#include <list>

namespace rv
{
	class RawMemoryChain
	{
	public:
		RawMemoryChain(size_t blocksize = 1024);
		~RawMemoryChain();
		void* Allocate(size_t size);
		void Free(void* object, size_t size);

		void SetBlockSize(size_t size);

	private:
		struct Range
		{
			void* begin = nullptr;
			void* end = nullptr;
			size_t size() const;
		};

		struct BlockHeader
		{
			void* next = nullptr;
			void* prev = nullptr;
			size_t blocksize = 0;
			size_t filled = 0;
		};

		static BlockHeader* header_cast(void* ptr);
		static const BlockHeader* header_cast(const void* ptr);

		void* first;
		void* last;
		size_t blocksize;
		std::list<Range> freeRanges;
	};

	template<typename T>
	class MemoryChain
	{

	};
}