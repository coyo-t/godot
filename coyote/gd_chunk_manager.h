#pragma once

#include<core/object/ref_counted.h>
#include<core/object/class_db.h>
#include<core/math/vector2i.h>
#include<core/math/vector3i.h>

#include"coyote.h"
#include"block_stuff.h"

using namespace Coyote;

class ChunkManager : RefCounted {
	GDCLASS(ChunkManager, RefCounted);
	
	FunnyBlock::World world = {};

	ChunkManager ();
	~ChunkManager ();

	auto resize (I32 size) -> U0;

	auto handle_get_block (const Vector2i& handle) const -> I32;
	auto handle_get_chunk (const Vector2i& handle) const -> I32;

	auto get_block (const Vector2i& handle) const -> FunnyBlock::BlockInstance*;

	auto get_chunk_count () const -> I32;
	auto set_chunk_count (I32 to) -> U0;

	auto encode_chunk_point (const Vector3i& p, I32 mode=0) const -> I64;
	auto decode_chunk_point (I64 p) const -> Vector3i;

	auto chunk_get_global_location (I32 handle) const -> Vector3i;
	auto chunk_set_global_location (I32 handle, const Vector3i& p) -> Vector3i;

	auto block_get_template_handle (const Vector2i& handle) const -> I64;
	auto block_set_template_handle (const Vector2i& handle, I64 to) const -> I64;

	static auto _bind_methods () -> U0;
};

