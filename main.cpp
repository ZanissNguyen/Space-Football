#include "main.h"

//The window we'll be rendering to
SDL_Window* window = NULL;

// Create renderer
SDL_Renderer* renderer = NULL;

int main(int argc, char* args[])
{
    int delay = (int)1000/FPS;
    game_initialize();

    Gameplay game;
    std::vector<Player*> players_red;
    players_red.push_back(new Player(0,0,RED));
    players_red.push_back(new Player(0,0,RED));
    std::vector<Player*> players_blue;
    players_blue.push_back(new Player(0,0,BLUE));
    players_blue.push_back(new Player(0,0,BLUE));
    // printf("game decleard!");
    game.init(EARTH, players_red, players_blue);
    game.mode = PVP;
    // printf("game init completely!");

    enum GAME_STATE state = GAME_STATE::PLAYING;
    bool running = true;
    SDL_Event event;

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    // Event Handler
    while (running)
    {
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) running = false;

            switch (state) {
                case MENU:    event_handler_menu(&event); break;
                case PLAYING: event_handler_playing(&event); break;
                case SCORING: event_handler_scoring(&event); break;
            }
        }

        // smooth moving
        if (state == PLAYING)
        {
            if (game.mode == PVP)
            {
                // printf("is it ok");
                if (key_state[SDL_SCANCODE_UP]) 
                    game.blue.members[game.blue.active_player]->acceleration.y -= BASE_ACCELERATION;
                if (key_state[SDL_SCANCODE_DOWN]) 
                    game.blue.members[game.blue.active_player]->acceleration.y += BASE_ACCELERATION;
                if (key_state[SDL_SCANCODE_LEFT]) 
                    game.blue.members[game.blue.active_player]->acceleration.x -= BASE_ACCELERATION;
                if (key_state[SDL_SCANCODE_RIGHT])
                    game.blue.members[game.blue.active_player]->acceleration.x += BASE_ACCELERATION;
            }

            if (key_state[SDL_SCANCODE_W]) 
                game.red.members[game.red.active_player]->acceleration.y -= BASE_ACCELERATION;
            if (key_state[SDL_SCANCODE_S]) 
                game.red.members[game.red.active_player]->acceleration.y += BASE_ACCELERATION;
            if (key_state[SDL_SCANCODE_A]) 
                game.red.members[game.red.active_player]->acceleration.x -= BASE_ACCELERATION;
            if (key_state[SDL_SCANCODE_D])
                game.red.members[game.red.active_player]->acceleration.x += BASE_ACCELERATION;

            // update game logic
            // goal check, moving, 
            game.process(delay/1000.0f);
        }

        // draws_things()
        SDL_RenderClear(renderer);
        switch (state)
        {
            case MENU:
            // event_handler_menu(&event);
            break;

            case PLAYING:
            draw_game(&game, window, renderer);
            // event_handler_playing(&event);
            break;

            case SCORING:
            // event_handler_scoring(&event);
            break;
            
            // case PAUSE:
            // // event_handler_pause(&event);
            // break;

            default:
            // do not thing
            break;
        }
        SDL_RenderPresent(renderer);

        SDL_Delay(delay); // ~60 FPS
        // printf("Printing Frame...");
    }

    //Free resources and close SDL
    close();

    return 0;
}

bool game_initialize()
{
    if(!library_init())
    {
        printf("Failed to initialize!\n");
        return false;
    }
    // else
    // {
    //     //Load media
    //     if( !load_media() )
    //     {
    //         printf( "Failed to load media!\n" );
    //     }
    // }
    return true;
}

bool library_init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO)<0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        //Create window
        window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }

        // Create renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
    }

    return success;
}

void close()
{
    //Destroy window
    SDL_DestroyWindow(window);
    window = NULL;

    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

void event_handler_menu(SDL_Event * event)
{

}

void event_handler_playing(SDL_Event* event)
{
    // Process Smooth Moving
    
}
void event_handler_pause(SDL_Event * event)
{

}
void event_handler_scoring(SDL_Event * event)
{

}


