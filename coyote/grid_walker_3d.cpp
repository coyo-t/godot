#include "grid_walker_3d.h"

#include<core/math/math_funcs.h>


static auto calc_iter_count_sep (
	double x,  double y,  double z,
	double xd, double yd, double zd
) -> int64_t {
	return (
		static_cast<int64_t>(Math::abs(floor(x+xd)-Math::floor(x)))+
		static_cast<int64_t>(Math::abs(floor(y+yd)-Math::floor(y)))+
		static_cast<int64_t>(Math::abs(floor(z+zd)-Math::floor(z)))+
		1
	);
};

auto GridSpaceWalker3D::update_params() -> void {
	constexpr auto INF = Math::INF;
	auto dpx = xDirection >= 0;
	auto dpy = yDirection >= 0;
	auto dpz = zDirection >= 0;
	
	int xStep = dpx ? +1 : -1;
	int yStep = dpy ? +1 : -1;
	int zStep = dpz ? +1 : -1;
	this->step_[0] = xStep;
	this->step_[1] = yStep;
	this->step_[2] = zStep;
	
	double xSlope = (Math::abs(xDirection) <= 0) ? INF : Math::abs(1.0/xDirection);
	double ySlope = (Math::abs(yDirection) <= 0) ? INF : Math::abs(1.0/yDirection);
	double zSlope = (Math::abs(zDirection) <= 0) ? INF : Math::abs(1.0/zDirection);
	this->slope[0] = xSlope;
	this->slope[1] = ySlope;
	this->slope[2] = zSlope;
	
	auto xCel = static_cast<int64_t>(Math::floor(xStart));
	auto yCel = static_cast<int64_t>(Math::floor(yStart));
	auto zCel = static_cast<int64_t>(Math::floor(zStart));
	this->cel_[0] = xCel;
	this->cel_[1] = yCel;
	this->cel_[2] = zCel;
	
	this->nextDistance[0] = xSlope*(dpx?(xCel+1-xStart):(xStart-xCel));
	this->nextDistance[1] = ySlope*(dpy?(yCel+1-yStart):(yStart-yCel));
	this->nextDistance[2] = zSlope*(dpz?(zCel+1-zStart):(zStart-zCel));

	iterationCount = calc_iter_count_sep(
		xStart, yStart, zStart,
		xDirection, yDirection, zDirection
	);
	
	this->xHitNormal = 0;
	this->yHitNormal = 0;
	this->zHitNormal = 0;
	this->hitTime = 1.0;
};

auto GridSpaceWalker3D::setup (
	double start_x, double start_y, double start_z,
	double end_x, double end_y, double end_z
) -> void {
	this->xStart = start_x;
	this->yStart = start_y;
	this->zStart = start_z;
	this->xEnd = end_x;
	this->yEnd = end_y;
	this->zEnd = end_z;
	this->xDirection = end_x-start_x;
	this->yDirection = end_y-start_y;
	this->zDirection = end_z-start_z;
	this->update_params();
};

auto GridSpaceWalker3D::advance () -> void {
	int axis;
	auto xd = nextDistance[0];
	auto yd = nextDistance[1];
	auto zd = nextDistance[2];
	
	if(xd<yd)
	{
		axis = (zd<xd) ? 2 : 0;
	}
	else
	{
		axis = (zd<yd) ? 2 : 1;
	}
	hitTime = nextDistance[axis];
	
	xHitNormal = (axis == 0) ? (-step_[0]) : 0;
	yHitNormal = (axis == 1) ? (-step_[1]) : 0;
	zHitNormal = (axis == 2) ? (-step_[2]) : 0;

	nextDistance[axis] += slope[axis];
	cel_[axis] += step_[axis];
};

auto GridSpaceWalker3D::_bind_methods () -> void {
	ClassDB::bind_static_method(
		"GridSpaceWalker3D",
		D_METHOD("calc_iter_count_sep", "x", "y", "z", "xd", "yd", "zd"),
		calc_iter_count_sep
	);
	
	ClassDB::bind_method(
		D_METHOD("reset"),
		&GridSpaceWalker3D::update_params
	);
	
	ClassDB::bind_method(
		D_METHOD("setup_endpoints", "start_x", "start_y", "start_z", "end_x", "end_y", "end_z"),
		&GridSpaceWalker3D::setup
	);
	
	ClassDB::bind_method(
		D_METHOD("setup_endpointsv", "start", "end"),
		&GridSpaceWalker3D::setup_endpointsv
	);
	
	ClassDB::bind_method(
		D_METHOD("get_hit_time"),
		&GridSpaceWalker3D::get_hit_time
	);
	
	ClassDB::bind_method(
		D_METHOD("get_hit_normal"),
		&GridSpaceWalker3D::get_hit_normal
	);
	
	ClassDB::bind_method(
		D_METHOD("get_iteration_count"),
		&GridSpaceWalker3D::get_iteration_count
	);
	
	ClassDB::bind_method(
		D_METHOD("get_current_cel"),
		&GridSpaceWalker3D::get_current_cel
	);
	
	ClassDB::bind_method(
		D_METHOD("advance"),
		&GridSpaceWalker3D::advance
	);
	/*
	ClassDB::bind_method(
		D_METHOD("get_then_advance"),
		&GridSpaceWalker3D::getThenAdvance
	);
	*/
};

