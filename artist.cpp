#include "artist.h"

// this file draw things
void draw_game(Gameplay * game, SDL_Window * window, SDL_Renderer * renderer)
{
    draw_field(window, renderer);

    for (int i = 0; i<NUMBER_OF_PLAYER; i++)
    {
        draw_player(game->red.members[i], window, renderer);
        draw_player(game->blue.members[i], window, renderer);
    }

    draw_ball(&(game->ball), window, renderer);
}

SDL_Texture * getTexture(SDL_Window* window, SDL_Renderer* renderer, std::string path)
{   
     // Load BMP into surface
    SDL_Surface* surface = SDL_LoadBMP(path.c_str());
    if (!surface) {
        printf("ERROR CODE = %s\n", ERROR_CODE::IMAGE_NOT_FOUND);
        printf("Path = %s\n", path);
        printf("SDL ERROR CODE = %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface); // free surface after creating texture
    if (!texture) {
        printf("ERROR CODE = %s\n", ERROR_CODE::CANNOT_CREATE_TEXTURE);
        printf("SDL ERROR CODE = %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    return texture;
}

void draw_player(Player* player, SDL_Window* window, SDL_Renderer* renderer)
{
    std::ostringstream ospath;
    ospath << IMAGE_PATH << "earth_player_" << player->team << ".bmp";
    std::string path = ospath.str(); 
    
    SDL_Texture * texture = NULL;
    texture = getTexture(window, renderer, path);

    SDL_Rect * rect = &player->rect;
    SDL_RenderCopy(renderer, texture, NULL, rect);
    SDL_DestroyTexture(texture);
    texture = NULL;
}

void draw_field(SDL_Window * window, SDL_Renderer * renderer)
{
    std::ostringstream ospath;
    ospath << IMAGE_PATH << "background_earth.bmp";
    std::string path = ospath.str(); 
    
    SDL_Texture * texture = NULL;
    texture = getTexture(window, renderer, path);

    SDL_Rect rect = {0, 120, SCREEN_WIDTH, SCREEN_HEIGHT-120};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    texture = NULL;
}

void draw_ball(Ball* ball, SDL_Window* window, SDL_Renderer* renderer)
{
    std::ostringstream ospath;
    ospath << IMAGE_PATH << "ball_earth.bmp";
    std::string path = ospath.str(); 
    
    SDL_Texture * texture = NULL;
    texture = getTexture(window, renderer, path);

    SDL_Rect * rect = &ball->display_rect;
    SDL_RenderCopy(renderer, texture, NULL, rect);
    SDL_DestroyTexture(texture);
    texture = NULL;
}