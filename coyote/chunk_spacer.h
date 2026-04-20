#pragma once

#include<cstdint>
#include<core/math/vector3i.h>
#include<core/object/class_db.h>
#include<core/object/ref_counted.h>
#include<core/typedefs.h>
#include<core/templates/vector.h>

class ChunkSpacer : public RefCounted {
	GDCLASS(ChunkSpacer, RefCounted);

	public:
	Vector3i cardinals[6];
	
	int size_po2 = 0;
	int size = 0;

	int bitIndex = 0;
	int innerCount = 0;
	uint64_t componentMask = 0;
	uint64_t inverseComponentMask = 0;
	int componentBits = 1;
	uint64_t maxMask = 0;
	uint64_t inverseMaxMask = 0;

	int cxOffset = 0;
	int cyOffset = 0;
	int czOffset = 0;

	Vector3i* innerOrder = nullptr;
	int64_t* innerAdjacency = nullptr;
	
	public:
	auto resize (int to) -> void;

	auto setCardinal (int index, const Vector3i& v) -> void;
	
	auto getAdjacentCoord (int i, int cardinal) const -> int;

	auto getOrderCoord (int i) const -> Vector3i;

	_FORCE_INLINE_ auto chunkifyVector (const Vector3i& v) const -> Vector3i {
		return Vector3i(v.x >> bitIndex, v.y >> bitIndex, v.z >> bitIndex);
	}

	_FORCE_INLINE_ auto unchunkifyVector (const Vector3i& v) const -> Vector3i {
		return Vector3i(v.x<<bitIndex, v.y<<bitIndex, v.z<<bitIndex);
	}

	_FORCE_INLINE_ auto encodeLocalVector (const Vector3i& v) const -> int {
		return (((v.x&componentMask)<<cxOffset)|((v.y&componentMask)<<cyOffset)|((v.z&componentMask)<<czOffset));
	}

	_FORCE_INLINE_ auto decodeLocalVector (int i) const -> Vector3i {
		auto j = i & maxMask;
		return Vector3i(
			(j>>cxOffset)&componentMask,
			(j>>cyOffset)&componentMask,
			(j>>czOffset)&componentMask
		);
	}

	_FORCE_INLINE_ auto localizeVector (const Vector3i& v) const -> Vector3i {
		return Vector3i(v.x&componentMask, v.y&componentMask, v.z&componentMask);
	}

	_FORCE_INLINE_ auto vectorIsInLocal (const Vector3i& v) const -> bool {
		return (
			((v.x&inverseComponentMask)==0)&&
			((v.y&inverseComponentMask)==0)&&
			((v.z&inverseComponentMask)==0)
		);
	}

	auto gd_get_inner_count () const -> uint64_t;
	auto gd_get_size () const -> int;
	auto gd_get_size_po2 () const -> int;
	auto gd_get_component_bit_count () const -> int;
	auto gd_get_component_mask () const -> uint64_t;
	auto gd_get_component_offset () const -> Vector3i;
	auto gd_get_component_total_mask () const -> uint64_t;


	
	ChunkSpacer();
	~ChunkSpacer();

	static auto _bind_methods () -> void;
};


