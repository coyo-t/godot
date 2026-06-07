#pragma once

#include<core/templates/span.h>
#include<core/math/vector3i.h>
#include<core/templates/hash_map.h>
#include<core/templates/rid.h>

#include"coyote.h"

using namespace Coyote;

namespace FunnyBlock {
	class World;
	class Chunk;
	struct BlockInstance;
	
	constexpr I64 CHUNK_PO2 = 4;
	constexpr I64 CHUNK_SIZE = 1 << CHUNK_PO2;
	constexpr I64 CHUNK_MASK = ((1<<(CHUNK_PO2-1))-1) | (1<<(CHUNK_PO2-1));
	constexpr I64 CHUNK_INNER_COUNT = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
	
	constexpr I64 CHUNK_XSIZE = CHUNK_SIZE;
	constexpr I64 CHUNK_YSIZE = CHUNK_SIZE;
	constexpr I64 CHUNK_ZSIZE = CHUNK_SIZE;

	constexpr I64 CHUNK_XSHIFT = 0 * CHUNK_PO2;
	constexpr I64 CHUNK_YSHIFT = 1 * CHUNK_PO2;
	constexpr I64 CHUNK_ZSHIFT = 2 * CHUNK_PO2;
	
	constexpr I64 CHUNK_XBITS = CHUNK_MASK;
	constexpr I64 CHUNK_YBITS = CHUNK_MASK;
	constexpr I64 CHUNK_ZBITS = CHUNK_MASK;
	
	constexpr I64 CHUNK_XMASK = CHUNK_XBITS << CHUNK_XSHIFT;
	constexpr I64 CHUNK_YMASK = CHUNK_YBITS << CHUNK_YSHIFT;
	constexpr I64 CHUNK_ZMASK = CHUNK_ZBITS << CHUNK_ZSHIFT;

	union Handle2Block {
		I64 asLong;
		struct {
			I32 chunkIndex;
			I32 blockIndex;
		};
		Handle2Block(): chunkIndex(-1), blockIndex(0) {
		}
		Handle2Block(I64 from): asLong(from) {
		}
		Handle2Block(I32 ci, I32 bi): chunkIndex(ci), blockIndex(bi) {
		}
		Handle2Block(RID from): asLong(from.get_id()) {
		}

		auto toRID () const -> RID {
			return RID::from_uint64(static_cast<U64>(asLong));
		}

		auto isValid () const -> Boolean {
			return chunkIndex >= 0;
		}
	};
	static_assert(sizeof(Handle2Block) == sizeof(I64));

	class World {
		public:
		I32 chunkCount = 0;
		I32 allocatedChunkCount = 0;
		U0* thaAllocation = nullptr;
		I32 defaultTemplateHandle = 0;

		private:
		HashMap<Vector3i, I32> hmChunksByLocation = {};
		I32 hmPevReadChunk = -2;
		I32 hmPevReadBlock = -2;
		Handle2Block hmPevBlockHandle = {};
		Vector3i hmPevChunkLocation = {};
		Vector3i hmPevBlockLocation = {};

		public:
		auto tgGetChunkInnerCount () const -> I64;
		auto tgGetChunkSize () const -> Vector3i;

		auto tgvRemoveChunkPart (const Vector3i& p) const -> Vector3i;
		auto tgvRemoveSubChunkPart (const Vector3i& p) const -> Vector3i;
		auto tgvChunkify (const Vector3i& p) const -> Vector3i;
		auto tgvInChunkBounds (const Vector3i& p) const -> Boolean;
		auto tgvPack (const Vector3i& p) const -> I64;
		auto tgvPackStrict (const Vector3i& p) const -> I64;
		auto tgvPackLocal (const Vector3i& p) const -> I64;
		auto tgvPackLocalAdjacent (const Vector3i& p) const -> I64;
		auto tgvUnpack (I64 v) const -> Vector3i;
		
		auto tgResetChunkLocation (I32 chunk) -> Boolean;
		auto tgChunkHasBeenPlaced (I32 chunk) const -> Boolean;
		auto tgSetChunkLocation (I32 chunk, const Vector3i& p) -> Boolean;
		auto tgGetChunkByLocation (const Vector3i& p) -> I32;
		auto tgGetBlockHandle (const Vector3i& p) -> Handle2Block;

		auto tgResize (I32 newChunkCount) -> U0;

		auto getBlockPointer (Handle2Block handle) const -> BlockInstance*;
		auto getChunkUnsafe (I64 i) const -> Chunk*;
	};
	
	class Chunk {
		public:
		BlockInstance* blocks = nullptr;
		// these are locations in real space, and not the chunk space
		// IE chunk in cel (2, 1, 4) would have a location of (32, 16, 64)
		// assuming theyre 16x anyway :P
		Vector3i globalLocation;
		U8 hasBeenPlaced = false;
		U8 pad1;
		U16 pad2;

		// this shouldnt change even if chunks are reorganized IE via sorting
		I32 handle = 0;
		I32 worldHandle = 0;

		auto getBlockUnsafe (I64 i) const -> BlockInstance*;
	};

	struct BlockInstance {
		public:
		// my index inside the chunk
		I16 handle = 0;
		// the index of the chunk i belong to
		I16 ownerHandle = 0;
		//, the index in the array of registered bloc templates
		I32 templateHandle = 0;
		
		// Free space :D
		I32 reserved[2] = {};
	};
};

