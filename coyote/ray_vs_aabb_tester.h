#pragma once

#include <core/object/ref_counted.h>
#include <core/variant/variant.h>
#include <core/math/aabb.h>
#include <core/math/vector3.h>

class RayVsAABB : public RefCounted {
	GDCLASS(RayVsAABB, RefCounted);
	
	struct HitData {
		int value;
		double xNormal;
		double yNormal;
		double zNormal;
		double nearTime;
		double farTime;
		double xHit;
		double yHit;
		double zHit;
	};
	
	public:
	enum Line {
		LINE_END_POINTS,
		LINE_ORIGIN_DIRECTION,
	};
	
	enum HitResult {
		HIT_POINT,
		HIT_NORMAL,
		HIT_TIME,
	};
	
	double xOrigin;
	double yOrigin;
	double zOrigin;
	
	double xEnd;
	double yEnd;
	double zEnd;
	
	double xDirection;
	double yDirection;
	double zDirection;
	
	double inverseXDirection;
	double inverseYDirection;
	double inverseZDirection;
	
	double directionLength;
	double inverseDirectionLength;
	
	double xDirectionNormal;
	double yDirectionNormal;
	double zDirectionNormal;
	
	//double xHit;
	//double yHit;
	//double zHit;
	
	bool xSignNegative;
	bool ySignNegative;
	bool zSignNegative;
	
	double xSign;
	double ySign;
	double zSign;
	
	bool didHit;
	HitData hit;
	
	//double nearTime;
	//double farTime;
	
	double xInflate;
	double yInflate;
	double zInflate;
	
	//double xNormal;
	//double yNormal;
	//double zNormal;
	
	double epsilon = 0.000001;
	
	bool includeSkinAsInside = true;
	
	auto test (
		double x0, double y0, double z0,
		double x1, double y1, double z1
	) -> bool;
	
	auto setupDirectionValues () -> void;
	
	auto setupRay (
		double xo, double yo, double zo,
		double xd, double yd, double zd,
		Line lineType = LINE_ORIGIN_DIRECTION
	) -> void {
		xOrigin = xo;
		yOrigin = yo;
		zOrigin = zo;
		
		if (lineType == LINE_END_POINTS)
		{
			xEnd = xd;
			yEnd = yd;
			zEnd = zd;

			xDirection = xd - xo;
			yDirection = yd - yo;
			zDirection = zd - zo;
		}
		else
		{
			xEnd = xo + xd;
			yEnd = yo + yd;
			zEnd = zo + zd;

			xDirection = xd;
			yDirection = yd;
			zDirection = zd;
		}
		setupDirectionValues();

		xInflate = 0.0;
		yInflate = 0.0;
		zInflate = 0.0;
	};
	
	auto setupRayv (
		const Vector3& a,
		const Vector3& b,
		Line lineType=LINE_ORIGIN_DIRECTION
	) -> void {
		setupRay(
			a.x, a.y, a.z,
			b.x, b.y, b.z,
			lineType
		);
	};
	
	auto setupTraceHull (
		const AABB& bbox,
		const Vector3& direction
	) -> void {
		const auto p0 = bbox.get_position();
		const auto p1 = bbox.get_end();
		
		setupRay(
			(p0.x + p1.x) * 0.5,
			(p0.y + p1.y) * 0.5,
			(p0.z + p1.z) * 0.5,
			direction.x,
			direction.y,
			direction.z
		);

		xInflate = (p1.x - p0.x) * 0.5;
		yInflate = (p1.y - p0.y) * 0.5;
		zInflate = (p1.z - p0.z) * 0.5;
	};
	
	auto get_epsilon () const -> double {
		return epsilon;
	};
	auto set_epsilon (const double to) -> void {
		epsilon = (to < 0.0) ? 0.000001 : to;
	};
	
	auto get_skin_inside () const -> bool {
		return includeSkinAsInside;
	};
	auto set_skin_inside (const bool to) -> void {
		includeSkinAsInside = to;
	};
	
	auto test_aabb (const AABB& obstacle, const Vector3& offset = Vector3(0,0,0)) -> bool {
		const auto p = obstacle.get_position();
		const auto k = obstacle.get_end();
		return test(
			p.x+offset.x,
			p.y+offset.y,
			p.z+offset.z,
			k.x+offset.x,
			k.y+offset.y,
			k.z+offset.z
		);
	};
	
	auto get_hit_result (HitResult type, double* x, double* y, double* z) const -> void {
		#define ASS(DST,V) if ((DST)!=nullptr) *(DST)=(V)
		switch (type) {
			case HIT_POINT:
				ASS(x, hit.xHit);
				ASS(y, hit.yHit);
				ASS(z, hit.zHit);
				break;
			case HIT_NORMAL:
				ASS(x, hit.xNormal);
				ASS(y, hit.yNormal);
				ASS(z, hit.zNormal);
				break;
			case HIT_TIME:
				ASS(x, hit.nearTime);
				ASS(y, hit.farTime);
				ASS(z, directionLength);
		};
		#undef ASS
	};
	
	auto get_hit_resultv (HitResult type) const -> Vector3 {
		double ox=0,oy=0,oz=0;
		get_hit_result(type, &ox, &oy, &oz);
		return Vector3(ox,oy,oz);
	};

	auto get_did_hit () const -> bool {
		return didHit;
	};
	
	static auto _bind_methods () -> void;
	
	
};

VARIANT_ENUM_CAST(RayVsAABB::Line);
VARIANT_ENUM_CAST(RayVsAABB::HitResult);
