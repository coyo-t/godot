#include "block_stuff.h"

#include<core/os/memory.h>

struct AllocationResult {
	U0* raw;
	FunnyBlock::Chunk* chunks;
	FunnyBlock::BlockInstance* blocks;
};

static auto allocSegment (I32 chunkCount, U0* pev) -> AllocationResult {
	using namespace FunnyBlock;
	if (chunkCount <= 0)
	{
		memrealloc(pev, 0);
		auto outs = AllocationResult {};
		outs.raw = nullptr;
		return outs;
	}
	
	constexpr U64 blockInstSegmentSize = sizeof(BlockInstance) * CHUNK_INNER_COUNT;
	// constexpr U64 chBlockSize = sizeof(Chunk) + blockInstSegmentSize;
	
	U64 cc = static_cast<U64>(chunkCount);
	U64 chunkSegmentSize = sizeof(Chunk) * cc;
	U64 segmentByteSize = chunkSegmentSize + blockInstSegmentSize * cc;
	U0* segment = memrealloc(pev, segmentByteSize);
	
	auto outs = AllocationResult {};
	outs.raw = segment;
	if (segment != nullptr)
	{
		outs.chunks = static_cast<Chunk*>(segment);
		outs.blocks = reinterpret_cast<BlockInstance*>(outs.chunks+cc);
	}
	return outs;
}

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
		p.x & static_cast<I32>(CHUNK_XBITS),
		p.y & static_cast<I32>(CHUNK_YBITS),
		p.z & static_cast<I32>(CHUNK_ZBITS),
	};
}

auto FunnyBlock::World::tgvRemoveSubChunkPart(const Vector3i & p) const -> Vector3i
{
	return Vector3i {
		p.x & ~static_cast<I32>(CHUNK_XBITS),
		p.y & ~static_cast<I32>(CHUNK_YBITS),
		p.z & ~static_cast<I32>(CHUNK_ZBITS),
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

auto FunnyBlock::World::tgResetChunkLocation(I32 chunk) -> Boolean
{
	if (0 > chunk || chunk >= chunkCount)
		return false;
	auto* ch = getChunkUnsafe(chunk);
	if (ch->hasBeenPlaced)
	{
		ch->hasBeenPlaced = false;
		return true;
	}
	return false;
}

auto FunnyBlock::World::tgChunkHasBeenPlaced(I32 chunk) const -> Boolean
{
	if (0 > chunk || chunk >= chunkCount)
		return false;
	return getChunkUnsafe(chunk)->hasBeenPlaced;
}

auto FunnyBlock::World::tgSetChunkLocation(I32 chunk, const Vector3i & p) -> Boolean
{
	if (0 > chunk || chunk >= chunkCount)
		return false;
	auto* ch = getChunkUnsafe(chunk);
	auto localized = tgvRemoveSubChunkPart(p);
	
	if (ch->hasBeenPlaced)
	{
		if (localized == ch->globalLocation)
		{
			return false;
		}
		if (hmChunksByLocation.has(ch->globalLocation))
		{
			hmChunksByLocation.erase(ch->globalLocation);
		}
	}

	if (hmChunksByLocation.has(localized))
	{
		auto* pev = getChunkUnsafe(hmChunksByLocation[localized]);
		pev->hasBeenPlaced = false;
	}
	ch->hasBeenPlaced = true;
	hmChunksByLocation[localized] = chunk;

	return true;
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

auto FunnyBlock::World::tgResize(I32 newChunkCount) -> U0
{
	hmPevReadBlock = -2;
	hmPevReadChunk = -2;
	hmChunksByLocation.clear();
	chunkCount = newChunkCount < 0 ? 0 : newChunkCount;
	allocatedChunkCount = chunkCount;
	auto res = allocSegment(newChunkCount, thaAllocation);
	thaAllocation = res.raw;
	if (res.raw == nullptr)
	{
		return;
	}
	for (auto i = 0; i < chunkCount; i++) {
		Chunk newch = {};
		newch.handle = i;
		newch.blocks = res.blocks + CHUNK_INNER_COUNT * i;
		res.chunks[i] = newch;
		for (auto j = 0; j < CHUNK_INNER_COUNT; j++) {
			BlockInstance newb = {};
			newb.handle = j;
			newb.ownerHandle = i;
			newb.templateHandle = defaultTemplateHandle;
			newch.blocks[j] = newb;
		}
	}
}

auto FunnyBlock::World::getBlockPointer(Handle2Block handle) const -> BlockInstance *
{
	auto ci = handle.chunkIndex;
	if (0 > ci || ci >= chunkCount)
	{
		return nullptr;
	}
	auto bi = handle.blockIndex;
	if (0 > bi || bi >= CHUNK_INNER_COUNT)
	{
		return nullptr;
	}
	return getChunkUnsafe(ci)->getBlockUnsafe(bi);
}


auto FunnyBlock::World::getChunkUnsafe(I64 i) const -> Chunk *
{
	return static_cast<Chunk*>(thaAllocation) + i;
}

auto FunnyBlock::Chunk::getBlockUnsafe(I64 i) const -> BlockInstance *
{
	return blocks + i;
}


