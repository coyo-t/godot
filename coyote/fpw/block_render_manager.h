#ifndef BLOCKRENMANAGERHAJKDLHSAJ
#define BLOCKRENMANAGERHAJKDLHSAJ


#include <cstdint>
#include <core/templates/vector.h>
#include <core/object/ref_counted.h>
#include <core/math/vector3i.h>
#include <core/variant/typed_dictionary.h>
#include <core/string/string_name.h>
#include <core/math/vector2.h>
#include <core/math/vector3.h>
#include <core/math/vector2i.h>
#include <scene/resources/texture.h>
#include <core/math/transform_2d.h>
#include <core/variant/typed_array.h>
#include <core/templates/rid.h>

struct SubTextureInfo {
	int index = -1;
	int pageIndex = -1;
	Vector2 uv0 { 0.0, 0.0 };
	Vector2 uv1 { 1.0, 1.0 };
	Ref<Texture2D> source;

	auto setUVs (const Vector2& t0, const Vector2& t1) -> void {
		uv0 = t0;
		uv1 = t1;
	};
};


class BlockTextureManager : public RefCounted {
	GDCLASS(BlockTextureManager, RefCounted);

	public:
	mutable Vector<Ref<Texture2D>> texturePages {};
	mutable Vector<SubTextureInfo> textures {};

	private:
	mutable TypedDictionary<RID, int> texturePageMapping {};
	mutable TypedDictionary<Ref<Texture2D>, int> texturesByResource {};
	Ref<Texture2D> missingTextureResource;
	int missingTextureIndex = -1;

	public:
	BlockTextureManager();
	~BlockTextureManager();

	auto setMissingTexture (Ref<Texture2D> to) -> void;
	auto getMissingTextureIndex () const -> int;
	auto getOrAddTexture (Ref<Texture2D> texture) -> int;

	public: // bindzorz
	auto sprite_get_page_index (int sprite_index) const -> int;
	auto sprite_get_uvs (int sprite_index) const -> Rect2;
	auto sprite_get_resource (int sprite_index) const -> Ref<Texture2D>;
	auto get_sprite_count () const -> int;

	auto get_texture_page (int pageIndex) const -> Ref<Texture2D>;
	auto get_texture_page_count () const -> int;

	private:
	auto setMissingToBeTheMissingistOfMissings () -> void;
	auto getOrAddTexturePage (Ref<Texture2D> texture) -> int;

	public:
	static auto _bind_methods () -> void;
};

#endif
