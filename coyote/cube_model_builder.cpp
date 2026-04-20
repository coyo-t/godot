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

auto CubeModelBuilder::createFace (
	int cardinal,
	TypedArray<Vector3> intov,
	TypedArray<Vector2> intou,
	TypedArray<Vector3> inton,
	TypedArray<int32_t> intoi
) -> void
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
	if (Math::is_zero_approx(x1-x0) || Math::is_zero_approx(y1-y0)) {
		return;
	}
	
	auto q0 = Vector2(x0, y0);
	auto q1 = Vector2(x1, y1);
	auto depth = corners[faceData.depth] + ((faceData.zNeg==0) ? faceOffset : -faceOffset);
	auto trans = FACE_TRANSFORMS[cardinal].translated(cornerPostOffset);
	auto ntrans = trans.basis.orthonormalized();
	auto n = ntrans.xform(FACE_NORMALS[cardinal]);
	inton.append_array({n,n,n,n});

	auto uv0 = Vector2 {
		q0.x,
		1-q1.y,
	};
	auto uv1 = Vector2 {
		q1.x,
		1-q0.y,
	};

	intov.append_array({
		trans.xform(Vector3(q0.x, q0.y, depth)),
		trans.xform(Vector3(q0.x, q1.y, depth)),
		trans.xform(Vector3(q1.x, q1.y, depth)),
		trans.xform(Vector3(q1.x, q0.y, depth)),
	});
	intou.append_array({
		Vector2(uv0.x, uv1.y),
		Vector2(uv0.x, uv0.y),
		Vector2(uv1.x, uv0.y),
		Vector2(uv1.x, uv1.y),
	});
	intoi.append_array({0, 1, 2, 0, 2, 3});

	return;
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

auto CubeModelBuilder::
_bind_methods() -> void
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
		D_METHOD("create_face","cardinal","dst_vertices","dst_uvs","dst_normals","dst_indices"),
		&CubeModelBuilder::createFace
	);

}




