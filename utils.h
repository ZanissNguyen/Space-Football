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
};

class Circle {
public:
    float x,y,r;

    Circle(float x=0, float y=0, float r=0) : x(x), y(y), r(r) {}
};

SDL_Texture * getTexture(SDL_Window* window, SDL_Renderer* renderer, std::string path);