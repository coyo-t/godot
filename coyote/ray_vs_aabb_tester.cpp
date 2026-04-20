
#include "ray_vs_aabb_tester.h"
// #include "fpw/mth.h"
#include <core/math/math_funcs.h>
#include <core/object/class_db.h>
#include <core/variant/variant.h>
#include <core/typedefs.h>

static auto directionComponent (double self) -> double {
	// abs should account for +0 and -0?
	return (Math::abs(self+0.0) == 0.0)
		? Math::INF
		: (1.0 / self);
};

auto RayVsAABB::setupDirectionValues () -> void {
	xSignNegative = xDirection < 0;
	ySignNegative = yDirection < 0;
	zSignNegative = zDirection < 0;

	xSign = xSignNegative ? (-1) : (+1);
	ySign = ySignNegative ? (-1) : (+1);
	zSign = zSignNegative ? (-1) : (+1);

	inverseXDirection = directionComponent(xDirection);
	inverseYDirection = directionComponent(yDirection);
	inverseZDirection = directionComponent(zDirection);

	// adding 0 should remove anything dumb like -0?
	double mag = 0.0 + (
		Math::pow(xDirection,2.0) +
		Math::pow(yDirection,2.0) +
		Math::pow(zDirection,2.0)
	);

	if (mag <= 0)
	{
		directionLength = 0.0;
		inverseDirectionLength = Math::INF;
		xDirectionNormal = 0.0;
		yDirectionNormal = 0.0;
		zDirectionNormal = 0.0;
	}
	else if (mag == 1.0)
	{
		directionLength = 1.0;
		inverseDirectionLength = 1.0;
		xDirectionNormal = xDirection;
		yDirectionNormal = yDirection;
		zDirectionNormal = zDirection;
	}
	else
	{
		directionLength = Math::sqrt(mag);
		inverseDirectionLength = 1.0 / directionLength;
		xDirectionNormal *= inverseDirectionLength;
		yDirectionNormal *= inverseDirectionLength;
		zDirectionNormal *= inverseDirectionLength;
	}
};

auto RayVsAABB::test (
	double x0_, double y0_, double z0_,
	double x1_, double y1_, double z1_
) -> bool {
	didHit = false;

	auto x0 = x0_ - xInflate;
	auto y0 = y0_ - yInflate;
	auto z0 = z0_ - zInflate;
	auto x1 = x1_ + xInflate;
	auto y1 = y1_ + yInflate;
	auto z1 = z1_ + zInflate;
	
	bool isInside;
	
	if (includeSkinAsInside) {
		isInside = (
			(x0 <= xOrigin && xOrigin <= x1) &&
			(y0 <= yOrigin && yOrigin <= y1) &&
			(z0 <= zOrigin && zOrigin <= z1)
		);
	}
	else
	{
		isInside = (
			(x0 < xOrigin && xOrigin < x1) &&
			(y0 < yOrigin && yOrigin < y1) &&
			(z0 < zOrigin && zOrigin < z1)
		);
	}

	if (isInside)
	{
		return false;
	}
	x0 -= epsilon;
	y0 -= epsilon;
	z0 -= epsilon;
	x1 += epsilon;
	y1 += epsilon;
	z1 += epsilon;

	double t1, t2;

	t1 = (((xSignNegative) ? x1 : x0) - xOrigin) * inverseXDirection;
	t2 = (((xSignNegative) ? x0 : x1) - xOrigin) * inverseXDirection;

	double hxmin = MIN(t1, t2);

	double tmin = hxmin;
	double tmax = MAX(t1, t2);

	t1 = (((ySignNegative) ? y1 : y0) - yOrigin) * inverseYDirection;
	t2 = (((ySignNegative) ? y0 : y1) - yOrigin) * inverseYDirection;

	double hymin = MIN(t1, t2);

	tmin = MAX(tmin, hymin);
	tmax = MIN(tmax, MAX(t1, t2));


	t1 = (((zSignNegative) ? z1 : z0) - zOrigin) * inverseZDirection;
	t2 = (((zSignNegative) ? z0 : z1) - zOrigin) * inverseZDirection;

	double hzmin = MIN(t1, t2);

	tmin = MAX(tmin, hzmin);
	tmax = MIN(tmax, MAX(t1, t2));
	
	double nearTime = MAX(tmin, 0.0);
	double farTime  = tmax;

	int axis = 0;
	
	if (hxmin > hymin) {
		axis = (hzmin > hxmin) ? 2 : 0;
	}
	else {
		axis = (hzmin > hymin) ? 2 : 1;
	}

	double xNormal = (axis == 0)?(-xSign):0.0;
	double yNormal = (axis == 1)?(-ySign):0.0;
	double zNormal = (axis == 2)?(-zSign):0.0;

	hit.nearTime = nearTime;
	hit.farTime = farTime;
	hit.xNormal = xNormal;
	hit.yNormal = yNormal;
	hit.zNormal = zNormal;
	
	didHit = (farTime > nearTime) && (1.0 > nearTime) && (farTime > 0.0);

	if (didHit) {
		hit.xHit = xDirection * nearTime + xOrigin;
		hit.yHit = yDirection * nearTime + yOrigin;
		hit.zHit = zDirection * nearTime + zOrigin;
	}
	return didHit;
}

auto RayVsAABB::_bind_methods () -> void {
	ClassDB::bind_method(
		D_METHOD("setup_direction_values"),
		&RayVsAABB::setupDirectionValues
	);
	
	ClassDB::bind_method(
		D_METHOD(
			"setup_tracer",
			"xa", "ya", "za",
			"xb", "yb", "zb",
			"line_type"
		),
		&RayVsAABB::setupRay,
		DEFVAL(LINE_ORIGIN_DIRECTION)
	);

	ClassDB::bind_method(
		D_METHOD(
			"setup_tracerv",
			"a", "b",
			"line_type"
		),
		&RayVsAABB::setupRayv,
		DEFVAL(LINE_ORIGIN_DIRECTION)
	);


	ClassDB::bind_method(
		D_METHOD(
			"setup_tracer_hull",
			"aabb", "direction"
		),
		&RayVsAABB::setupTraceHull
	);

	ClassDB::bind_method(
		D_METHOD(
			"test",
			"start_x", "start_y", "start_z",
			"end_x", "end_y", "end_z"
		),
		&RayVsAABB::test
	);

	ClassDB::bind_method(
		D_METHOD(
			"test_aabb",
			"obstacle",
			"obstacle_offset"
		),
		&RayVsAABB::test_aabb,
		DEFVAL(Vector3(0,0,0))
	);

	BIND_ENUM_CONSTANT(LINE_END_POINTS);
	BIND_ENUM_CONSTANT(LINE_ORIGIN_DIRECTION);
	BIND_ENUM_CONSTANT(HIT_POINT);
	BIND_ENUM_CONSTANT(HIT_NORMAL);
	BIND_ENUM_CONSTANT(HIT_TIME);

	ClassDB::bind_method(
		D_METHOD("get_hit_resultv", "type"),
		&RayVsAABB::get_hit_resultv
	);
	ClassDB::bind_method(
		D_METHOD("set_epsilon", "to"),
		&RayVsAABB::set_epsilon
	);
	ClassDB::bind_method(
		D_METHOD("get_epsilon"),
		&RayVsAABB::get_epsilon
	);
	ADD_PROPERTY(
		PropertyInfo(Variant::FLOAT, "epsilon"),
		"set_epsilon",
		"get_epsilon"
	);

	ClassDB::bind_method(
		D_METHOD("get_did_hit"),
		&RayVsAABB::get_did_hit
	);
	ADD_PROPERTY(
		PropertyInfo(Variant::BOOL, "did_hit"),
		"",
		"get_did_hit"
	);

	ClassDB::bind_method(
		D_METHOD("set_skin_inside", "to"),
		&RayVsAABB::set_skin_inside
	);
	ClassDB::bind_method(
		D_METHOD("get_skin_inside"),
		&RayVsAABB::get_skin_inside
	);
	ADD_PROPERTY(
		PropertyInfo(Variant::BOOL, "consider_surface_as_inside"),
		"set_skin_inside",
		"get_skin_inside"
	);
};