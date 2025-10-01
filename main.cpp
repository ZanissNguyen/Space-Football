#include "main.h"
#include <iomanip>

Gameplay game; // Make game global for event handlers
SDL_Window * window = NULL;
SDL_Renderer * renderer = NULL;

// Menu state variables
GAME_MODE selected_mode = PVP;
GAME_MAP selected_map = EARTH;
int menu_selection = 0; // 0 = PvP, 1 = PvE
int map_selection = 0;  // 0 = Earth, 1 = Moon
int player_selection = 0; // 0 = Striker, 1 = Defender
bool is_player_red_choosing = true;
std::vector<Player*> players_red;
std::vector<Player*> players_blue;
int red_slots[2] = {5, 5};
int blue_slots[2] = {5, 5}; // 5 = not chosen, 0 - 4 = chosen


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
                case CHOOSE_PLAYER: event_handler_choose_player(&event, &state); break;
                case SCORING: event_handler_scoring(&event, &state); break;
                case PLAYING:
                    // Pause with ESC
                    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                        state = PAUSE;
                        pause_selection = 0;
                        break;
                    }
                    event_handler_playing(&game, &event);
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
                                event_handler_scoring(&event, &state);
                                state = CHOOSE_MAP;
                            } else {
                                event_handler_scoring(&event, &state);
                                state = MENU;
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        // smooth moving
        if (state == PLAYING && !game.countdown_active)
        {
            if (game.mode == PVP)
            {
                if (!game.blue.members[game.blue.active_player]->is_stunned)
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
            }

            if (!game.red.members[game.red.active_player]->is_stunned)
            {
                if (key_state[SDL_SCANCODE_W])
                    game.red.members[game.red.active_player]->acceleration.y -= BASE_ACCELERATION;
                if (key_state[SDL_SCANCODE_S])
                    game.red.members[game.red.active_player]->acceleration.y += BASE_ACCELERATION;
                if (key_state[SDL_SCANCODE_A])
                    game.red.members[game.red.active_player]->acceleration.x -= BASE_ACCELERATION;
                if (key_state[SDL_SCANCODE_D])
                    game.red.members[game.red.active_player]->acceleration.x += BASE_ACCELERATION;
            }
        }

        // update game logic (always call to handle countdown)
        if (state == PLAYING) {
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
            case CHOOSE_PLAYER:
                draw_choose_player(window, renderer);
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
        RendererManager::init("Tiny Football", SCREEN_WIDTH, SCREEN_HEIGHT);
        window = RendererManager::getWindow();
        renderer = RendererManager::getRenderer();
    }

    return success;
}

void close()
{
    //Destroy window
    RendererManager::cleanup();

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

void event_handler_playing(Gameplay * game, SDL_Event* event)
{
    // Handle half-time break resume
    if (event->type == SDL_KEYDOWN && game->half_time_break) {
        game->resume_second_half();
        return;
    }

    // Process Smooth Moving
    if (event->type == SDL_KEYDOWN)
    {
        switch( event->key.keysym.sym )
        {
            case SDLK_f:
                game->red.change_control();
                break;
            case SDLK_RCTRL:
                if (game->mode == PVP)
                    game->blue.change_control();
                break;
            default:
                break;
        }
    }
    
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
//                    std::vector<Player*> players_red;
//                    players_red.push_back(new Speeder(0,0,RED));
//                    players_red.push_back(new Tackle(0,0,RED));
//                    std::vector<Player*> players_blue;
//                    players_blue.push_back(new Power_Shooter(0,0,BLUE));
//                    players_blue.push_back(new Shield(0,0,BLUE));
//                    game.init(selected_map, players_red, players_blue);
//                    game.mode = selected_mode;
                    // init game in choose player state
                    *state = CHOOSE_PLAYER;
                    break;
                }
            case SDLK_ESCAPE:
                *state = MENU;
                break;
        }
    }
}

void event_handler_choose_player(SDL_Event * event, GAME_STATE* state)
{
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_LEFT:
                player_selection = (player_selection - 1 + 5) % 5; // Wrap around
                break;
            case SDLK_a:
                player_selection = (player_selection - 1 + 5) % 5; // Wrap around
                break;
            case SDLK_RIGHT:
                player_selection = (player_selection + 1) % 5; // Wrap around
                break;
            case SDLK_d:
                player_selection = (player_selection + 1) % 5; // Wrap around
                break;
            case SDLK_RETURN:
            case SDLK_SPACE:
                {
                    if (is_player_red_choosing) {
                        if (red_slots[0] == 5) {
                            red_slots[0] = player_selection;
                        } else if (red_slots[1] == 5) {
                            red_slots[1] = player_selection;
                        } else {
                            break; // Already chosen
                        }
                    } else {
                        if (blue_slots[0] == 5) {
                            blue_slots[0] = player_selection;
                        } else if (blue_slots[1] == 5) {
                            blue_slots[1] = player_selection;
                        } else {
                            break; // Already chosen
                        }
                    }
                    switch (player_selection) {
                        case 0:
                            if (is_player_red_choosing) {
                                players_red.push_back(new Power_Shooter(0,0,RED));
                            } else {
                                players_blue.push_back(new Power_Shooter(0,0,BLUE));
                            }
                            break;
                        case 1:
                            if (is_player_red_choosing)
                                players_red.push_back(new Speeder(0,0,RED));
                            else
                                players_blue.push_back(new Speeder(0,0,BLUE));
                            break;
                        case 2:
                            if (is_player_red_choosing)
                                players_red.push_back(new Controller(0,0,RED));
                            else
                                players_blue.push_back(new Controller(0,0,BLUE));
                            break;
                        case 3:
                            if (is_player_red_choosing)
                                players_red.push_back(new Tackle(0,0,RED));
                            else
                                players_blue.push_back(new Tackle(0,0,BLUE));
                            break;
                        case 4:
                            if (is_player_red_choosing)
                                players_red.push_back(new Shield(0,0,RED));
                            else
                                players_blue.push_back(new Shield(0,0,BLUE));
                            break;
                    }
                    if (players_red.size() + players_blue.size() >= 4) {
                        game.init(selected_map, players_red, players_blue);
                        game.mode = selected_mode;
                        is_player_red_choosing = true;
                        *state = PLAYING;
                    } else {
                        is_player_red_choosing = !is_player_red_choosing;
                    }
                    break;
                }
        }
    }
    
}

void event_handler_scoring(SDL_Event * event, GAME_STATE* state)
{
    // Reset game state for new game
    game.red.score = 0;
    game.blue.score = 0;
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

void draw_choose_player(SDL_Window* window, SDL_Renderer* renderer)
{
    // Set dark blue background
    SDL_SetRenderDrawColor(renderer, 20, 20, 60, 255);
    SDL_RenderClear(renderer);

    SDL_Rect field_rect = {SCREEN_WIDTH/2 - 480, 110, 920, 410};
    SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255); // Dark Green
    SDL_RenderFillRect(renderer, &field_rect);
    SDL_SetRenderDrawColor(renderer, 20, 20, 60, 255);

    // Draw title with white text
    draw_text_white("CHOOSE PLAYER", SCREEN_WIDTH/2 - 150, 50, window, renderer, 0.6f);
    const int left_margin = 80;
    const int right_margin = 400;
    
    SDL_Rect power_shooter_rect = {SCREEN_WIDTH/2 - 500, 550, 160, 200};
    SDL_Rect speeder_rect = {SCREEN_WIDTH/2 - 300, 550, 160, 200};
    SDL_Rect controller_rect = {SCREEN_WIDTH/2 - 100, 550, 160, 200};
    SDL_Rect tackle_rect = {SCREEN_WIDTH/2 + 100, 550, 160, 200};
    SDL_Rect shield_rect = {SCREEN_WIDTH/2 + 300, 550, 160, 200};

    SDL_Rect red_slot_1 = {SCREEN_WIDTH/2 - 340, 150, 100, 120};
    SDL_Rect red_slot_2 = {SCREEN_WIDTH/2 - 340, 380, 100, 120};
    SDL_Rect blue_slot_1 = {SCREEN_WIDTH/2 + 200, 150, 100, 120};
    SDL_Rect blue_slot_2 = {SCREEN_WIDTH/2 + 200, 380, 100, 120};

    static SDL_Texture* power_shooter_red_texture = nullptr;
    static SDL_Texture* speeder_red_texture = nullptr;
    static SDL_Texture* controller_red_texture = nullptr;
    static SDL_Texture* tackle_red_texture = nullptr;
    static SDL_Texture* shield_red_texture = nullptr;
    static SDL_Texture* power_shooter_blue_texture = nullptr;
    static SDL_Texture* speeder_blue_texture = nullptr;
    static SDL_Texture* controller_blue_texture = nullptr;
    static SDL_Texture* tackle_blue_texture = nullptr;
    static SDL_Texture* shield_blue_texture = nullptr;
    if (!power_shooter_red_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "power_shooter_red.bmp";
        power_shooter_red_texture = getTexture(window, renderer, ospath.str());
    }
    if (!speeder_red_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "speeder_red.bmp";
        speeder_red_texture = getTexture(window, renderer, ospath.str());
    }
    if (!controller_red_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "controller_red.bmp";
        controller_red_texture = getTexture(window, renderer, ospath.str());
    }
    if (!tackle_red_texture) {  
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "tackle_red.bmp";
        tackle_red_texture = getTexture(window, renderer, ospath.str());
    }
    if (!shield_red_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "shield_red.bmp";
        shield_red_texture = getTexture(window, renderer, ospath.str());
    }
    if (!power_shooter_blue_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "power_shooter_blue.bmp";
        power_shooter_blue_texture = getTexture(window, renderer, ospath.str());
    }
    if (!speeder_blue_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "speeder_blue.bmp";
        speeder_blue_texture = getTexture(window, renderer, ospath.str());
    }
    if (!controller_blue_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "controller_blue.bmp";
        controller_blue_texture = getTexture(window, renderer, ospath.str());
    }   
    if (!tackle_blue_texture) {  
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "tackle_blue.bmp";
        tackle_blue_texture = getTexture(window, renderer, ospath.str());
    }
    if (!shield_blue_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "shield_blue.bmp";
        shield_blue_texture = getTexture(window, renderer, ospath.str());
    }

    if (power_shooter_red_texture && power_shooter_blue_texture) {
        SDL_RenderCopy(renderer, is_player_red_choosing ? power_shooter_red_texture : power_shooter_blue_texture, NULL, &power_shooter_rect);
    }
    if (speeder_red_texture && speeder_blue_texture) {
        SDL_RenderCopy(renderer, is_player_red_choosing ? speeder_red_texture : speeder_blue_texture, NULL, &speeder_rect);
    }
    if (controller_red_texture && controller_blue_texture) {
        SDL_RenderCopy(renderer, is_player_red_choosing ? controller_red_texture : controller_blue_texture, NULL, &controller_rect);
    }
    if (tackle_red_texture && tackle_blue_texture) {
        SDL_RenderCopy(renderer, is_player_red_choosing ? tackle_red_texture : tackle_blue_texture, NULL, &tackle_rect);
    }
    if (shield_red_texture && shield_blue_texture) {
        SDL_RenderCopy(renderer, is_player_red_choosing ? shield_red_texture : shield_blue_texture, NULL, &shield_rect);
    }


    auto draw_thick_border = [&](const SDL_Rect& rect, bool selected, int thickness) {
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
    draw_thick_border(power_shooter_rect, player_selection == 0, 8);
    draw_thick_border(speeder_rect, player_selection == 1, 8);
    draw_thick_border(controller_rect, player_selection == 2, 8);
    draw_thick_border(tackle_rect, player_selection == 3, 8);
    draw_thick_border(shield_rect, player_selection == 4, 8);
    draw_thick_border(red_slot_1, true, 2);
    draw_thick_border(red_slot_2, true, 2);
    draw_thick_border(blue_slot_1, true, 2);
    draw_thick_border(blue_slot_2, true, 2);
    


    std::string text = "v";
    if (red_slots[0] == 5) {
        draw_text_white(text, red_slot_1.x + 40, red_slot_1.y - 40, window, renderer, 0.5f);
    } else if (blue_slots[0] == 5) {
        draw_text_white(text, blue_slot_1.x + 40, blue_slot_1.y - 40, window, renderer, 0.5f);
    } else if (red_slots[1] == 5) {
        draw_text_white(text, red_slot_2.x + 40, red_slot_2.y - 40, window, renderer, 0.5f);
    } else if (blue_slots[1] == 5) {
        draw_text_white(text, blue_slot_2.x + 40, blue_slot_2.y - 40, window, renderer, 0.5f);
    }

    switch (red_slots[0]) {
        case 0: 
            SDL_RenderCopy(renderer, power_shooter_red_texture, NULL, &red_slot_1);
            break;
        case 1:
            SDL_RenderCopy(renderer, speeder_red_texture, NULL, &red_slot_2);
            break;
        case 2:
            SDL_RenderCopy(renderer, controller_red_texture, NULL, &red_slot_1);
            break;
        case 3:
            SDL_RenderCopy(renderer, tackle_red_texture, NULL, &red_slot_1);
            break;
        case 4:
            SDL_RenderCopy(renderer, shield_red_texture, NULL, &red_slot_1);
            break;
    }
    switch (red_slots[1]) {
        case 0: 
            SDL_RenderCopy(renderer, power_shooter_red_texture, NULL, &red_slot_2);
            break;
        case 1:
            SDL_RenderCopy(renderer, speeder_red_texture, NULL, &red_slot_2);
            break;
        case 2:
            SDL_RenderCopy(renderer, controller_red_texture, NULL, &red_slot_2);
            break;
        case 3:
            SDL_RenderCopy(renderer, tackle_red_texture, NULL, &red_slot_2);
            break;
        case 4:
            SDL_RenderCopy(renderer, shield_red_texture, NULL, &red_slot_2);
            break;
    }
    switch (blue_slots[0]) {
        case 0: 
            SDL_RenderCopy(renderer, power_shooter_blue_texture, NULL, &blue_slot_1);
            break;
        case 1:
            SDL_RenderCopy(renderer, speeder_blue_texture, NULL, &blue_slot_1);
            break;
        case 2: 
            SDL_RenderCopy(renderer, controller_blue_texture, NULL, &blue_slot_1);
            break;
        case 3:
            SDL_RenderCopy(renderer, tackle_blue_texture, NULL, &blue_slot_1);
            break;
        case 4:
            SDL_RenderCopy(renderer, shield_blue_texture, NULL, &blue_slot_1);
            break;
        
    }
    switch (blue_slots[1]) {
        case 0: 
            SDL_RenderCopy(renderer, power_shooter_blue_texture, NULL, &blue_slot_2);
            break;
        case 1:
            SDL_RenderCopy(renderer, speeder_blue_texture, NULL, &blue_slot_2);
            break;
        case 2: 
            SDL_RenderCopy(renderer, controller_blue_texture, NULL, &blue_slot_2);
            break;
        case 3:
            SDL_RenderCopy(renderer, tackle_blue_texture, NULL, &blue_slot_2);
            break;
        case 4:
            SDL_RenderCopy(renderer, shield_blue_texture, NULL, &blue_slot_2);
            break;    
    }

    // Instructions at the bottom
    const int bottom_margin = 70;
    int instruction = SCREEN_HEIGHT - bottom_margin;
    draw_text_white("USE A/D OR LEFT/RIGHT TO NAVIGATE", left_margin, instruction, window, renderer, 0.3f);
    draw_text_white("PRESS ENTER OR SPACE TO SELECT", SCREEN_WIDTH - right_margin, instruction, window, renderer, 0.3f);
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


