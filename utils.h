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

class RendererManager {
public:
    static void init(const char* title, int w, int h);
    static void cleanup();

    static SDL_Renderer* getRenderer() { return renderer; }
    static SDL_Window* getWindow() { return window; }

private:
    static SDL_Window* window;
    static SDL_Renderer* renderer;
};

void init_random();
int random_int(int start, int end);