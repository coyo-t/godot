#pragma once
#include<cstdint>
#include<core/variant/typed_array.h>
#include<core/object/class_db.h>
#include<core/object/ref_counted.h>
#include<core/math/vector2.h>
#include<core/math/vector3.h>
#include<core/math/aabb.h>

class CubeModelBuilder : public RefCounted {
	GDCLASS(CubeModelBuilder, RefCounted);

	public:
	AABB bbox {};
	Vector3 cornerPostOffset {};
	double faceOffset = 0.0;
	double faceInset = 0.0;

	public:
	auto createFace (
		int cardinal,
		TypedArray<Vector3> intov,
		TypedArray<Vector2> intou,
		TypedArray<Vector3> inton,
		TypedArray<int32_t> intoi
	) -> void;

	public: // bindingz
	auto get_bbox () const -> AABB;
	auto set_bbox (const AABB& b) -> void;

	auto get_corner_post_offset () const -> Vector3;
	auto set_corner_post_offset (const Vector3& v) -> void;

	auto get_face_offset () const -> double;
	auto set_face_offset (const double d) -> void;

	auto get_face_inset () const -> double;
	auto set_face_inset (const double d) -> void;

	public: // staticz
	static auto _bind_methods () -> void;
};

