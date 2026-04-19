#include "register_tha_coyote_typez.h"

#include <core/object/class_db.h>
#include <core/os/os.h>

#include "fpw/block_render_manager.h"
#include "chunk_spacer.h"

auto register_tha_coyote_typez(int mode) -> void
{
	OS::get_singleton()->yield();
	switch (mode) {
		case 0: { // register
			GDREGISTER_CLASS(BlockTextureManager);
			GDREGISTER_CLASS(ChunkSpacer);
			break;
		}
		case 1: { // unregister
			break;
		}
		case 2: { // register singletons (if any)
			break;
		}
	}
	OS::get_singleton()->yield();
}
