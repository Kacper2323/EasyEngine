#pragma once

namespace MATH
{
	/*
	Returns value or min, max if value is not in min < value < max
	*/
	float clamp(float min, float value, float max)
	{
		if (value < min) { return min; }
		else if (value > max) { return max; }
		else { return value; };
	}
}