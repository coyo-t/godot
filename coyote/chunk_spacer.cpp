#include "chunk_spacer.h"

#include<core/os/memory.h>
#include<core/variant/variant.h>

auto ChunkSpacer::resize (int to) -> void
{
	int biti = -1;
	for (auto i = 0; i < 16; i++)
	{
		if (((1<<i) & to) != 0)
		{
			biti = i;
			break;
		}
	}
	// arbitrary
	ERR_FAIL_COND(0 > biti || biti >= 8);


	uint64_t bitc = 1 << biti;
	uint64_t bitmask = (1 << biti) - 1;
	bitIndex = biti;
	componentMask = bitmask;
	inverseComponentMask = ~bitmask;
	componentBits = bitc;
	cxOffset = biti << 1;
	cyOffset = biti;
	czOffset = 0;

	size_po2 = biti;
	size = bitc;

	maxMask = (bitmask<<cxOffset)|(bitmask<<cyOffset)|(bitmask<<czOffset);
	inverseMaxMask = ~maxMask;
	
	if (innerOrder != nullptr)
	{
		memfree(innerOrder);
	}
	if (innerAdjacency != nullptr)
	{
		memfree(innerAdjacency);
	}

	innerCount = bitc * bitc * bitc;
	Vector3i* order = static_cast<Vector3i*>(memalloc(sizeof(Vector3i) * innerCount));
	int64_t* adjacency = static_cast<int64_t*>(memalloc(sizeof(int64_t) * innerCount * 6));

	for (auto i = 0; i < innerCount; i++)
	{
		auto v = decodeLocalVector(i);
		order[i] = v;
		for (auto j = 0; j < 6; j++)
		{
			auto ofs = v + cardinals[j];
			auto localz = localizeVector(ofs);
			auto jk = encodeLocalVector(localz);
			if (vectorIsInLocal(ofs))
			{
				adjacency[i * 6 + j] = jk;
			}
			else
			{
				adjacency[i * 6 + j] = -jk - 1;
			}
		}
	}
	
	innerOrder = order;
	innerAdjacency = adjacency;
}

ChunkSpacer::ChunkSpacer()
{
}

ChunkSpacer::~ChunkSpacer()
{
	if (innerAdjacency != nullptr)
	{
		memfree(innerAdjacency);
	}
	if (innerOrder != nullptr)
	{
		memfree(innerOrder);
	}
}

auto ChunkSpacer::getAdjacentCoord(int i, int cardinal) const -> int
{
	ERR_FAIL_COND_V(innerAdjacency == nullptr, 0);
	ERR_FAIL_INDEX_V_MSG(cardinal, 6, 0, "cardinal out of range");
	ERR_FAIL_INDEX_V_MSG(i, innerCount, 0, "inner index out of bounds");
	return innerAdjacency[i*6+cardinal];
}

auto ChunkSpacer::getOrderCoord(int i) const -> Vector3i
{
	ERR_FAIL_COND_V(innerOrder == nullptr, Vector3i());
	ERR_FAIL_INDEX_V(i, innerCount, Vector3i());
	return innerOrder[i];
}

auto ChunkSpacer::setCardinal(int index, const Vector3i& v) -> void
{
	ERR_FAIL_INDEX(index, 6);
	cardinals[index] = v;
}

auto ChunkSpacer::gd_get_inner_count() const -> uint64_t
{
	return innerCount;
}

auto ChunkSpacer::gd_get_size() const -> int
{
	return size;
}

auto ChunkSpacer::gd_get_size_po2() const -> int
{
	return size_po2;
}

auto ChunkSpacer::gd_get_component_bit_count() const -> int
{
	return componentBits;
}

auto ChunkSpacer::gd_get_component_mask() const -> uint64_t
{
	return componentMask;
}

auto ChunkSpacer::gd_get_component_offset() const -> Vector3i
{
	return Vector3i(cxOffset, cyOffset, czOffset);
}

auto ChunkSpacer::gd_get_component_total_mask() const -> uint64_t
{
	return maxMask;
}

auto ChunkSpacer::_bind_methods() -> void
{
	ClassDB::bind_method(
		D_METHOD("resize", "to"),
		&ChunkSpacer::resize
	);
	ClassDB::bind_method(
		D_METHOD("set_cardinal_vector", "cardinal_index", "cardinal_vector"),
		&ChunkSpacer::setCardinal
	);
	ClassDB::bind_method(
		D_METHOD("get_adjacenct_vector", "inner_index", "cardinal_index"),
		&ChunkSpacer::getAdjacentCoord
	);
	ClassDB::bind_method(
		D_METHOD("get_inner_vector", "inner_index"),
		&ChunkSpacer::getOrderCoord
	);

	ClassDB::bind_method(
		D_METHOD("chunkify_vector", "v"),
		&ChunkSpacer::chunkifyVector
	);
	ClassDB::bind_method(
		D_METHOD("unchunkify_vector", "v"),
		&ChunkSpacer::unchunkifyVector
	);
	ClassDB::bind_method(
		D_METHOD("encode_vector", "v"),
		&ChunkSpacer::encodeLocalVector
	);
	ClassDB::bind_method(
		D_METHOD("decode_vector", "packed"),
		&ChunkSpacer::decodeLocalVector
	);
	ClassDB::bind_method(
		D_METHOD("localize_vector", "v"),
		&ChunkSpacer::localizeVector
	);
	ClassDB::bind_method(
		D_METHOD("is_in_local_bounds", "v"),
		&ChunkSpacer::vectorIsInLocal
	);

	ClassDB::bind_method(
		D_METHOD("get_inner_count"),
		&ChunkSpacer::gd_get_inner_count
	);
	ADD_PROPERTY(
		PropertyInfo(Variant::INT, "inner_count"),
		"",
		"get_inner_count"
	);

	ClassDB::bind_method(
		D_METHOD("get_component_bit_count"),
		&ChunkSpacer::gd_get_component_bit_count
	);
	ADD_PROPERTY(
		PropertyInfo(Variant::INT, "component_bit_count"),
		"",
		"get_component_bit_count"
	);
	ClassDB::bind_method(
		D_METHOD("get_component_mask"),
		&ChunkSpacer::gd_get_component_mask
	);
	ADD_PROPERTY(
		PropertyInfo(Variant::INT, "component_mask"),
		"",
		"get_component_mask"
	);
	ClassDB::bind_method(
		D_METHOD("get_component_offset"),
		&ChunkSpacer::gd_get_component_offset
	);
	ADD_PROPERTY(
		PropertyInfo(Variant::VECTOR3I, "component_offset"),
		"",
		"get_component_offset"
	);
	ClassDB::bind_method(
		D_METHOD("get_component_total_mask"),
		&ChunkSpacer::gd_get_component_total_mask
	);
	ADD_PROPERTY(
		PropertyInfo(Variant::INT, "component_total_mask"),
		"",
		"get_component_total_mask"
	);

	ClassDB::bind_method(
		D_METHOD("get_size"),
		&ChunkSpacer::gd_get_size
	);
	ADD_PROPERTY(
		PropertyInfo(Variant::INT, "size"),
		"",
		"get_size"
	);

	ClassDB::bind_method(
		D_METHOD("get_size_po2"),
		&ChunkSpacer::gd_get_size_po2
	);
	ADD_PROPERTY(
		PropertyInfo(Variant::INT, "size_po2"),
		"",
		"get_size_po2"
	);
}

