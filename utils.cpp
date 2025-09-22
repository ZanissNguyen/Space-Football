#include "settings.h"
#include "utils.h"

Vec2 Vec2::operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
Vec2 Vec2::operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
Vec2 Vec2::operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }

Vec2& Vec2::operator+=(const Vec2& other) { x += other.x; y += other.y; return *this; }
Vec2& Vec2::operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }

Vec2 Vec2::normalize()
{
    float len = magnitude();
    if (len == 0) return Vec2(0,0);
    return Vec2(x/len, y/len);
}

float Vec2::magnitude()
{
    return sqrt(x*x + y*y);
}

float dot(const Vec2& a, const Vec2&b)
{
    return a.x*b.x + a.y*b.y;
}