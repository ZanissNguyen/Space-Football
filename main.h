//Using SDL and standard IO

#include "settings.h"
#include "gameplay.h"
#include "utils.h"
#include "artist.h"

int main(int argc, char* args[]);

bool game_initialize();
bool library_init();
void close();

void event_handler_menu(SDL_Event* event, GAME_STATE* state);
void event_handler_choose_map(SDL_Event* event, GAME_STATE* state);
void event_handler_playing(SDL_Event* event);
void event_handler_pause(SDL_Event * event);
void event_handler_scoring(SDL_Event * event);

void draw_menu(SDL_Window* window, SDL_Renderer* renderer);
void draw_choose_map(SDL_Window* window, SDL_Renderer* renderer);
void draw_pause(SDL_Window* window, SDL_Renderer* renderer, int selection);
void draw_result(SDL_Window* window, SDL_Renderer* renderer, int winner, int selection);
