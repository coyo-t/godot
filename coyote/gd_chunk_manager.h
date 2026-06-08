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
	auto get_chunk_by_location (const Vector3i& global) -> I32;
	auto set_default_block_template_value (I64 to) -> U0;
	auto tg_get_handle (const Vector3i& global) -> RID;

	auto handle_get_chunk (const RID& handle) const -> I32;
	auto handle_get_block (const RID& handle) const -> I32;
	auto handle_to_vector2i (const RID& handle) const -> Vector2i;
	auto handle_from_vector2i (const Vector2i& i) const -> RID;
	auto handle_is_valid (const RID& handle) const -> Boolean;

	auto point_remove_subchunk_part (const Vector3i& p) const -> Vector3i;
	auto point_remove_chunk_part (const Vector3i& p) const -> Vector3i;
	auto point_to_chunk_cel (const Vector3i& p) const -> Vector3i;
	auto point_in_local_chunk_bounds (const Vector3i& p) const -> Boolean;
	auto point_pack (const Vector3i& p) const -> I64;
	auto point_pack_strict (const Vector3i& p) const -> I64;
	auto point_pack_localized (const Vector3i& p) const -> I64;
	auto point_pack_localized_adjacent (const Vector3i& p) const -> I64;
	auto point_unpack (I64 v) const -> Vector3i;

	auto chunk_get_inner_count (I32 chunk) const -> I32;
	auto chunk_get_size (I32 chunk) const -> Vector3i;
	auto chunk_has_point (I32 chunk, const Vector3i& p) const -> Boolean;
	auto chunk_get_location (I32 chunk) -> Vector3i;
	auto chunk_set_location (I32 chunk, const Vector3i& p) -> Boolean;
	auto chunk_is_in_world (I32 chunk) const -> Boolean;
	auto chunk_remove_from_world (I32 chunk) -> Boolean;

	auto block_get_template (const RID& handle) const -> I32;
	auto block_set_template (const RID& handle, I32 h) const -> Boolean;

	public:


	static auto _bind_methods () -> U0;
};

