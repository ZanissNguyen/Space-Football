#include "main.h"
#include <iomanip>

//The window we'll be rendering to
SDL_Window* window = NULL;

// Create renderer
SDL_Renderer* renderer = NULL;

Gameplay game; // Make game global for event handlers

// Menu state variables
GAME_MODE selected_mode = PVP;
GAME_MAP selected_map = EARTH;
int menu_selection = 0; // 0 = PvP, 1 = PvE
int map_selection = 0;  // 0 = Earth, 1 = Moon

int main(int argc, char* args[])
{
    int delay = (int)1000/FPS;
    game_initialize();

    enum GAME_STATE state = GAME_STATE::MENU;
    bool running = true;
    SDL_Event event;

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    // Event Handler

    int pause_selection = 0; // 0 = Resume, 1 = Quit to menu
    int result_selection = 0; // 0 = New Game, 1 = Quit to menu
    int winner = -1; // 0 = red, 1 = blue, 2 = tie
    // 0 = PvP, 1 = PvE, 2 = Exit

    while (running)
    {
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) running = false;

            switch (state) {
                case MENU:
                    if (event.type == SDL_KEYDOWN) {
                        if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {
                            menu_selection = (menu_selection + 2) % 3;
                        } else if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
                            menu_selection = (menu_selection + 1) % 3;
                        } else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
                            if (menu_selection == 0 || menu_selection == 1) {
                                selected_mode = (menu_selection == 0) ? PVP : PVE;
                                state = CHOOSE_MAP;
                            } else if (menu_selection == 2) {
                                running = false;
                            }
                        }
                    }
                    break;
                case CHOOSE_MAP: event_handler_choose_map(&event, &state); break;
                case PLAYING:
                    // Pause with ESC
                    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                        state = PAUSE;
                        pause_selection = 0;
                        break;
                    }
                    event_handler_playing(&event);
                    break;
                case PAUSE:
                    if (event.type == SDL_KEYDOWN) {
                        if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
                            pause_selection = 1 - pause_selection;
                        } else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
                            if (pause_selection == 0) {
                                state = PLAYING;
                            } else {
                                state = MENU;
                            }
                        }
                    }
                    break;
                case RESULT:
                    if (event.type == SDL_KEYDOWN) {
                        if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
                            result_selection = 1 - result_selection;
                        } else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
                            if (result_selection == 0) {
                                state = CHOOSE_MAP;
                            } else {
                                state = MENU;
                            }
                        }
                    }
                    break;
                case SCORING:
                    // event_handler_scoring(&event);
                    break;
            }
        }

        // smooth moving
        if (state == PLAYING)
        {
            if (game.mode == PVP)
            {
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
            game.process(delay/1000.0f);

            // Check for end of second half (game.half_time_break == false, game.half_time_remaining <= 0, current_half == 2)
            if (!game.half_time_break && game.half_time_remaining <= 0 && game.current_half == 2) {
                if (game.red.score > game.blue.score) winner = 0;
                else if (game.red.score < game.blue.score) winner = 1;
                else winner = 2;
                state = RESULT;
                result_selection = 0;
            }
        }

        // draws_things()
        SDL_RenderClear(renderer);
        switch (state)
        {
            case MENU:
                draw_menu(window, renderer);
                break;
            case CHOOSE_MAP:
                draw_choose_map(window, renderer);
                break;
            case PLAYING:
                draw_game(&game, window, renderer);
                break;
            case PAUSE:
                draw_pause(window, renderer, pause_selection);
                break;
            case RESULT:
                draw_result(window, renderer, winner, result_selection);
                break;
            case SCORING:
                // event_handler_scoring(&event);
                break;
            default:
                break;
        }
        SDL_RenderPresent(renderer);

        SDL_Delay(delay); // ~60 FPS
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

void event_handler_menu(SDL_Event * event, GAME_STATE* state)
{
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_UP:
            case SDLK_w:
                menu_selection = (menu_selection - 1 + 2) % 2; // Wrap around
                break;
            case SDLK_DOWN:
            case SDLK_s:
                menu_selection = (menu_selection + 1) % 2; // Wrap around
                break;
            case SDLK_RETURN:
            case SDLK_SPACE:
                selected_mode = (menu_selection == 0) ? PVP : PVE;
                *state = CHOOSE_MAP;
                break;
        }
    }
}

void event_handler_playing(SDL_Event* event)
{
    // Handle half-time break resume
    if (event->type == SDL_KEYDOWN && game.half_time_break) {
        game.resume_second_half();
        return;
    }

    // Process Smooth Moving

}
void event_handler_pause(SDL_Event * event)
{

}
void event_handler_choose_map(SDL_Event * event, GAME_STATE* state)
{
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_LEFT:
            case SDLK_a:
                map_selection = (map_selection - 1 + 2) % 2; // Wrap around
                break;
            case SDLK_RIGHT:
            case SDLK_d:
                map_selection = (map_selection + 1) % 2; // Wrap around
                break;
            case SDLK_RETURN:
            case SDLK_SPACE:
                {
                    selected_map = (map_selection == 0) ? EARTH : MOON;
                    // Initialize game with selected settings
                    std::vector<Player*> players_red;
                    players_red.push_back(new Player(0,0,RED));
                    players_red.push_back(new Player(0,0,RED));
                    std::vector<Player*> players_blue;
                    players_blue.push_back(new Player(0,0,BLUE));
                    players_blue.push_back(new Player(0,0,BLUE));
                    game.init(selected_map, players_red, players_blue);
                    game.mode = selected_mode;
                    *state = PLAYING;
                    break;
                }
            case SDLK_ESCAPE:
                *state = MENU;
                break;
        }
    }
}

void event_handler_scoring(SDL_Event * event)
{

}

void draw_menu(SDL_Window* window, SDL_Renderer* renderer)
{

    // Animated background using 80 BMP frames (0.1s per frame, 8s loop)
    static const int NUM_BG_FRAMES = 80;
    static SDL_Texture* bg_frames[NUM_BG_FRAMES] = {nullptr};
    static bool bg_loaded = false;
    if (!bg_loaded) {
        char path[256];
        for (int i = 0; i < NUM_BG_FRAMES; ++i) {
            snprintf(path, sizeof(path), "assets/main_menu_bg/ezgif-split/frame_%02d_delay-0.1s.bmp", i);
            bg_frames[i] = getTexture(window, renderer, path);
        }
        bg_loaded = true;
    }
    // Calculate current frame based on SDL_GetTicks()
    Uint32 ticks = SDL_GetTicks();
    int frame_idx = (int)((ticks / 100) % NUM_BG_FRAMES);
    SDL_Texture* bg_tex = bg_frames[frame_idx];
    if (bg_tex) {
        SDL_Rect bg_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, bg_tex, NULL, &bg_rect);
    } else {
        SDL_SetRenderDrawColor(renderer, 20, 20, 60, 255);
        SDL_RenderClear(renderer);
    }

    // Left-align: set a left margin
    const int left_margin = 80;
    // Title
    draw_text_white("SPACE FOOTBALL", left_margin, 150, window, renderer, 0.8f);

    // Menu options: 0 = PvP, 1 = PvE, 2 = Exit
    for (int i = 0; i < 3; ++i) {
        std::string text;
        if (i == 0) text = "PLAYER VS PLAYER";
        else if (i == 1) text = "PLAYER VS AI";
        else text = "EXIT";
        if (menu_selection == i) text = "> " + text;
        else text = "  " + text;
        draw_text_white(text, left_margin, 300 + i * 50, window, renderer, 0.5f);
    }

    // Instructions at the bottom
    const int bottom_margin = 70;
    int instruction_y2 = SCREEN_HEIGHT - bottom_margin;
    int instruction_y1 = instruction_y2 - 30; // 30px above
    draw_text_white("USE W/S OR UP/DOWN TO NAVIGATE", left_margin, instruction_y1, window, renderer, 0.3f);
    draw_text_white("PRESS ENTER OR SPACE TO SELECT", left_margin, instruction_y2, window, renderer, 0.3f);
}

void draw_choose_map(SDL_Window* window, SDL_Renderer* renderer)
{
    // Set dark blue background
    SDL_SetRenderDrawColor(renderer, 20, 20, 60, 255);
    SDL_RenderClear(renderer);

    // Draw title with white text
    draw_text_white("CHOOSE MAP", SCREEN_WIDTH/2 - 80, 150, window, renderer, 0.6f);

    // Draw map options with selection indicators using white text
    if (map_selection == 0) {
        // Highlight Earth
        draw_text_white("> EARTH", SCREEN_WIDTH/2 - 200, 300, window, renderer, 0.6f);
        draw_text_white("  MOON", SCREEN_WIDTH/2 + 100, 300, window, renderer, 0.6f);
    } else {
        // Highlight Moon
        draw_text_white("  EARTH", SCREEN_WIDTH/2 - 200, 300, window, renderer, 0.6f);
        draw_text_white("> MOON", SCREEN_WIDTH/2 + 100, 300, window, renderer, 0.6f);
    }

    // Draw map option images
    static SDL_Texture* earth_option_texture = nullptr;
    static SDL_Texture* moon_option_texture = nullptr;
    if (!earth_option_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "earth_option.bmp";
        earth_option_texture = getTexture(window, renderer, ospath.str());
    }
    if (!moon_option_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "moon_option.bmp";
        moon_option_texture = getTexture(window, renderer, ospath.str());
    }

    // Use the same (larger) size for both images and center them
    const int option_width = 320;
    const int option_height = 220;
    const int option_gap = 60;
    int total_width = option_width * 2 + option_gap;
    int start_x = (SCREEN_WIDTH - total_width) / 2;
    int y = 260;
    SDL_Rect earth_rect = {start_x, y, option_width, option_height};
    SDL_Rect moon_rect = {start_x + option_width + option_gap, y, option_width, option_height};

    // Draw images
    if (earth_option_texture) {
        SDL_RenderCopy(renderer, earth_option_texture, NULL, &earth_rect);
    }
    if (moon_option_texture) {
        SDL_RenderCopy(renderer, moon_option_texture, NULL, &moon_rect);
    }

    // Draw selection border
    // Draw thick white border for selected, gray for unselected
    auto draw_thick_border = [&](const SDL_Rect& rect, bool selected) {
        int thickness = 8;
        if (selected) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
            for (int i = 0; i < thickness; ++i) {
                SDL_Rect r = {rect.x - i, rect.y - i, rect.w + 2 * i, rect.h + 2 * i};
                SDL_RenderDrawRect(renderer, &r);
            }
        } else {
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Gray
            SDL_RenderDrawRect(renderer, &rect);
        }
    };
    draw_thick_border(earth_rect, map_selection == 0);
    draw_thick_border(moon_rect, map_selection == 1);

    // Draw instructions with white text
    draw_text_white("USE A/D OR LEFT/RIGHT TO NAVIGATE", SCREEN_WIDTH/2 - 210, 500, window, renderer, 0.3f);
    draw_text_white("PRESS ENTER OR SPACE TO SELECT", SCREEN_WIDTH/2 - 190, 530, window, renderer, 0.3f);
    draw_text_white("PRESS ESC TO GO BACK", SCREEN_WIDTH/2 - 130, 560, window, renderer, 0.3f);
}

void draw_pause(SDL_Window* window, SDL_Renderer* renderer, int selection)
{
    // Animated background using 68 BMP frames (0.1s per frame, 6.8s loop)
    static const int NUM_PAUSE_FRAMES = 68;
    static SDL_Texture* pause_frames[NUM_PAUSE_FRAMES] = {nullptr};
    static bool pause_bg_loaded = false;

    if (!pause_bg_loaded) {
        for (int i = 0; i < NUM_PAUSE_FRAMES; ++i) {
            std::ostringstream ospath;
            ospath << "assets/main_menu_bg/pause-frame/frame_" << std::setfill('0') << std::setw(2) << i << "_delay-0.1s.bmp";
            std::string path_str = ospath.str();

            // Check if file exists before loading
            FILE* file = fopen(path_str.c_str(), "r");
            if (file) {
                fclose(file);
                pause_frames[i] = getTexture(window, renderer, path_str);
            } else {
                pause_frames[i] = nullptr;
            }
        }
        pause_bg_loaded = true;
    }

    // Calculate current frame
    Uint32 ticks = SDL_GetTicks();
    int frame_idx = (int)((ticks / 100) % NUM_PAUSE_FRAMES);
    SDL_Texture* pause_bg_tex = pause_frames[frame_idx];

    if (pause_bg_tex) {
        SDL_Rect bg_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, pause_bg_tex, NULL, &bg_rect);
    } else {
        // Fallback to solid color if texture fails
        SDL_SetRenderDrawColor(renderer, 20, 20, 60, 255);
        SDL_RenderClear(renderer);
    }
    const int left_margin = 80;
    draw_text_white("PAUSED", left_margin, 180, window, renderer, 0.8f);
    if (selection == 0) {
        draw_text_white("> RESUME", left_margin, 300, window, renderer, 0.6f);
        draw_text_white("  QUIT TO MENU", left_margin, 370, window, renderer, 0.6f);
    } else {
        draw_text_white("  RESUME", left_margin, 300, window, renderer, 0.6f);
        draw_text_white("> QUIT TO MENU", left_margin, 370, window, renderer, 0.6f);
    }
    draw_text_white("UP/DOWN or W/S to move, ENTER/SPACE to select", left_margin, 500, window, renderer, 0.3f);
}

void draw_result(SDL_Window* window, SDL_Renderer* renderer, int winner, int selection)
{
    SDL_SetRenderDrawColor(renderer, 20, 20, 60, 255);
    SDL_RenderClear(renderer);
    if (winner == 0)
        draw_text_white("RED WINS!", SCREEN_WIDTH/2 - 90, 180, window, renderer, 0.8f);
    else if (winner == 1)
        draw_text_white("BLUE WINS!", SCREEN_WIDTH/2 - 100, 180, window, renderer, 0.8f);
    else
        draw_text_white("IT'S A TIE!", SCREEN_WIDTH/2 - 90, 180, window, renderer, 0.8f);
    char score_str[32];
    snprintf(score_str, sizeof(score_str), "RED: %d   BLUE: %d", game.red.score, game.blue.score);
    draw_text_white(score_str, SCREEN_WIDTH/2 - 110, 260, window, renderer, 0.6f);
    if (selection == 0) {
        draw_text_white("> NEW GAME", SCREEN_WIDTH/2 - 80, 350, window, renderer, 0.6f);
        draw_text_white("  QUIT TO MENU", SCREEN_WIDTH/2 - 80, 420, window, renderer, 0.6f);
    } else {
        draw_text_white("  NEW GAME", SCREEN_WIDTH/2 - 80, 350, window, renderer, 0.6f);
        draw_text_white("> QUIT TO MENU", SCREEN_WIDTH/2 - 80, 420, window, renderer, 0.6f);
    }
    draw_text_white("UP/DOWN or W/S to move, ENTER/SPACE to select", SCREEN_WIDTH/2 - 220, 520, window, renderer, 0.3f);
}


