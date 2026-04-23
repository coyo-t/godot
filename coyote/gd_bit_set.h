#pragma once

#include<core/object/class_db.h>
#include<core/object/ref_counted.h>
#include"bit_set.h"


class GDBitSet : public BitSet, public RefCounted {
	GDCLASS(GDBitSet, RefCounted);


	public:

	static auto _bind_methods () -> void;
};
