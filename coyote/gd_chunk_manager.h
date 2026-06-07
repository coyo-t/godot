#pragma once

#include<core/object/ref_counted.h>
#include<core/object/class_db.h>
#include<core/math/vector2i.h>
#include<core/math/vector3i.h>
#include<core/templates/hash_map.h>
#include<core/variant/typed_array.h>
#include<core/templates/rid.h>

#include"coyote.h"
#include"block_stuff.h"
#include"bit_set.h"

using namespace Coyote;

class ChunkManager : RefCounted {
	GDCLASS(ChunkManager, RefCounted);
	
	public:
	FunnyBlock::World world = {};

	public:
	ChunkManager ();
	~ChunkManager ();
	
	auto resize (I32 newCount) -> U0;
	auto get_chunk_count () -> I64;

	auto point_remove_subchunk_part (const Vector3i& p) const -> Vector3i;
	auto point_remove_chunk_part (const Vector3i& p) const -> Vector3i;
	auto point_to_chunk_cel (const Vector3i& p) const -> Vector3i;
	auto point_in_local_chunk_bounds (const Vector3i& p) const -> Boolean;

	auto chunk_get_inner_count (I32 chunk) const -> I32;
	auto chunk_get_size (I32 chunk) const -> Vector3i;
	auto chunk_has_point (I32 chunk, const Vector3i& p) const -> Boolean;
	auto chunk_get_location (I32 chunk) -> Vector3i;
	auto chunk_set_location (I32 chunk, const Vector3i& p) -> Boolean;
	auto chunk_is_in_world (I32 chunk) const -> Boolean;
	auto chunk_remove_from_world (I32 chunk) -> Boolean;


	public:


	static auto _bind_methods () -> U0;
};

