#include "block_stuff.h"

#include<core/os/memory.h>

auto FunnyBlock::World::tgGetChunkInnerCount() const -> I64
{
	return CHUNK_INNER_COUNT;
}

auto FunnyBlock::World::tgGetChunkSize() const -> Vector3i
{
	return Vector3i { CHUNK_XSIZE, CHUNK_YSIZE, CHUNK_ZSIZE };
}


auto FunnyBlock::World::tgvRemoveChunkPart(const Vector3i & p) const -> Vector3i
{
	return Vector3i {
		p.x & CHUNK_XBITS,
		p.y & CHUNK_YBITS,
		p.z & CHUNK_ZBITS,
	};
}

auto FunnyBlock::World::tgvRemoveSubChunkPart(const Vector3i & p) const -> Vector3i
{
	return Vector3i {
		p.x & ~CHUNK_XBITS,
		p.y & ~CHUNK_YBITS,
		p.z & ~CHUNK_ZBITS,
	};
}

auto FunnyBlock::World::tgvChunkify(const Vector3i & p) const -> Vector3i
{
	return Vector3i {
		p.x >> CHUNK_XBITS,
		p.y >> CHUNK_YBITS,
		p.z >> CHUNK_ZBITS,
	};
}

auto FunnyBlock::World::tgvInChunkBounds(const Vector3i & p) const -> Boolean
{
	auto ux = static_cast<U64>(p.x);
	auto uy = static_cast<U64>(p.y);
	auto uz = static_cast<U64>(p.z);
	return (
		((ux&CHUNK_XBITS)==ux)&&
		((uy&CHUNK_YBITS)==uy)&&
		((uz&CHUNK_ZBITS)==uz)
	);
}

auto FunnyBlock::World::tgvPack(const Vector3i & p) const -> I64
{
	return (
		((static_cast<I64>(p.x) & CHUNK_XBITS) << CHUNK_XSHIFT) |
		((static_cast<I64>(p.y) & CHUNK_YBITS) << CHUNK_YSHIFT) |
		((static_cast<I64>(p.z) & CHUNK_ZBITS) << CHUNK_ZSHIFT)
	);
}

auto FunnyBlock::World::tgvPackStrict(const Vector3i & p) const -> I64
{
	return tgvInChunkBounds(p) ? tgvPack(p) : -1;
}

auto FunnyBlock::World::tgvPackLocal(const Vector3i & p) const -> I64
{
	return tgvPack(tgvRemoveChunkPart(p));
}

auto FunnyBlock::World::tgvPackLocalAdjacent(const Vector3i & p) const -> I64
{
	auto i = tgvPack(tgvRemoveChunkPart(p));
	return tgvInChunkBounds(p) ? i : -i - 1;
}

auto FunnyBlock::World::tgvUnpack(I64 v) const -> Vector3i
{
	return Vector3i {
		static_cast<I32>((v >> CHUNK_XSHIFT) & CHUNK_XMASK),
		static_cast<I32>((v >> CHUNK_YSHIFT) & CHUNK_YMASK),
		static_cast<I32>((v >> CHUNK_ZSHIFT) & CHUNK_ZMASK),
	};
}

auto FunnyBlock::World::tgGetChunkByLocation(const Vector3i & p) -> I32
{
	auto gl = tgvRemoveSubChunkPart(p);
	if (hmPevReadChunk != -2)
	{
		if (hmPevChunkLocation == gl)
		{
			return hmPevReadChunk;
		}
	}
	hmPevChunkLocation = gl;
	if (hmChunksByLocation.has(gl))
	{
		auto x = hmChunksByLocation[gl];
		hmPevReadChunk = x;
		return x;
	}
	hmPevReadChunk = -1;
	return -1;
}

auto FunnyBlock::World::tgGetBlockHandle(const Vector3i & p) -> Handle2Block
{
	if (hmPevReadBlock != -2)
	{
		if (hmPevBlockLocation == p)
			return hmPevBlockHandle;
	}
	hmPevBlockLocation = p;
	auto ci = tgGetChunkByLocation(p);
	if (ci < 0)
	{
		hmPevReadBlock = -1;
		return Handle2Block();
	}
	auto x = tgvPackLocal(p);
	hmPevReadBlock = x;
	return Handle2Block(ci, x);
}





auto FunnyBlock::World::getChunkUnsafe(I64 i) const -> Chunk *
{
	return static_cast<Chunk*>(thaAllocation) + i;
}

auto FunnyBlock::World::createChunks(I32 chCount) -> U0
{
	if (thaAllocation != nullptr)
	{
		return;
	}
	constexpr U64 blockInstSegmentSize = sizeof(BlockInstance) * CHUNK_INNER_COUNT;
	constexpr U64 chBlockSize = sizeof(Chunk) + blockInstSegmentSize;
	
	U64 cc = (chCount < 0) ? 0 : static_cast<U64>(chCount);
	U64 chunkSegmentSize = sizeof(Chunk) * cc;
	U64 segmentByteSize = chunkSegmentSize + blockInstSegmentSize * cc;
	auto* segment = memalloc(segmentByteSize);

	allocatedChunkCount = (chCount < 0) ? 0 : chCount;
	chunkCount = allocatedChunkCount;
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

auto FunnyBlock::World::destroyChunks() -> U0
{
	allocatedChunkCount = 0;
	chunkCount = 0;
	if (thaAllocation != nullptr)
	{
		memfree(thaAllocation);
		thaAllocation = nullptr;
	}
}

auto FunnyBlock::Chunk::getBlockUnsafe(I64 i) const -> BlockInstance *
{
	return blocks + i;
}


