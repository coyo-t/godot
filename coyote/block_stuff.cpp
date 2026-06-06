#include "block_stuff.h"

#include<core/os/memory.h>

auto FunnyBlock::World::encodeChunkPoint(const Vector3i & p, int wrapped) const -> I64
{
	if ((wrapped&0b01) != 0)
	{
		auto localx = p.x & CHUNK_XBITS;
		auto localy = p.y & CHUNK_YBITS;
		auto localz = p.z & CHUNK_ZBITS;
		auto encode = encodeLocalChunkPointRaw(localx, localy, localz);
		if ((wrapped&0b10) != 0)
		{
			if (localx != p.x || localy != p.y || localz != p.z)
			{
				return -encode - 1;
			}
		}
		return encode;
	}
	else
	{
		if ((wrapped&0b10) != 0)
		{
			if (pointIsLocallyInChunk(p.x, p.y, p.z))
			{
				return encodeLocalChunkPointRaw(p.x, p.y, p.z);
			}
			return -1;
		}
		return encodeLocalChunkPointRaw(p.x, p.y, p.z);
	}
}

auto FunnyBlock::World::createChunks(I32 chCount)
{
	if (thaAllocation != nullptr)
	{
		return;
	}
	constexpr U64 blockInstSegmentSize = sizeof(BlockInstance) * CHUNK_INNER_COUNT;
	constexpr U64 chBlockSize = sizeof(Chunk) + blockInstSegmentSize;

	U64 chunkSegmentSize = sizeof(Chunk) * chCount;
	U64 segmentByteSize = chunkSegmentSize + blockInstSegmentSize * chCount;
	auto* segment = memalloc(segmentByteSize);

	chunkCount = chCount;
	allocatedChunkCount = chCount;
	thaAllocation = segment;
	auto* chunks = static_cast<Chunk*>(segment);
	auto* blocks = reinterpret_cast<BlockInstance*>(chunks+chCount);

	for (auto i = 0; i < chunkCount; i++) {
		Chunk newch = {};
		newch.handle = i;
		newch.blocks = blocks + CHUNK_INNER_COUNT * i;
		chunks[i] = newch;
		for (auto j = 0; j < CHUNK_INNER_COUNT; j++) {
			BlockInstance newb = {};
			newb.handle = j;
			newb.ownerHandle = i;
			newb.templateHandle = 0;
			newch.blocks[j] = newb;
		}
	}
}

auto FunnyBlock::World::destroyChunks()
{
	if (thaAllocation == nullptr)
	{
		return;
	}
	memfree(thaAllocation);
	thaAllocation = nullptr;
}



