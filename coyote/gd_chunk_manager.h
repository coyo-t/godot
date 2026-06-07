#pragma once

#include<core/object/ref_counted.h>
#include<core/object/class_db.h>
#include<core/math/vector2i.h>
#include<core/math/vector3i.h>
#include<core/templates/hash_map.h>
#include<core/variant/typed_array.h>
#include<core/templates/rid.h>

#include"coyote.h"
#include"block_stuff.h"
#include"bit_set.h"

using namespace Coyote;

class ChunkManager : RefCounted {
	GDCLASS(ChunkManager, RefCounted);
	
	public:

	public:
	ChunkManager ();
	~ChunkManager ();
	

	public:


	static auto _bind_methods () -> U0;
};

