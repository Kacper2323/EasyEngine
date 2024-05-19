#include "Vec2.h"


Vec2::Vec2() {};

Vec2::Vec2(float xin, float yin)
	:x(xin), y(yin)
{};

float Vec2::length() const
{
	return sqrt((x * x) + (y * y));
}

float Vec2::dist(const Vec2& v) const
{
	float dx = v.x - x;
	float dy = v.y - y;
	return sqrt((dx * dx) + (dy * dy));
}

Vec2 Vec2::norm() const
{
	float len = (*this).length();
	return Vec2(x / len, y / len);
}

bool Vec2::operator == (const Vec2& v)
{
	return (x == v.x) && (y == v.y);
}

bool Vec2::operator != (const Vec2& v)
{
	return (x != v.x) || (y != v.y);
}

Vec2 Vec2::operator + (const Vec2& v)
{
	return Vec2(x + v.x, y + v.y);
}

Vec2 Vec2::operator - (const Vec2& v)
{
	return Vec2(x - v.x, y - v.y);
}

Vec2 Vec2::operator * (const float s)
{
	return Vec2(x * s, y * s);
}

Vec2 Vec2::operator / (const float s)
{
	return Vec2(x / s, y / s);
}

void Vec2::operator += (const Vec2& v)
{
	x += v.x;
	y += v.y;
}

void Vec2::operator -= (const Vec2& v)
{
	x -= v.x;
	y -= v.y;
}

void Vec2::operator *= (const Vec2& v)
{
	x *= v.x;
	y *= v.y;
}

void Vec2::operator /= (const Vec2& v)
{
	x /= v.x;
	y /= v.y;
}