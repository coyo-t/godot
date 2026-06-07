#include "gd_chunk_manager.h"

ChunkManager::ChunkManager()
{
}

ChunkManager::~ChunkManager()
{
	world.destroyChunks();
}

auto ChunkManager::_update_tha_pev_read(I32 i, const Vector3 & p) -> I32
{
	pevChunkRead = i;
	return i;
}

auto ChunkManager::resize(I32 size) -> U0
{
	ERR_FAIL_COND(size < 0);
	pevChunkRead = -1;
	chunkHasBeenPlaced.resize(size);
	if (size == 0)
	{
		world.destroyChunks();
		return;
	}
	chunkHasBeenPlaced.clear();
	if (world.allocatedChunkCount > 0)
	{
		world.destroyChunks();
	}

	world.createChunks(size);
}

auto ChunkManager::handle_get_block(const Vector2i & handle) const -> I32
{
	return handle.y;
}

auto ChunkManager::handle_get_chunk(const Vector2i & handle) const -> I32
{
	return handle.x;
}

auto ChunkManager::get_block(const Vector2i & handle) const -> FunnyBlock::BlockInstance*
{
	auto chunk = handle_get_chunk(handle);
	auto block = handle_get_block(handle);
	ERR_FAIL_INDEX_V(chunk, world.chunkCount, nullptr);
	ERR_FAIL_INDEX_V(block, FunnyBlock::CHUNK_INNER_COUNT, nullptr);

	return world.getChunkUnsafe(chunk)->getBlockUnsafe(block);
}

auto ChunkManager::get_chunk_count() const -> I32
{
	return world.chunkCount;
}

auto ChunkManager::set_chunk_count(I32 to) -> U0
{
	if (to < 0)
	{
		world.chunkCount = world.allocatedChunkCount;
		return;
	}
	ERR_FAIL_COND(to > world.allocatedChunkCount);
	world.chunkCount = to;
}

auto ChunkManager::encode_chunk_point(const Vector3i & p, I32 mode) const -> I64
{
	return world.encodeChunkPoint(p, mode);
}

auto ChunkManager::decode_chunk_point(I64 p) const -> Vector3i
{
	return world.decodeChunkHandle(p);
}

auto ChunkManager::get_chunk_by_global_location(const Vector3i & location) -> I32
{
	auto gb = world.makeGlobalChunkLocation(location);
	
	if (pevChunkRead >= 0)
	{
		if (gb == pevChunkReadAt)
		{
			return pevChunkRead;
		}
	}
	if (chunksByLocation.has(gb))
	{
		auto outs = chunksByLocation[gb];
		return _update_tha_pev_read(outs, gb);
	}
	return _update_tha_pev_read(-1, gb);
}

auto ChunkManager::clear_chunk_global_hashmap() -> U0
{
	chunksByLocation.clear();
	chunkHasBeenPlaced.clear();
}

auto ChunkManager::chunk_get_global_location(I32 handle) const -> Vector3i
{
	ERR_FAIL_INDEX_V(handle, world.chunkCount, Vector3i());
	auto* chunk = world.getChunkUnsafe(handle);
	return chunk->globalLocation;
}

auto ChunkManager::chunk_set_global_location(I32 handle, const Vector3i& p) -> Vector3i
{
	ERR_FAIL_INDEX_V(handle, world.chunkCount, Vector3i());
	auto* chunk = world.getChunkUnsafe(handle);
	auto pev = chunk->globalLocation;
	auto gb = world.makeGlobalChunkLocation(p);
	chunk->globalLocation = gb;
	if (chunkHasBeenPlaced.isBitFalse(chunk->handle))
	{
		chunkHasBeenPlaced.setBitTrue(chunk->handle);
		chunksByLocation[gb] = chunk->handle;
	}
	else
	{
		if (chunksByLocation.has(gb))
		{
			chunksByLocation.erase(gb);
		}
		chunksByLocation[gb] = chunk->handle;
	}
	
	return pev;
}

auto ChunkManager::block_get_template_handle (const Vector2i& handle) const -> I64
{
	auto* b = get_block(handle);
	ERR_FAIL_NULL_V(b, 0);
	return b->templateHandle;
}

auto ChunkManager::block_set_template_handle(const Vector2i & handle, I64 to) const -> I64
{
	auto* b = get_block(handle);
	ERR_FAIL_NULL_V(b, 0);
	auto pev = b->templateHandle;
	b->templateHandle = to;
	return pev;
}

auto ChunkManager::globalize_chunk_location(const Vector3i & p) const -> Vector3i
{
	return world.makeGlobalChunkLocation(p);
}

auto ChunkManager::get_chunk_size() -> Vector3i
{
	constexpr auto cal = Vector3i(FunnyBlock::CHUNK_XSIZE, FunnyBlock::CHUNK_YSIZE, FunnyBlock::CHUNK_ZSIZE);
	return cal;
}


auto ChunkManager::_bind_methods() -> U0
{
	ClassDB::bind_method(
		D_METHOD("encode_chunk_point", "p"),
		&ChunkManager::encode_chunk_point
	);
	ClassDB::bind_method(
		D_METHOD("decode_chunk_point", "i"),
		&ChunkManager::decode_chunk_point
	);
	ClassDB::bind_method(
		D_METHOD("block_get_template_handle", "handle"),
		&ChunkManager::block_get_template_handle
	);
	ClassDB::bind_method(
		D_METHOD("block_set_template_handle", "handle", "template"),
		&ChunkManager::block_set_template_handle
	);
	ClassDB::bind_method(
		D_METHOD("chunk_get_global_location", "i"),
		&ChunkManager::chunk_get_global_location
	);
	ClassDB::bind_method(
		D_METHOD("chunk_set_global_location", "i", "p"),
		&ChunkManager::chunk_set_global_location
	);

	ClassDB::bind_method(
		D_METHOD("resize", "chunk_count"),
		&ChunkManager::resize
	);

	ClassDB::bind_method(
		D_METHOD("handle_get_block", "handle"),
		&ChunkManager::handle_get_block
	);
	ClassDB::bind_method(
		D_METHOD("handle_get_chunk", "handle"),
		&ChunkManager::handle_get_chunk
	);
	ClassDB::bind_method(
		D_METHOD("get_chunk_count"),
		&ChunkManager::get_chunk_count
	);
	ClassDB::bind_method(
		D_METHOD("set_chunk_count", "count"),
		&ChunkManager::set_chunk_count
	);
	ClassDB::bind_method(
		D_METHOD("globalize_chunk_location", "p"),
		&ChunkManager::globalize_chunk_location
	);
	ClassDB::bind_method(
		D_METHOD("get_chunk_by_global_location", "global_point"),
		&ChunkManager::get_chunk_by_global_location
	);
	ClassDB::bind_static_method(
		"ChunkManager",
		D_METHOD("get_chunk_size"),
		&ChunkManager::get_chunk_size
	);
}
