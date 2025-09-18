#include "settings.h"
#include "utils.h"

Vec2 Vec2::operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
Vec2 Vec2::operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
Vec2 Vec2::operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }

Vec2& Vec2::operator+=(const Vec2& other) { x += other.x; y += other.y; return *this; }
Vec2& Vec2::operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }