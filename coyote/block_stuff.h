#pragma once

#include<core/templates/span.h>
#include<core/math/vector3i.h>

#include"coyote.h"

using namespace Coyote;

namespace FunnyBlock {
	class World;
	class Chunk;
	struct BlockInstance;
	
	constexpr U64 CHUNK_PO2 = 4;
	constexpr U64 CHUNK_SIZE = 1 << CHUNK_PO2;
	constexpr U64 CHUNK_MASK = ((1<<(CHUNK_PO2-1))-1) | (1<<(CHUNK_PO2-1));
	constexpr U64 CHUNK_INNER_COUNT = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
	
	constexpr U64 CHUNK_XSIZE = CHUNK_SIZE;
	constexpr U64 CHUNK_YSIZE = CHUNK_SIZE;
	constexpr U64 CHUNK_ZSIZE = CHUNK_SIZE;

	constexpr U64 CHUNK_XSHIFT = 0 * CHUNK_PO2;
	constexpr U64 CHUNK_YSHIFT = 1 * CHUNK_PO2;
	constexpr U64 CHUNK_ZSHIFT = 2 * CHUNK_PO2;
	
	constexpr U64 CHUNK_XBITS = CHUNK_MASK;
	constexpr U64 CHUNK_YBITS = CHUNK_MASK;
	constexpr U64 CHUNK_ZBITS = CHUNK_MASK;
	
	constexpr U64 CHUNK_XMASK = CHUNK_XBITS << CHUNK_XSHIFT;
	constexpr U64 CHUNK_YMASK = CHUNK_YBITS << CHUNK_YSHIFT;
	constexpr U64 CHUNK_ZMASK = CHUNK_ZBITS << CHUNK_ZSHIFT;
	

	// struct OrderArray {
	// 	Vector3i order[CHUNK_INNER_COUNT] = {};
	// 	constexpr OrderArray () : order() {
	// 		for (auto zz = 0; zz < CHUNK_ZSIZE; zz++) {
	// 			for (auto yy = 0; yy < CHUNK_YSIZE; yy++) {
	// 				for (auto xx = 0; xx < CHUNK_XSIZE; xx++) {
	// 					auto i = (zz * CHUNK_YSIZE + yy) * CHUNK_XSIZE + xx;
	// 					order[i] = { xx, yy, zz };
	// 				}
	// 			}
	// 		}
	// 	};
	// };


	class World {
		public:
		U64 chunkCount = 0;
		U64 allocatedChunkCount = 0;
		U0* thaAllocation = nullptr;

		public:
		auto encodeLocalChunkPointRaw (I64 x, I64 y, I64 z) const -> I64 {
			return (
				((x & CHUNK_XBITS) << CHUNK_XSHIFT) |
				((y & CHUNK_YBITS) << CHUNK_YSHIFT) |
				((z & CHUNK_ZBITS) << CHUNK_ZSHIFT)
			);
		};

		auto pointIsLocallyInChunk (I64 x, I64 y, I64 z) const -> Boolean {
			return ((x&CHUNK_XBITS)==x)&&((y&CHUNK_YBITS)==y)&&((z&CHUNK_ZBITS)==z);
		};
		
		auto getChunks () const -> Chunk* {
			return static_cast<Chunk*>(thaAllocation);
		}

		// wrapped = 0b00: returns -1 if the point is not in local chunk space
		// wrapped = 0b10: no safety, returns whatever the raw encode returns
		//
		// wrapped = 0b01: returns the localized index
		// wrapped = 0b11: same as 0b01, but will return `-index-1` if it was outside
		auto encodeChunkPoint (const Vector3i& p, int wrapped) const -> I64;

		auto createChunks (I32 chCount);
		auto destroyChunks ();
	};
	
	class Chunk {
		public:
		BlockInstance* blocks = nullptr;
		// these are locations in real space, and not the chunk space
		// IE chunk in cel (2, 1, 4) would have a location of (32, 16, 64)
		// assuming theyre 16x anyway :P
		Vector3i globalLocation;
		I32 padding;

		// this shouldnt change even if chunks are reorganized IE via sorting
		I32 handle = 0;
		I32 worldHandle = 0;

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

