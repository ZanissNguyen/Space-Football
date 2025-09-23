#include "main.h"

//The window we'll be rendering to
SDL_Window* window = NULL;

// Create renderer
SDL_Renderer* renderer = NULL;

TTF_Font* font = NULL;

SDL_Color white = {255, 255, 255, 255};
SDL_Color black = {0, 0, 0, 255};

void render_text(SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color)
{
    if(font == NULL) return;
    
    // Create text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, color);
    if(textSurface == NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return;
    }
    
    // Create texture from surface
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if(textTexture == NULL)
    {
        printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }
    
    // Get text dimensions
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_FreeSurface(textSurface);
    
    // Render text
    SDL_Rect renderQuad = {x, y, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
    
    // Clean up
    SDL_DestroyTexture(textTexture);
}


void render_choose_map(SDL_Renderer* renderer) {

    // Clear screen with a background color
    SDL_SetRenderDrawColor(renderer, 0, 0, 100, 255); // Dark blue background
    SDL_RenderClear(renderer);
    
    SDL_Rect modeRect = { SCREEN_WIDTH/2 - 150, 100, 300, 60 };
    SDL_SetRenderDrawColor(renderer, 50, 50, 150, 255); // Lighter blue for title box
    SDL_RenderFillRect(renderer, &modeRect);

    SDL_Rect backButton = { SCREEN_WIDTH/2 - 450, 100, 100, 60 };
    SDL_SetRenderDrawColor(renderer, 50, 50, 150, 255); // Lighter blue for title box
    SDL_RenderFillRect(renderer, &backButton);
    
    SDL_Rect earthButton = { SCREEN_WIDTH/2 - 100, 250, 200, 50 };
    SDL_Rect moonButton = { SCREEN_WIDTH/2 - 100, 320, 200, 50 };
    SDL_Rect spaceButton = { SCREEN_WIDTH/2 - 100, 390, 200, 50 };
    
    int selectedOption = 0; // You should define this as a persistent variable
    
    // Draw buttons with different colors based on selection
    if (selectedOption == 0) {
        SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255); // Highlight color
    } else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255); // Normal color
    }
    SDL_RenderFillRect(renderer, &earthButton);


    if (selectedOption == 1) {
        SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
    }
    SDL_RenderFillRect(renderer, &moonButton);

    if (selectedOption == 2) {
        SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
    }
    SDL_RenderFillRect(renderer, &spaceButton);

    // Present the renderer
    SDL_RenderPresent(renderer);
}


void render_main_menu(SDL_Renderer* renderer) {
    // Clear screen with a background color
    SDL_SetRenderDrawColor(renderer, 0, 0, 100, 255); // Dark blue background
    SDL_RenderClear(renderer);
    
    SDL_Rect titleRect = { SCREEN_WIDTH/2 - 150, 100, 300, 60 };
    SDL_SetRenderDrawColor(renderer, 50, 50, 150, 255); // Lighter blue for title box
    SDL_RenderFillRect(renderer, &titleRect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White border
    SDL_RenderDrawRect(renderer, &titleRect);

    render_text(renderer, "Space Football", SCREEN_WIDTH/2 - 80, 120, white);
    
    SDL_Rect pveButton = { SCREEN_WIDTH/2 - 100, 250, 200, 50 };
    SDL_Rect pvpButton = { SCREEN_WIDTH/2 - 100, 390, 200, 50 };
    
    int selectedOption = 0; // You should define this as a persistent variable
    
    // Draw buttons with different colors based on selection
    if (selectedOption == 0) {
        SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255); // Highlight color
    } else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255); // Normal color
    }
    SDL_RenderFillRect(renderer, &pveButton);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White border
    SDL_RenderDrawRect(renderer, &pveButton);
    render_text(renderer, "PVE", SCREEN_WIDTH/2 - 20, 265, white);



    if (selectedOption == 1) {
        SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
    }
    SDL_RenderFillRect(renderer, &pvpButton);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White border
    SDL_RenderDrawRect(renderer, &pvpButton);
    render_text(renderer, "PVP", SCREEN_WIDTH/2 - 20, 405, white);
 
    // Present the renderer
    SDL_RenderPresent(renderer);
}

void render_game_over(SDL_Renderer* renderer, Gameplay& game) {
    // Clear screen with a background color
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); // Semi-transparent black overlay
    SDL_RenderClear(renderer);
    
    SDL_Rect gameOverRect = { SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 350, 300, 100 };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for game over box
    SDL_RenderFillRect(renderer, &gameOverRect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White border
    SDL_RenderDrawRect(renderer, &gameOverRect);
    render_text(renderer, "Game Over", SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2 - 320, white);


    SDL_Rect redScore = { SCREEN_WIDTH/2 - 450, 250, 300, 50 };
    SDL_Rect blueScore = { SCREEN_WIDTH/2 + 150, 250, 300, 50 };
    
    int selectedOption = 0; // You should define this as a persistent variable
    
    // Draw buttons with different colors based on selection

    SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255); // Highlight color
    
    SDL_RenderFillRect(renderer, &redScore);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White border
    SDL_RenderDrawRect(renderer, &redScore);
    std::string redScoreText = "Red Score = " + std::to_string(game.red.score);
    render_text(renderer, redScoreText.c_str(), SCREEN_WIDTH/2 - 420, 265, white);

    SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255);

    SDL_RenderFillRect(renderer, &blueScore);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White border
    SDL_RenderDrawRect(renderer, &blueScore);
    std::string blueScoreText = "Blue Score = " + std::to_string(game.blue.score);
    render_text(renderer, blueScoreText.c_str(), SCREEN_WIDTH/2 + 180, 265, white);
    
    SDL_Rect tryAgainButton = { SCREEN_WIDTH/2 - 100, 390, 200, 50 };
    SDL_Rect mainMenuButton = { SCREEN_WIDTH/2 - 100, 490, 200, 50 };

    
    // Draw buttons with different colors based on selection
    if (selectedOption == 0) {
        SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255); // Highlight color
    } else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255); // Normal color
    }
    SDL_RenderFillRect(renderer, &tryAgainButton);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White border
    SDL_RenderDrawRect(renderer, &tryAgainButton);
    render_text(renderer, "Try Again", SCREEN_WIDTH/2 - 40, 405, white);



    if (selectedOption == 1) {
        SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
    }
    SDL_RenderFillRect(renderer, &mainMenuButton);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White border
    SDL_RenderDrawRect(renderer, &mainMenuButton);
    render_text(renderer, "Main Menu", SCREEN_WIDTH/2 - 40, 505, white);

    
    // Present the renderer
    SDL_RenderPresent(renderer);
}

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

    enum GAME_STATE state = GAME_STATE::SCORING;
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
                case MENU:    event_handler_menu(&event, game, state, running); break;
                case CHOOSE_MAP: event_handler_choose_map(&event, game, state, running); break;
                case CHOOSE_PLAYER: state = PLAYING; break; // choose player skipped
                case PLAYING: event_handler_playing(&event); break;
                case SCORING: event_handler_scoring(&event, game, state); break;
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
            render_main_menu(renderer);
            event_handler_menu(&event, game, state, running);
            break;

            case CHOOSE_MAP:
            render_choose_map(renderer);
            event_handler_choose_map(&event, game, state, running);
            break;

            case CHOOSE_PLAYER:
            state = PLAYING;
            break;

            case PLAYING:
            draw_game(&game, window, renderer);
            // event_handler_playing(&event);
            break;

            case SCORING:
            // event_handler_scoring(&event);
            render_game_over(renderer, game);
            event_handler_scoring(&event, game, state);
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
    }

    font = TTF_OpenFont("assets/font/ARIAL.ttf", 24);
    if(font == NULL)
    {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
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
    return false;
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
        if(TTF_Init() == -1)
        {
            printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
            success = false;
        }
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
    TTF_CloseFont(font);
    font = NULL;
    
    // Quit TTF
    TTF_Quit();
    //Destroy window
    SDL_DestroyWindow(window);
    window = NULL;

    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}


void event_handler_menu(SDL_Event * event, Gameplay &game, enum GAME_STATE &state, bool &running)
{
    // Define menu button rectangles (must match those in render_main_menu)
    SDL_Rect pveButton = { SCREEN_WIDTH/2 - 100, 250, 200, 50 };
    SDL_Rect pvpButton = { SCREEN_WIDTH/2 - 100, 390, 200, 50 };

    int selected_menu_option = -1; // -1: none, 0: PVE, 1: PVP
    
    // Handle mouse motion (hovering)
    if (event->type == SDL_MOUSEMOTION) {
        int mouseX = event->motion.x;
        int mouseY = event->motion.y;
        
        // Check if mouse is over PVE button
        if (mouseX >= pveButton.x && mouseX <= pveButton.x + pveButton.w &&
            mouseY >= pveButton.y && mouseY <= pveButton.y + pveButton.h) {
            selected_menu_option = 0; // Highlight PVE option
        }
        // Check if mouse is over PVP button
        else if (mouseX >= pvpButton.x && mouseX <= pvpButton.x + pvpButton.w &&
                 mouseY >= pvpButton.y && mouseY <= pvpButton.y + pvpButton.h) {
            selected_menu_option = 1; // Highlight PVP option
        }
    }
    
    // Handle mouse clicks
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            int mouseX = event->button.x;
            int mouseY = event->button.y;
            
            // Check if clicked on PVE button
            if (mouseX >= pveButton.x && mouseX <= pveButton.x + pveButton.w &&
                mouseY >= pveButton.y && mouseY <= pveButton.y + pveButton.h) {
                // PVE mode selected
                game.mode = PVE;
                state = CHOOSE_MAP;
            }
            // Check if clicked on PVP button
            else if (mouseX >= pvpButton.x && mouseX <= pvpButton.x + pvpButton.w &&
                     mouseY >= pvpButton.y && mouseY <= pvpButton.y + pvpButton.h) {
                // PVP mode selected
                game.mode = PVP;
                state = CHOOSE_MAP;
            }
        }
    }
    
    // Keep keyboard escape to exit
    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE) {
        running = false;
    }
}

void event_handler_choose_map(SDL_Event* event, Gameplay &game, enum GAME_STATE &state, bool &running) {

    SDL_Rect earthButton = { SCREEN_WIDTH/2 - 100, 250, 200, 50 };
    SDL_Rect moonButton = { SCREEN_WIDTH/2 - 100, 320, 200, 50 };
    SDL_Rect spaceButton = { SCREEN_WIDTH/2 - 100, 390, 200, 50 };
    SDL_Rect backButton = { SCREEN_WIDTH/2 - 450, 100, 100, 60 };
    

    if (event->type == SDL_MOUSEMOTION) {
        int mouseX = event->motion.x;
        int mouseY = event->motion.y;
        int selected_menu_option = -1; 
        

        if (mouseX >= earthButton.x && mouseX <= earthButton.x + earthButton.w &&
            mouseY >= earthButton.y && mouseY <= earthButton.y + earthButton.h) {
            selected_menu_option = 0; 
        }
 
        else if (mouseX >= moonButton.x && mouseX <= moonButton.x + moonButton.w &&
                 mouseY >= moonButton.y && mouseY <= moonButton.y + moonButton.h) {
            selected_menu_option = 1; 
        }
     
        else if (mouseX >= spaceButton.x && mouseX <= spaceButton.x + spaceButton.w &&
                 mouseY >= spaceButton.y && mouseY <= spaceButton.y + spaceButton.h) {
            selected_menu_option = 2; 
        }
    }
    
   
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            int mouseX = event->button.x;
            int mouseY = event->button.y;
            
         
            if (mouseX >= earthButton.x && mouseX <= earthButton.x + earthButton.w &&
                mouseY >= earthButton.y && mouseY <= earthButton.y + earthButton.h) {
            
                game.map = EARTH;
                state = CHOOSE_PLAYER;
            }
        
            else if (mouseX >= moonButton.x && mouseX <= moonButton.x + moonButton.w &&
                     mouseY >= moonButton.y && mouseY <= moonButton.y + moonButton.h) {
         
                game.map = MOON;
                state = CHOOSE_PLAYER;
            }
          
            else if (mouseX >= spaceButton.x && mouseX <= spaceButton.x + spaceButton.w &&
                     mouseY >= spaceButton.y && mouseY <= spaceButton.y + spaceButton.h) {
              
                game.map = SPACE;
                state = CHOOSE_PLAYER;
            }

            else if (mouseX >= backButton.x && mouseX <= backButton.x + backButton.w &&
                     mouseY >= backButton.y && mouseY <= backButton.y + backButton.h) {
              
                state = MENU;
            }
        }
    }
}

void event_handler_playing(SDL_Event* event)
{
    // Process Smooth Moving
    
}
void event_handler_pause(SDL_Event * event)
{

}
void event_handler_scoring(SDL_Event * event, Gameplay &game, enum GAME_STATE &state)
{
    // Define menu button rectangles (must match those in render_game_over)
    SDL_Rect tryAgainButton = { SCREEN_WIDTH/2 - 100, 390, 200, 50 };
    SDL_Rect mainMenuButton = { SCREEN_WIDTH/2 - 100, 490, 200, 50 };

    int selected_menu_option = -1; // -1: none, 0: Try Again, 1: Main Menu
    // Handle mouse motion (hovering)
    if (event->type == SDL_MOUSEMOTION) {
        int mouseX = event->motion.x;
        int mouseY = event->motion.y;
        
        // Check if mouse is over Try Again button
        if (mouseX >= tryAgainButton.x && mouseX <= tryAgainButton.x + tryAgainButton.w &&
            mouseY >= tryAgainButton.y && mouseY <= tryAgainButton.y + tryAgainButton.h) {
            selected_menu_option = 0; // Highlight Try Again option
        }
        // Check if mouse is over Main Menu button
        else if (mouseX >= mainMenuButton.x && mouseX <= mainMenuButton.x + mainMenuButton.w &&
                 mouseY >= mainMenuButton.y && mouseY <= mainMenuButton.y + mainMenuButton.h) {
            selected_menu_option = 1; // Highlight Main Menu option
        }
    }
    // Handle mouse clicks
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            int mouseX = event->button.x;
            int mouseY = event->button.y;
            
            // Check if clicked on Try Again button
            if (mouseX >= tryAgainButton.x && mouseX <= tryAgainButton.x + tryAgainButton.w &&
                mouseY >= tryAgainButton.y && mouseY <= tryAgainButton.y + tryAgainButton.h) {
                // Try Again selected
                game.new_play();
                state = PLAYING;
            }
            // Check if clicked on Main Menu button
            else if (mouseX >= mainMenuButton.x && mouseX <= mainMenuButton.x + mainMenuButton.w &&
                     mouseY >= mainMenuButton.y && mouseY <= mainMenuButton.y + mainMenuButton.h) {
                // Main Menu selected
                state = MENU;
            }
        }
    }
}


