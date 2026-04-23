#include "bit_grid_3d.h"

auto BitGrid3D::_setBitFalse(uint64_t address) -> void
{
	auto bitIndex = address & 7;
	auto byteIndex = address >> 3;
	data[byteIndex] &= ~(1 << bitIndex);
}

auto BitGrid3D::_setBitTrue(uint64_t address) -> void
{
	auto bitIndex = address & 7;
	auto byteIndex = address >> 3;
	data[byteIndex] |= (1 << bitIndex);
}
