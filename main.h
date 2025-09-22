//Using SDL and standard IO

#include "settings.h"
#include "gameplay.h"
#include "utils.h"
#include "artist.h"

int main(int argc, char* args[]);

bool game_initialize();
bool library_init();
void close();

void event_handler_menu(SDL_Event* event);
void event_handler_playing(Gameplay* game, SDL_Event* event);
void event_handler_pause(SDL_Event * event);
void event_handler_scoring(SDL_Event * event);
