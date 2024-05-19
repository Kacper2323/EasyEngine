#pragma once

#include <math.h>

/*
Vec2 class. Stores x and y values as floats. Operators return copies.
*/
class Vec2
{
public:
	float x = 0;
	float y = 0;

	Vec2();
	Vec2(float xin, float yin);

	bool operator == (const Vec2& v);
	bool operator != (const Vec2& v);

	Vec2 operator + (const Vec2& v);
	Vec2 operator - (const Vec2& v);
	Vec2 operator * (const float s);
	Vec2 operator / (const float s);

	void operator += (const Vec2& v);
	void operator -= (const Vec2& v);
	void operator *= (const Vec2& v);
	void operator /= (const Vec2& v);
	
	/*
	Return the length of the vector
	*/
	float length() const;

	/*
	Return distance from one point to another
	*/
	float dist(const Vec2& v) const;

	/*
	Normalize the vector
	*/
	Vec2 norm() const;

};
