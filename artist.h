#pragma once
#include "settings.h"
#include "utils.h"
#include "gameplay.h"

SDL_Texture * getTexture(SDL_Window* window, SDL_Renderer* renderer, std::string path);
void draw_game(Gameplay * game, SDL_Window * window, SDL_Renderer * renderer);
void draw_player(Player* player, SDL_Window* window, SDL_Renderer* renderer);
void draw_limb(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture,
               int src_x, int src_y, int src_w, int src_h,
               int dst_x, int dst_y, int dst_w, int dst_h,
               double angle, SDL_RendererFlip flip);
void draw_field(SDL_Window * window, SDL_Renderer * renderer);
void draw_ball(Ball* ball,SDL_Window* window, SDL_Renderer * renderer);
void draw_goals(SDL_Window* window, SDL_Renderer* renderer);