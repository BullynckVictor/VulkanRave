#include "Engine/General/MemoryChain.h"
#include "Engine/Utilities/Types.h"

rv::u8* byte_cast(void* ptr) { return reinterpret_cast<rv::u8*>(ptr); }
const rv::u8* byte_cast(const void* ptr) { return reinterpret_cast<const rv::u8*>(ptr); }

rv::RawMemoryChain::RawMemoryChain(size_t blocksize)
	:
	first(nullptr),
	last(nullptr),
	blocksize(blocksize)
{
}

rv::RawMemoryChain::~RawMemoryChain()
{
	void* next = nullptr;
	for (void* block = first; block; block = next)
	{
		next = header_cast(block)->next;
		free(block);
	}
}

void* rv::RawMemoryChain::Allocate(size_t size)
{
	BlockHeader fresh{};

	if (!first) // there are no blocks allocated yet
	{
		first = malloc(blocksize + sizeof(BlockHeader));
		BlockHeader& header = *header_cast(first);
		memcpy(&header, &fresh, sizeof(BlockHeader));
		header.blocksize = blocksize;
		last = first;
	}
	else if (!freeRanges.empty()) // there are free allocated ranges
	{
		std::list<Range>::iterator smallest_match = freeRanges.end();
		size_t smallest = std::numeric_limits<size_t>::max();
		for (auto it = freeRanges.begin(); it != freeRanges.end(); ++it) // loop through all ranges
		{
			size_t range_size = it->size();
			if (range_size == size) // matches perfectly
			{
				smallest_match = it;
				break;
			}
			if (range_size > size && range_size < smallest) // smallest match up until now
			{
				smallest_match = it;
				smallest = range_size;
			}
		}
		if (smallest_match != freeRanges.end()) // we have a match
		{
			void* ret = smallest_match->begin;
			smallest_match->begin = byte_cast(smallest_match->begin) + size;
			if (!smallest_match->size()) // there are no bytes left in the range
				freeRanges.erase(smallest_match); // delete range
			return ret;
		}
	}

	BlockHeader* header = nullptr;
	for (void* block = first; block; block = header->next)
	{
		header = header_cast(block);
		u8* data = byte_cast(block) + sizeof(BlockHeader);
		if (header->filled + size <= header->blocksize) // there is space in this block
		{
			void* ret = data + header->filled;
			header->filled += size;
			return ret;
		}
	}

	// no space left
	if (size <= blocksize)
	{
		void* next = malloc(blocksize);
		header_cast(last)->next = next;
		BlockHeader& header = *header_cast(next);
		memcpy(&header, &fresh, sizeof(BlockHeader));
		header.blocksize = blocksize;
		header.filled = size;
		header.prev = last;
		last = next;
	}
	else
	{
		void* next = malloc(size);
		header_cast(last)->next = next;
		BlockHeader& header = *header_cast(next);
		memcpy(&header, &fresh, sizeof(BlockHeader));
		header.blocksize = size;
		header.filled = size;
		header.prev = last;
		last = next;
	}
	return byte_cast(last) + sizeof(BlockHeader);
}

void rv::RawMemoryChain::Free(void* object, size_t size)
{
}

void rv::RawMemoryChain::SetBlockSize(size_t size)
{
	blocksize = size;
}

rv::RawMemoryChain::BlockHeader* rv::RawMemoryChain::header_cast(void* ptr)
{
	return reinterpret_cast<BlockHeader*>(ptr);
}

const rv::RawMemoryChain::BlockHeader* rv::RawMemoryChain::header_cast(const void* ptr)
{
	return reinterpret_cast<const BlockHeader*>(ptr);
}


size_t rv::RawMemoryChain::Range::size() const
{
	return byte_cast(end) - byte_cast(begin);
}
