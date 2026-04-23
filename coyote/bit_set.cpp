#include "bit_set.h"
#include<core/os/memory.h>
#include<core/math/math_funcs.h>

// this is marked as const even though it "modifies" the bitset
// because as far as the outside world cares, nothing about the bitset changes
auto BitSet::normalizeEndingByte() const -> void
{
	if (hasAnyData())
	{
		bytes[byteCount-1] &= finalByteMask;
	}
}

auto BitSet::hasAnyData() const -> bool
{
	return bytes != nullptr && byteCount > 0;
}

BitSet::BitSet()
{
}

BitSet::~BitSet()
{
	if (hasAnyData())
	{
		memfree(bytes);
		bytes = nullptr;
	}
}

auto BitSet::clear(bool to) -> void
{
	if (!hasAnyData())
		return;
	std::memset(bytes, to ? 0xFF : 0, byteCount);
	normalizeEndingByte();
}

auto BitSet::eq(const BitSet & other) const -> bool
{
	auto iHaveData = hasAnyData();
	auto sheHasData = other.hasAnyData();
	if (!iHaveData)
	{
		return !sheHasData;
	}
	if (!sheHasData)
	{
		return !iHaveData;
	}
	normalizeEndingByte();
	other.normalizeEndingByte();
	return std::memcmp(bytes, other.bytes, byteCount);
}

auto BitSet::andWithIsZero(const BitSet & other) const -> bool
{
	normalizeEndingByte();
	other.normalizeEndingByte();
	uint8_t noThotHedEmpy = 0;
	uint8_t* heah = &noThotHedEmpy;
	auto mySize = byteCount;
	auto herSize = other.byteCount;
	// auto minSize = (mySize < herSize) ? mySize : herSize;
	auto maxSize = (mySize > herSize) ? mySize : herSize;
	auto myPtr = (hasAnyData()) ? bytes : heah;
	auto myEnd = myPtr + mySize;
	auto herPtr = (other.hasAnyData()) ? other.bytes : heah;
	auto herEnd = herPtr + herSize;

	for (uint64_t i = 0; i < maxSize; i++)
	{
		if (myPtr == heah && herPtr == heah)
		{
			break;
		}
		if (((*myPtr) & (*herPtr)) != 0)
		{
			return false;
		}
		if (myPtr != heah && myPtr >= myEnd)
		{
			myPtr = heah;
		}
		else
		{
			myPtr++;
		}
		if (herPtr != heah && herPtr >= herEnd)
		{
			herPtr = heah;
		}
		else
		{
			herPtr++;
		}
	}
	return true;
}

auto BitSet::anyBitsAreTrue() const -> bool
{
	if (!hasAnyData())
		return false;
	normalizeEndingByte();
	const auto* end = bytes+byteCount;
	auto* b = bytes;
	while (b < end)
	{
		if (*b != 0)
			return true;
		b++;
	}
	return false;
}

auto BitSet::resize(uint64_t newBitCount) -> void
{
	uint64_t newByteCount;
	if (newBitCount == 0)
		newByteCount = 0;
	else
		newByteCount = ((newBitCount - 1) >> 3) + 1;
	//
	uint64_t actualBitCount = newByteCount * 8;
	int deadBitCount = actualBitCount - newBitCount;
	uint8_t endMask = 0b1111'1111 >> deadBitCount;

	bytes = static_cast<uint8_t*>(memrealloc(bytes, newByteCount));
	bitCount = newBitCount;
	byteCount = newByteCount;
	finalByteMask = endMask;
	normalizeEndingByte();
}

auto BitSet::toByteArray() const -> Vector<uint8_t>
{
	if (!hasAnyData())
		return Vector<uint8_t>();
	const auto bc = byteCount;
	auto news = Vector<uint8_t>();
	news.reserve(bc);
	normalizeEndingByte();
	auto myAddr = bytes;
	auto herAddr = news.ptrw();
	for (uint64_t i = 0; i < bc; i++)
	{
		*herAddr = *myAddr;
		myAddr++;
		herAddr++;
	}
	return news;
}

auto BitSet::getBitCount() const -> uint64_t
{
	return bitCount;
}

auto BitSet::countTrueBits() const -> uint64_t
{
	if (!hasAnyData())
		return 0;
	normalizeEndingByte();
	return Math::count_bits(bytes, byteCount);
}

auto BitSet::getBit(uint64_t index) -> bool
{
	ERR_FAIL_COND_V(bytes == nullptr, false);
	ERR_FAIL_INDEX_V(index, bitCount, false);
	return (bytes[index >> 3] & (1<<(index&7))) != 0;
}

auto BitSet::setBitTrue(uint64_t index) -> bool
{
	ERR_FAIL_COND_V(bytes == nullptr, false);
	ERR_FAIL_INDEX_V(index, bitCount, false);
	auto by = index >> 3;
	auto pev = bytes[by];
	auto news = pev | (1 << (index & 7));
	bytes[by] = news;
	return pev != news;
}

auto BitSet::setBitFalse(uint64_t index) -> bool
{
	ERR_FAIL_COND_V(bytes == nullptr, false);
	ERR_FAIL_INDEX_V(index, bitCount, false);
	auto by = index >> 3;
	auto pev = bytes[by];
	auto news = pev & ~(1 << (index & 7));
	bytes[by] = news;
	return pev != news;
}

auto BitSet::toggleBit(uint64_t index) -> void
{
	ERR_FAIL_COND(bytes == nullptr);
	ERR_FAIL_INDEX(index, bitCount);
	bytes[index >> 3] ^= (1 << (index & 7));
}

auto BitSet::gd_and_with_is_0(Ref<BitSet> other) const -> bool
{
	ERR_FAIL_COND_V(other.is_null(), false);
	return andWithIsZero(*other.ptr());
}

auto BitSet::gd_contents_eq(const Ref<BitSet> other) const -> bool
{
	ERR_FAIL_COND_V(other.is_null(), false);
	return andWithIsZero(*other.ptr());
}

auto BitSet::setBitTo(uint64_t index, bool value) -> bool
{
	if (value)
		return setBitTrue(index);
	else
		return setBitFalse(index);
}


auto BitSet::_bind_methods() -> void
{

	ClassDB::bind_method(D_METHOD("resize", "bit_count"), &BitSet::resize);
	ClassDB::bind_method(D_METHOD("clear", "value"), &BitSet::clear, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("to_byte_array"), &BitSet::toByteArray);
	ClassDB::bind_method(D_METHOD("doesnt_overlap", "other"), &BitSet::gd_and_with_is_0);
	ClassDB::bind_method(D_METHOD("any_are_true"), &BitSet::anyBitsAreTrue);
	ClassDB::bind_method(D_METHOD("count_true_bits"), &BitSet::countTrueBits);

	ClassDB::bind_method(D_METHOD("get_bit", "index"), &BitSet::getBit);
	ClassDB::bind_method(D_METHOD("set_bit", "index", "to"), &BitSet::setBitTo);
	ClassDB::bind_method(D_METHOD("toggle_bit", "index"), &BitSet::toggleBit);
	ClassDB::bind_method(D_METHOD("set_bit_on", "index"), &BitSet::setBitTrue);
	ClassDB::bind_method(D_METHOD("set_bit_off", "index"), &BitSet::setBitFalse);
	
	ClassDB::bind_method(D_METHOD("contents_equals", "other"), &BitSet::gd_contents_eq);

	ClassDB::bind_method(D_METHOD("size"), &BitSet::getBitCount);
}
