#include "gml_buffer.h"

auto GMLBuffer::writeU8(int32_t at, uint8_t value) -> void
{
	wrBytez(at, data.ptrw(), value);
}

auto GMLBuffer::writeU16(int32_t at, uint16_t value) -> void
{
	wrBytez(at, data.ptrw(), value);
}


auto GMLBuffer::ensure(int64_t at, int64_t amount) -> void
{
	auto newp = at + amount;
	if (newp <= capacity)
	{
		return;
	}
	auto newsz = newp + (newp << 1) - (newp >> 1);
	data.resize_uninitialized(newsz);
	capacity = newsz;
}


