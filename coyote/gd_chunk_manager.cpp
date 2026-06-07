#include "gd_chunk_manager.h"

using namespace FunnyBlock;

ChunkManager::ChunkManager()
{
}

ChunkManager::~ChunkManager()
{
	world.tgResize(0);
}

auto ChunkManager::resize(I32 newCount) -> U0
{
	world.tgResize(newCount);
}
auto ChunkManager::get_chunk_count() -> I64
{
	return world.chunkCount;
}

auto ChunkManager::point_remove_subchunk_part(const Vector3i & p) const -> Vector3i
{
	return world.tgvRemoveSubChunkPart(p);
}
auto ChunkManager::point_remove_chunk_part(const Vector3i & p) const -> Vector3i
{
	return world.tgvRemoveChunkPart(p);
}
auto ChunkManager::point_to_chunk_cel(const Vector3i & p) const -> Vector3i
{
	return world.tgvChunkify(p);
}
auto ChunkManager::point_in_local_chunk_bounds(const Vector3i & p) const -> Boolean
{
	return world.tgvInChunkBounds(p);
}

auto ChunkManager::point_pack(const Vector3i & p) const -> I64
{
	return world.tgvPack(p);
}

auto ChunkManager::point_unpack(I64 v) const -> Vector3i
{
	return world.tgvUnpack(v);
}

auto ChunkManager::chunk_get_inner_count(I32 chunk) const -> I32
{
	ERR_FAIL_INDEX_V(chunk, world.chunkCount, 0);
	return CHUNK_INNER_COUNT;
}
auto ChunkManager::chunk_get_size(I32 chunk) const -> Vector3i
{
	ERR_FAIL_INDEX_V(chunk, world.chunkCount, Vector3i());
	return Vector3i { CHUNK_XSIZE, CHUNK_YSIZE, CHUNK_ZSIZE };
}
auto ChunkManager::chunk_has_point(I32 chunk, const Vector3i & p) const -> Boolean
{
	ERR_FAIL_INDEX_V(chunk, world.chunkCount, false);
	auto* ch = world.getChunkUnsafe(chunk);
	ERR_FAIL_COND_V(!ch->hasBeenPlaced, false);
	return world.tgvInChunkBounds(p - ch->globalLocation);
}
auto ChunkManager::chunk_get_location(I32 chunk) -> Vector3i
{
	ERR_FAIL_INDEX_V(chunk, world.chunkCount, Vector3i());
	auto* ch = world.getChunkUnsafe(chunk);
	ERR_FAIL_COND_V(!ch->hasBeenPlaced, Vector3i());
	return ch->globalLocation;
}
auto ChunkManager::chunk_set_location(I32 chunk, const Vector3i & p) -> Boolean
{
	ERR_FAIL_INDEX_V(chunk, world.chunkCount, false);
	return world.tgSetChunkLocation(chunk, p);
}
auto ChunkManager::chunk_is_in_world(I32 chunk) const -> Boolean
{
	ERR_FAIL_INDEX_V(chunk, world.chunkCount, false);
	return world.tgChunkHasBeenPlaced(chunk);
}
auto ChunkManager::chunk_remove_from_world(I32 chunk) -> Boolean
{
	ERR_FAIL_INDEX_V(chunk, world.chunkCount, false);
	return world.tgResetChunkLocation(chunk);
}


auto ChunkManager::_bind_methods() -> U0
{
	ClassDB::bind_method(
		D_METHOD("chunk_remove_from_world", "chunk"),
		&ChunkManager::chunk_remove_from_world
	);
	ClassDB::bind_method(
		D_METHOD("chunk_is_in_world", "chunk"),
		&ChunkManager::chunk_is_in_world
	);
	ClassDB::bind_method(
		D_METHOD("chunk_get_location", "chunk"),
		&ChunkManager::chunk_get_location
	);
	ClassDB::bind_method(
		D_METHOD("chunk_set_location", "chunk", "p"),
		&ChunkManager::chunk_set_location
	);
	ClassDB::bind_method(
		D_METHOD("chunk_has_point", "chunk", "p"),
		&ChunkManager::chunk_has_point
	);
	ClassDB::bind_method(
		D_METHOD("chunk_get_size", "chunk"),
		&ChunkManager::chunk_get_size
	);
	ClassDB::bind_method(
		D_METHOD("chunk_get_inner_count", "chunk"),
		&ChunkManager::chunk_get_inner_count
	);
	ClassDB::bind_method(
		D_METHOD("point_in_local_chunk_bounds", "p"),
		&ChunkManager::point_in_local_chunk_bounds
	);
	ClassDB::bind_method(
		D_METHOD("point_to_chunk_cel", "p"),
		&ChunkManager::point_to_chunk_cel
	);
	ClassDB::bind_method(
		D_METHOD("point_remove_chunk_part", "p"),
		&ChunkManager::point_remove_chunk_part
	);
	ClassDB::bind_method(
		D_METHOD("point_remove_subchunk_part", "p"),
		&ChunkManager::point_remove_subchunk_part
	);
	ClassDB::bind_method(
		D_METHOD("get_chunk_count"),
		&ChunkManager::get_chunk_count
	);
	ClassDB::bind_method(
		D_METHOD("resize", "new_count"),
		&ChunkManager::resize
	);
}