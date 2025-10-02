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

Vec2 rotate(const Vec2&v, float degrees)
{
    float radians = degrees * M_PI / 180.0f; // convert to radians
    float cosA = std::cos(radians);
    float sinA = std::sin(radians);

    return Vec2(
        v.x * cosA - v.y * sinA,
        v.x * sinA + v.y * cosA
    );
}

SDL_Window* RendererManager::window = nullptr;
SDL_Renderer* RendererManager::renderer = nullptr;

void RendererManager::init(const char* title, int w, int h) {
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void RendererManager::cleanup() {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;

    SDL_DestroyWindow(window);
    window = nullptr;

}

void init_random() {
    srand(static_cast<unsigned>(time(0)));
}

int random_int(int start, int end)
{
    int result = start + rand() % (end - start + 1);;
    return result;
}

