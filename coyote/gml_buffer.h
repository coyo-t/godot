#pragma once

#include<cstdint>
#include<core/object/ref_counted.h>
#include<core/templates/vector.h>

class GMLBuffer : public RefCounted {
	GDCLASS(GMLBuffer, RefCounted);

	int64_t capacity = 0;
	int64_t cursor = 0;
	int64_t mark = -1;

	Vector<uint8_t> data {};

	public:
	auto writeU8 (int32_t at, uint8_t value) -> void;
	auto writeU16 (int32_t at, uint16_t value) -> void;
	auto writeU32 (int32_t at, uint32_t value) -> void;
	auto writeU64 (int32_t at, uint64_t value) -> void;

	auto writeNumber (int32_t at, double value) -> void;

	private:
	auto ensure (int64_t at, int64_t amount) -> void;

	public:
	static auto _bind_methods () -> void;
};
