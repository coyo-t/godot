#pragma once
#include<cstdint>
#include<core/string/ustring.h>
#include<core/math/vector3i.h>
#include<core/math/vector3.h>

namespace CD {
	constexpr int NORTH = 0;
	constexpr int EAST  = 1;
	constexpr int SOUTH = 2;
	constexpr int WEST  = 3;
	constexpr int DOWN  = 4;
	constexpr int UP    = 5;

	constexpr uint64_t BIT_NORTH = 1 << NORTH;
	constexpr uint64_t BIT_EAST  = 1 << EAST;
	constexpr uint64_t BIT_SOUTH = 1 << SOUTH;
	constexpr uint64_t BIT_WEST  = 1 << WEST;
	constexpr uint64_t BIT_DOWN  = 1 << DOWN;
	constexpr uint64_t BIT_UP    = 1 << UP;

	constexpr int IDENTITY[] = {
		NORTH,
		EAST,
		SOUTH,
		WEST,
		DOWN,
		UP,
	};

	constexpr int OPPOSITES[] = {
		SOUTH,
		WEST,
		NORTH,
		EAST,
		UP,
		DOWN,
	};

	constexpr int ROTATE_N_TO_E[] = {
		EAST,
		SOUTH,
		WEST,
		NORTH,
		DOWN,
		UP,
	};

	constexpr int ROTATE_N_TO_S[] = {
		SOUTH,
		WEST,
		NORTH,
		EAST,
		DOWN,
		UP,
	};

	constexpr int ROTATE_N_TO_W[] = {
		WEST,
		NORTH,
		EAST,
		SOUTH,
		DOWN,
		UP,
	};


	constexpr int ROTATION_MATRIX[] = {
		// NN NE NS NW ND NU
		// EN EE ES EW ED EU
		// SN SE SS SW SD SU
		// WN WE WS WW WD WU
		// DN DE DS DW DD DU
		// UN UE US UW UD UU
	};

	constexpr Vector3i NORMAL_I[] = {
		{  0,  0, -1 },
		{ +1,  0,  0 },
		{  0,  0, +1 },
		{ -1,  0,  0 },
		{  0, -1,  0 },
		{  0, +1,  0 },
	};

	constexpr uint64_t ALL_BITS = 0b111111;

	auto fromNormal (const Vector3& normal) -> int;
};


