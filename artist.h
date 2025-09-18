#pragma once
#include "settings.h"
#include "utils.h"
#include "gameplay.h"

SDL_Texture * getTexture(SDL_Window* window, SDL_Renderer* renderer, std::string path);
void draw_game(Gameplay * game, SDL_Window * window, SDL_Renderer * renderer);
void draw_player(Player* player, SDL_Window* window, SDL_Renderer* renderer);
void draw_field(SDL_Window * window, SDL_Renderer * renderer);
void draw_ball(Ball* ball,SDL_Window* window, SDL_Renderer * renderer);