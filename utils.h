#pragma once

#include "settings.h"

class Vec2 {
public:    
    float x, y;

    Vec2(float x = 0, float y = 0) : x(x), y(y) {}

    Vec2 operator+(const Vec2& other) const; 
    Vec2 operator-(const Vec2& other) const; 
    Vec2 operator*(float scalar) const;

    Vec2& operator+=(const Vec2& other); 
    Vec2& operator*=(float scalar);

    float magnitude();
    Vec2 normalize();
};

float dot(const Vec2& a, const Vec2& b);

Vec2 rotate(const Vec2& v, float degrees);

class Circle {
public:
    float x,y,r;

    Circle(float x=0, float y=0, float r=0) : x(x), y(y), r(r) {}
};

template <typename T>
T clamp(T value, T minVal, T maxVal) {
    return std::max(minVal, std::min(value, maxVal));
}

SDL_Texture * getTexture(SDL_Window* window, SDL_Renderer* renderer, std::string path);