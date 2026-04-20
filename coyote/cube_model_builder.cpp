#include"cube_model_builder.h"
#include<core/math/math_funcs.h>

struct FaceIndex {
	int x0, y0;
	int x1, y1;

	int depth;
	int xNeg, yNeg, zNeg;
};

const Vector3 FACE_NORMALS[] = {
	{  0,  0, -1 },
	{ +1,  0,  0 },
	{  0,  0, +1 },
	{ -1,  0,  0 },
	{  0, -1,  0 },
	{  0, +1,  0 },
};

const Transform3D FACE_TRANSFORMS[] = {
	Transform3D(
		Vector3(-1, 0, 0),
		Vector3(0, 1, 0),
		Vector3(0, 0, 1),
		Vector3(1, 0, 0)
	),
	Transform3D(
		Vector3(0, 0, -1),
		Vector3(0, 1, 0),
		Vector3(1, 0, 0),
		Vector3(0, 0, 1)
	),
	Transform3D(
		Vector3(1, 0, 0),
		Vector3(0, 1, 0),
		Vector3(0, 0, 1),
		Vector3(0, 0, 0)
	),
	Transform3D(
		Vector3(0, 0, 1),
		Vector3(0, 1, 0),
		Vector3(1, 0, 0),
		Vector3(0, 0, 0)
	),
	Transform3D(
		Vector3(1, 0, 0),
		Vector3(0, 0, 1),
		Vector3(0, 1, 0),
		Vector3(0, 0, 0)
	),
	Transform3D(
		Vector3(1, 0, 0),
		Vector3(0, 0, -1),
		Vector3(0, 1, 0),
		Vector3(0, 0, 1)
	),
};

constexpr FaceIndex FACE_INDICES[] = {
	//0  1   2  3   4      5  6  7
	//x0,y0, x1,y1, depth, -x,-y,-z
	{ 3,1, 0,4, 2, 1,0,1 },
	{ 5,1, 2,4, 3, 1,0,0 },
	{ 0,1, 3,4, 5, 0,0,0 },
	{ 2,1, 5,4, 0, 0,0,1 },
	{ 0,2, 3,5, 1, 0,0,1 },
	{ 0,5, 3,2, 4, 0,1,0 },
};

auto CubeModelBuilder::createFace (int cardinal) -> bool
{
	const auto bb = bbox.abs();
	const auto bbp = bb.get_position();
	const auto bbe = bb.get_end();
	const double corners[6] = {
		bbp.x, bbp.y, bbp.z,
		bbe.x, bbe.y, bbe.z,
	};
	const auto faceData = FACE_INDICES[cardinal];
	auto x0 = corners[faceData.x0];
	auto y0 = corners[faceData.y0];
	auto x1 = corners[faceData.x1];
	auto y1 = corners[faceData.y1];
	if (faceData.xNeg != 0) {
		x0 = 1 - x0;
		x1 = 1 - x1;
	}
	if (faceData.yNeg != 0) {
		y0 = 1 - y0;
		y1 = 1 - y1;
	}
	x0 += faceInset;
	y0 += faceInset;
	x1 -= faceInset;
	y1 -= faceInset;

	newVertexCount = 0;
	newIndexCount = 0;
	if (Math::is_zero_approx(x1-x0) || Math::is_zero_approx(y1-y0)) {
		return false;
	}
	
	auto q0 = Vector2(x0, y0);
	auto q1 = Vector2(x1, y1);
	auto depth = corners[faceData.depth] + ((faceData.zNeg==0) ? faceOffset : -faceOffset);
	auto trans = FACE_TRANSFORMS[cardinal].translated(cornerPostOffset);
	auto ntrans = trans.basis.orthonormalized();

	auto uv0 = Vector2 {
		q0.x,
		1-q1.y,
	};
	auto uv1 = Vector2 {
		q1.x,
		1-q0.y,
	};
	vVertices[0] = trans.xform(Vector3(q0.x, q0.y, depth));
	vVertices[1] = trans.xform(Vector3(q0.x, q1.y, depth));
	vVertices[2] = trans.xform(Vector3(q1.x, q1.y, depth));
	vVertices[3] = trans.xform(Vector3(q1.x, q0.y, depth));
	vUVs[0] = { uv0.x, uv1.y };
	vUVs[1] = { uv0.x, uv0.y };
	vUVs[2] = { uv1.x, uv0.y };
	vUVs[3] = { uv1.x, uv1.y };
	vIndices[0] = { 0, 1, 2 };
	vIndices[1] = { 0, 2, 3 };
	newVertexCount = 4;
	newIndexCount = 2;
	return true;
}

auto CubeModelBuilder::get_bbox() const -> AABB
{
	return bbox;
}

auto CubeModelBuilder::set_bbox(const AABB & b) -> void
{
	bbox = b;
}

auto CubeModelBuilder::get_corner_post_offset() const -> Vector3
{
	return cornerPostOffset;
}

auto CubeModelBuilder::set_corner_post_offset(const Vector3 & v) -> void
{
	cornerPostOffset = v;
}

auto CubeModelBuilder::get_face_offset() const -> double
{
	return faceOffset;
}

auto CubeModelBuilder::set_face_offset(const double d) -> void
{
	faceOffset = d;
}

auto CubeModelBuilder::get_face_inset() const -> double
{
	return faceInset;
}

auto CubeModelBuilder::set_face_inset(const double d) -> void
{
	faceInset = d;
}

auto CubeModelBuilder::get_new_data_count() const -> Vector2i
{
	return Vector2i(newVertexCount, newIndexCount);
}

auto CubeModelBuilder::rendered_anything() const -> bool
{
	return newVertexCount > 0 && newIndexCount > 0;
}

auto CubeModelBuilder::getTriangle(int i) const -> Vector3i
{
	ERR_FAIL_INDEX_V(i, 2, Vector3i());
	return vIndices[i];
}

auto CubeModelBuilder::vGetVertex(int i) const -> Vector3
{
	ERR_FAIL_INDEX_V(i, 4, Vector3());
	return vVertices[i];
}

auto CubeModelBuilder::vGetUVs(int i) const -> Vector2
{
	ERR_FAIL_INDEX_V(i, 4, Vector2());
	return vUVs[i];
}

auto CubeModelBuilder::_bind_methods() -> void
{
	ClassDB::bind_method(
		D_METHOD("set_bbox", "to"),
		&CubeModelBuilder::set_bbox
	);
	ClassDB::bind_method(
		D_METHOD("get_bbox"),
		&CubeModelBuilder::get_bbox
	);
	ADD_PROPERTY(
		PropertyInfo(Variant::AABB, "bbox"),
		"set_bbox",
		"get_bbox"
	);

	ClassDB::bind_method(
		D_METHOD("result_get_vertex", "i"),
		&CubeModelBuilder::vGetVertex
	);
	ClassDB::bind_method(
		D_METHOD("result_get_uvs", "i"),
		&CubeModelBuilder::vGetUVs
	);
	ClassDB::bind_method(
		D_METHOD("result_get_triangle", "i"),
		&CubeModelBuilder::getTriangle
	);

	ClassDB::bind_method(
		D_METHOD("set_face_offset", "to"),
		&CubeModelBuilder::set_face_offset
	);
	ClassDB::bind_method(
		D_METHOD("get_face_offset"),
		&CubeModelBuilder::get_face_offset
	);
	ADD_PROPERTY(
		PropertyInfo(Variant::FLOAT, "face_offset"),
		"set_face_offset",
		"get_face_offset"
	);

	ClassDB::bind_method(
		D_METHOD("set_face_inset", "to"),
		&CubeModelBuilder::set_face_inset
	);
	ClassDB::bind_method(
		D_METHOD("get_face_inset"),
		&CubeModelBuilder::get_face_inset
	);
	ADD_PROPERTY(
		PropertyInfo(Variant::FLOAT, "face_inset"),
		"set_face_inset",
		"get_face_inset"
	);

	ClassDB::bind_method(
		D_METHOD("set_corner_post_offset", "to"),
		&CubeModelBuilder::set_corner_post_offset
	);
	ClassDB::bind_method(
		D_METHOD("get_corner_post_offset"),
		&CubeModelBuilder::get_corner_post_offset
	);
	ADD_PROPERTY(
		PropertyInfo(Variant::VECTOR3, "corner_post_offset"),
		"set_corner_post_offset",
		"get_corner_post_offset"
	);

	ClassDB::bind_method(
		D_METHOD("rendered_anything"),
		&CubeModelBuilder::rendered_anything
	);

	ClassDB::bind_method(
		D_METHOD("get_new_data_counts"),
		&CubeModelBuilder::get_new_data_count
	);
	ADD_PROPERTY(
		PropertyInfo(Variant::VECTOR2I, "new_data_counts"),
		"",
		"get_new_data_counts"
	);

	ClassDB::bind_method(
		D_METHOD("create_face","cardinal"),
		&CubeModelBuilder::createFace
	);

}




