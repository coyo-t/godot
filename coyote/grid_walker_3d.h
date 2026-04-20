#pragma once

#include <cstdint>
#include <core/object/ref_counted.h>
#include <core/object/class_db.h>
#include <core/math/vector3.h>
#include <core/math/vector3i.h>


class GridSpaceWalker3D : public RefCounted {
	GDCLASS(GridSpaceWalker3D, RefCounted);
	
	protected:
	static auto _bind_methods() -> void;
	
	public:
	double xStart = 0.0;
	double yStart = 0.0;
	double zStart = 0.0;

	double xEnd = 0.0;
	double yEnd = 0.0;
	double zEnd = 0.0;

	double xDirection = 0.0;
	double yDirection = 0.0;
	double zDirection = 0.0;

	double xNextDistance = 0.0;
	double yNextDistance = 0.0;
	double zNextDistance = 0.0;

	double nextDistance[3] = { 0.0, 0.0, 0.0 };
	double slope[3] = { 1.0, 1.0, 1.0 };
	int64_t cel_[3] = { 0, 0, 0 };
	int64_t step_[3] = { 0, 0, 0 };

	int64_t iterationCount = -1;
	
	int xHitNormal = 0;
	int yHitNormal = 0;
	int zHitNormal = 0;
	
	double hitTime = 1.0;
	
	auto update_params () -> void;
	
	auto setup (
		double start_x, double start_y, double start_z,
		double end_x, double end_y, double end_z
	) -> void;
	
	auto advance () -> void;

	auto setup_endpointsv (const Vector3& start, const Vector3& end) -> void {
		this->setup(start.x, start.y, start.z, end.x, end.y, end.z);
	};
	
	auto get_iteration_count () const -> int64_t {
		return iterationCount;
	};
	
	auto get_hit_time () const -> double {
		return hitTime;
	};
	
	auto get_hit_normal () const -> Vector3i {
		return Vector3i(xHitNormal, yHitNormal, zHitNormal);
	};
	
	auto get_current_cel () const -> Vector3i {
		return Vector3i(cel_[0], cel_[1], cel_[2]);
	};
	
	
};

