#include "cardinals.h"
#include<core/math/math_funcs.h>

const double H = Math::sqrt(0.5);

auto CD::fromNormal(const Vector3 & normal) -> int
{
	if (normal.is_zero_approx())
		return UP;
	if (normal.y >= H)
		return UP;
	if (normal.y < -H)
		return DOWN;
	if (Math::abs(normal.x) >= Math::abs(normal.z))
	{
		if (normal.x < 0)
			return WEST;
		else
			return EAST;
	}
	else
	{
		if (normal.z < 0)
			return NORTH;
		else
			return SOUTH;
	}
}