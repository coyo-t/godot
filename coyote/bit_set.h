#pragma once

#include<cstdint>
#include<core/templates/vector.h>

class BitSet {
	uint64_t bitCount = 0;
	uint64_t byteCount = 0;
	uint8_t finalByteMask = 0;
	uint8_t* bytes = nullptr;

	private:
	auto normalizeEndingByte () const -> void;
	auto hasAnyData () const -> bool;

	
	public:
	~BitSet();

	auto clear (bool to=false) -> void;
	
	auto eq (const BitSet& other) const -> bool;
	
	auto resize (uint64_t newBitCount) -> void;
	auto toByteArray () const -> Vector<uint8_t>;
	auto getBitCount () const -> uint64_t;
	auto countTrueBits () const -> uint64_t;
	auto anyBitsAreTrue () const -> bool;
	auto andWithIsZero (const BitSet& other) const -> bool;

	auto getBit (uint64_t index) -> bool;
	auto setBitTrue (uint64_t index) -> bool;
	auto setBitFalse (uint64_t index) -> bool;
	auto setBitTo (uint64_t index, bool value) -> bool;
	auto toggleBit (uint64_t index) -> void;
};
