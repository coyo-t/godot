#include"block_render_manager.h"

#include<core/math/rect2.h>
#include<scene/resources/atlas_texture.h>
#include<core/math/math_funcs.h>
#include<servers/rendering/rendering_server.h>
#include<scene/resources/image_texture.h>
#include<scene/resources/placeholder_textures.h>


static auto atlasRegion (Ref<AtlasTexture> atlasTexture) -> Rect2 {
	ERR_FAIL_COND_V(atlasTexture.is_null(), Rect2());
	const auto t = atlasTexture->get_atlas();
	auto m = atlasTexture->get_region();
	if (t.is_null())
		return m;
	auto sz = t->get_size();
	if (Math::is_zero_approx(m.size.x))
		m.size.x = sz.x;
	if (Math::is_zero_approx(m.size.y))
		m.size.y = sz.y;
	return m;
};

BlockTextureManager::BlockTextureManager()
{
	setMissingToBeTheMissingistOfMissings();
}

BlockTextureManager::~BlockTextureManager()
{
}

auto BlockTextureManager::
setMissingTexture (Ref<Texture2D> to) -> void
{
	missingTextureIndex = getOrAddTexture(to);
	missingTextureResource = to;
}

auto BlockTextureManager::
getMissingTextureIndex() const -> int
{
	return missingTextureIndex;
}

auto BlockTextureManager::
setMissingToBeTheMissingistOfMissings() -> void
{
	const auto rs = RenderingServer::get_singleton();
	const auto test = rs->get_test_texture();
	const auto uhh = rs->texture_2d_get(test);
	missingTextureResource = ImageTexture::create_from_image(uhh);
}

auto BlockTextureManager::
getOrAddTexturePage(Ref<Texture2D> texture) -> int
{
	if (texture.is_null())
		return missingTextureIndex;
	const auto rid = texture->get_rid();
	if (texturePageMapping.has(rid))
		return texturePageMapping.get(rid, missingTextureIndex);
	const auto np = texturePages.size();
	texturePages.append(texture);
	texturePageMapping.set(rid, np);
	return np;
}

auto BlockTextureManager::
getOrAddTexture (Ref<Texture2D> texture) -> int {
	if (texture.is_null()) {
		return missingTextureIndex;
	}
	const Ref<PlaceholderTexture2D> asPlaceholder = texture;
	if (asPlaceholder.is_valid()) {
		return getOrAddTexture(missingTextureResource);
	}
	
	if (texturesByResource.has(texture)) {
		return texturesByResource.get(texture, missingTextureIndex);
	}

	auto outs = SubTextureInfo();

	const Ref<AtlasTexture> asAtlas = texture;
	if (asAtlas.is_valid())
	{
		const auto page = asAtlas->get_atlas();
		auto pageIndex = getOrAddTexturePage(page);
		outs.pageIndex = pageIndex;
		auto region = atlasRegion(asAtlas);
		// not using page directly as if its null this throws. getting like this
		// ensures if it is null were using the missing texture
		auto size = Vector2(texturePages[pageIndex]->get_size());
		outs.setUVs(region.get_position()/size, region.get_end()/size);
	}
	else
	{
		outs.pageIndex = getOrAddTexturePage(texture);
	}
	outs.source = texture;
	auto newIndex = textures.size();
	outs.index = newIndex;
	texturesByResource[texture] = newIndex;
	textures.append(outs);
	return newIndex;
}

auto BlockTextureManager::
sprite_get_page_index(int sprite_index) const -> int
{
	ERR_FAIL_INDEX_V(sprite_index, textures.size(), missingTextureIndex);
	return textures[sprite_index].pageIndex;
}

auto BlockTextureManager::
sprite_get_uvs(int sprite_index) const -> Rect2
{
	ERR_FAIL_INDEX_V(sprite_index, textures.size(), Rect2());
	const auto tex = textures[sprite_index];
	const auto uv0 = tex.uv0;
	return Rect2(uv0, tex.uv1-uv0);
}

auto BlockTextureManager::
sprite_get_resource(int sprite_index) const -> Ref<Texture2D>
{
	ERR_FAIL_INDEX_V(sprite_index, textures.size(), Ref<Texture2D>());
	return textures[sprite_index].source;
}

auto BlockTextureManager::
get_sprite_count() const -> int
{
	return textures.size();
}

auto BlockTextureManager::
get_texture_page(int pageIndex) const -> Ref<Texture2D>
{
	ERR_FAIL_INDEX_V(pageIndex, texturePages.size(), Ref<Texture2D>());
	return texturePages[pageIndex];
}

auto BlockTextureManager::
get_texture_page_count() const -> int
{
	return texturePages.size();
}

auto BlockTextureManager::
_bind_methods() -> void
{
	ClassDB::bind_method(
		D_METHOD("get_texture_page"),
		&BlockTextureManager::get_texture_page
	);
	ClassDB::bind_method(
		D_METHOD("get_texture_page_count"),
		&BlockTextureManager::get_texture_page_count
	);
	ClassDB::bind_method(
		D_METHOD("get_sprite_count"),
		&BlockTextureManager::get_sprite_count
	);
	ClassDB::bind_method(
		D_METHOD("set_missing_sprite", "texture"),
		&BlockTextureManager::setMissingTexture
	);
	ClassDB::bind_method(
		D_METHOD("get_missing_sprite_index"),
		&BlockTextureManager::getMissingTextureIndex
	);
	ClassDB::bind_method(
		D_METHOD("get_or_add_sprite", "texture"),
		&BlockTextureManager::getOrAddTexture
	);

	ClassDB::bind_method(
		D_METHOD("sprite_get_page_index", "sprite_index"),
		&BlockTextureManager::sprite_get_page_index
	);

	ClassDB::bind_method(
		D_METHOD("sprite_get_uvs", "sprite_index"),
		&BlockTextureManager::sprite_get_uvs
	);

	ClassDB::bind_method(
		D_METHOD("sprite_get_resource", "sprite_index"),
		&BlockTextureManager::sprite_get_resource
	);
}

