#include "gd_bit_set.h"

#include<core/templates/vector.h>


auto GDBitSet::_bind_methods() -> void
{

	ClassDB::bind_method(D_METHOD("resize", "bit_count"), &BitSet::resize);
	ClassDB::bind_method(D_METHOD("clear", "value"), &BitSet::clear);
	ClassDB::bind_method(D_METHOD("to_byte_array"), &BitSet::toByteArray);
	ClassDB::bind_method(D_METHOD("doesnt_overlap", "other"), &BitSet::andWithIsZero);
	ClassDB::bind_method(D_METHOD("any_are_true"), &BitSet::anyBitsAreTrue);
	ClassDB::bind_method(D_METHOD("count_true_bits"), &BitSet::countTrueBits);

	ClassDB::bind_method(D_METHOD("get_bit", "index"), &BitSet::getBit);
	ClassDB::bind_method(D_METHOD("set_bit", "index", "to"), &BitSet::setBitTo);
	ClassDB::bind_method(D_METHOD("toggle_bit", "index"), &BitSet::toggleBit);
	ClassDB::bind_method(D_METHOD("set_bit_on", "index"), &BitSet::setBitTrue);
	ClassDB::bind_method(D_METHOD("set_bit_off", "index"), &BitSet::setBitFalse);
	
	ClassDB::bind_method(D_METHOD("contents_equals", "other"), &BitSet::eq);

	ClassDB::bind_method(D_METHOD("size"), &BitSet::getBitCount);
}