#pragma once

#include<cstdint>
#include<core/templates/span.h>

namespace FunnyBlock {

	class BlockTemplate {

	};


	class World {
		Span<Chunk> chunks;
	};
	
	class Chunk {
		int32_t stableIndex;
		World& parent;
		
		Span<Block> blocks;
		
	};

	class Block {
		int32_t templateIndex;
		int32_t stableIndex;
		Chunk& parent;

	};
};

