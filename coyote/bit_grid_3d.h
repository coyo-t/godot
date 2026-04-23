#pragma once

#include<cstdint>
#include<core/object/ref_counted.h>
#include<core/object/class_db.h>

class BitGrid3D : public RefCounted {
	// GDCLASS(BitGrid3D, RefCounted);

	uint64_t virtualSize = 0;
	uint64_t dataSize = 0;
	uint8_t* data = nullptr;
	uint32_t xSize = 0;
	uint32_t ySize = 0;
	uint32_t zSize = 0;

	auto _setBitFalse (uint64_t address) -> void;
	auto _setBitTrue (uint64_t address) -> void;

};
