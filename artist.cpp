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

    // Draw goals on top of everything (highest z-index)
    draw_goals(window, renderer);

    // Draw timer on top of everything
    draw_timer(game, window, renderer);
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

void draw_limb(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture,
               int src_x, int src_y, int src_w, int src_h,
               int dst_x, int dst_y, int dst_w, int dst_h,
               double angle, SDL_RendererFlip flip) {
    SDL_Rect src_rect = {src_x, src_y, src_w, src_h};
    SDL_Rect dst_rect = {dst_x, dst_y, dst_w, dst_h};
    SDL_Point center = {0, dst_h/2};

    SDL_RenderCopyEx(renderer, texture, &src_rect, &dst_rect, angle, &center, flip);
}

void draw_player(Player* player, SDL_Window* window, SDL_Renderer* renderer)
{
    // Static texture to avoid loading every frame (this function is called 4 times per frame!)
    static SDL_Texture* characters_texture = nullptr;

    // Load texture once
    if (!characters_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "characters_sheet.bmp";
        characters_texture = getTexture(window, renderer, ospath.str());
    }

    if (!characters_texture) return;

    // Calculate sprite position in sheet based on player team and type
    int sprite_x = 0;
    int sprite_y = 0;

    if (player->team == RED) {
        // Team 0 (red) at position (7*21, 5*31)
        sprite_x = 7 * PLAYER_SPRITE_SRC_WIDTH;
        sprite_y = 5 * PLAYER_SPRITE_SRC_HEIGHT;
    } else if (player->team == BLUE) {
        // Team 1 (blue) at position (2*21, 1*31)
        sprite_x = 2 * PLAYER_SPRITE_SRC_WIDTH;
        sprite_y = 1 * PLAYER_SPRITE_SRC_HEIGHT;
    }

    SDL_Rect src_rect = {sprite_x, sprite_y, PLAYER_SPRITE_SRC_WIDTH, PLAYER_SPRITE_SRC_HEIGHT};
    SDL_Rect * dst_rect = &player->rect;

    // Use SDL_RenderCopyEx for rotation
    SDL_Point center = {PLAYER_SPRITE_WIDTH/2, PLAYER_SPRITE_HEIGHT/2};
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    // Blue team initially faces left, so flip horizontally when not moving
    if (player->team == BLUE && (player->velocity.x == 0 && player->velocity.y == 0)) {
        flip = SDL_FLIP_HORIZONTAL;
    }

    // Draw animated legs BEFORE body (so they appear under the body)
    // Only show legs when velocity is fast enough
    float velocity_magnitude = sqrt(player->velocity.x * player->velocity.x + player->velocity.y * player->velocity.y);
    if (player->is_moving && velocity_magnitude > 50.0f) {
        float running_cycle = player->animation_time;
        float ani_int = M_PI / 2.8;
        if(velocity_magnitude < 400.0f) ani_int = M_PI / 2;
        if(velocity_magnitude < 200.0f) ani_int = M_PI;

        // Determine leg positions based on cycle
        float cycle_position = fmod(running_cycle, 2.0f * ani_int);

        float leg1_angle, leg2_angle;

        if (cycle_position < ani_int) {
            // First half: leg1 forward, leg2 backward
            leg1_angle = 0.0f;   // Forward (reduced angle)
            leg2_angle = 180.0f;  // Backward (reduced angle)
        } else {
            // Second half: leg1 backward, leg2 forward
            leg1_angle = 180.0f;  // Backward (reduced angle)
            leg2_angle = 0.0f;   // Forward (reduced angle)
        }

        // Scale factor for legs - much smaller like in example
        float leg_scale = 1.2f;

        if (player->team == BLUE) {
            // Blue team legs
            int leg_dst_w = BLUE_LEG_WIDTH * leg_scale;
            int leg_dst_h = BLUE_LEG_HEIGHT * leg_scale;

            // Position both legs at center of body
            int leg_center_x = PLAYER_SPRITE_WIDTH * 0.5f;  // Center horizontally
            int leg_center_y = PLAYER_SPRITE_HEIGHT * 0.5f; // Bottom of body

            int add_center_x = 3;
            if(abs(player->rotation_angle - 90) < 20 || abs(player->rotation_angle + 90) < 20) {
                add_center_x = 8;
            }

            // Draw first leg from center
            draw_limb(window, renderer, characters_texture,
                     BLUE_LEG_X, BLUE_LEG_Y, BLUE_LEG_WIDTH, BLUE_LEG_HEIGHT,
                     player->rect.x + leg_center_x + add_center_x, player->rect.y + leg_center_y - leg_dst_h - 4,
                     leg_dst_w, leg_dst_h,
                     player->rotation_angle + leg1_angle, flip);

            // Draw second leg from center
            draw_limb(window, renderer, characters_texture,
                     BLUE_LEG_X, BLUE_LEG_Y, BLUE_LEG_WIDTH, BLUE_LEG_HEIGHT,
                     player->rect.x + leg_center_x - add_center_x, player->rect.y + leg_center_y + 4,
                     leg_dst_w, leg_dst_h,
                     player->rotation_angle + leg2_angle, flip);

        } else if (player->team == RED) {
            // Red team legs
            int leg_dst_w = RED_LEG_WIDTH * leg_scale;
            int leg_dst_h = RED_LEG_HEIGHT * leg_scale;

            // Position both legs at center of body
            int leg_center_x = PLAYER_SPRITE_WIDTH * 0.5f;  // Center horizontally
            int leg_center_y = PLAYER_SPRITE_HEIGHT * 0.5f; // Bottom of body

            int add_center_x = 3;
            if(abs(player->rotation_angle - 90) < 20 || abs(player->rotation_angle + 90) < 20) {
                add_center_x = 8;
            }

            // Draw first leg from center
            draw_limb(window, renderer, characters_texture,
                     RED_LEG_X, RED_LEG_Y, RED_LEG_WIDTH, RED_LEG_HEIGHT,
                     player->rect.x + leg_center_x + add_center_x, player->rect.y + leg_center_y - leg_dst_h - 4,
                     leg_dst_w, leg_dst_h,
                     player->rotation_angle + leg1_angle, flip);

            // Draw second leg from center
            draw_limb(window, renderer, characters_texture,
                     RED_LEG_X, RED_LEG_Y, RED_LEG_WIDTH, RED_LEG_HEIGHT,
                     player->rect.x + leg_center_x - add_center_x, player->rect.y + leg_center_y + 4,
                     leg_dst_w, leg_dst_h,
                     player->rotation_angle + leg2_angle, flip);
        }
    }

    // Draw main body AFTER legs (so body appears on top)
    SDL_RenderCopyEx(renderer, characters_texture, &src_rect, dst_rect, player->rotation_angle, &center, flip);

    // Don't destroy static texture - it's reused
}

void draw_field(SDL_Window * window, SDL_Renderer * renderer)
{
    // Static texture to avoid loading every frame

    // Use static textures for each map
    static SDL_Texture* earth_texture = nullptr;
    static SDL_Texture* moon_texture = nullptr;
    SDL_Texture* field_texture = nullptr;

    // Determine which texture to use based on map
    extern Gameplay game; // Use global game object to access map
    if (game.map == MOON) {
        if (!moon_texture) {
            std::ostringstream ospath;
            ospath << IMAGE_PATH << "field_sprite_sheet_moon.bmp";
            moon_texture = getTexture(window, renderer, ospath.str());
        }
        field_texture = moon_texture;
    } else {
        if (!earth_texture) {
            std::ostringstream ospath;
            ospath << IMAGE_PATH << "field_sprite_sheet.bmp";
            earth_texture = getTexture(window, renderer, ospath.str());
        }
        field_texture = earth_texture;
    }

    if (!field_texture) {
        // Fallback to simple green field if sprite sheet fails
        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
        SDL_Rect field_bg = {0, 120, SCREEN_WIDTH, SCREEN_HEIGHT-120};
        SDL_RenderFillRect(renderer, &field_bg);
        return;
    }

    // Field dimensions
    int field_width = SCREEN_WIDTH;
    int field_height = SCREEN_HEIGHT - TOP_PADDING;
    int field_x = 0;
    int field_y = TOP_PADDING;
    int tile_size = 64;

    int num_x = field_width / 64;
    int num_y = field_height / 64;

    // Inline functions for better performance (avoid lambda overhead)
    auto render_tile = [=](int src_x, int src_y, int dst_x, int dst_y) {
        SDL_Rect src_rect = {src_x * tile_size, src_y * tile_size, tile_size, tile_size};
        SDL_Rect dst_rect = {field_x + dst_x * tile_size, field_y + dst_y * tile_size, tile_size, tile_size};
        SDL_RenderCopy(renderer, field_texture, &src_rect, &dst_rect);
    };

    for (int y = 0; y < num_y; y++) {
        for (int x = 0; x < num_x; x++) {
            render_tile(0, 0, x, y);
        }
    }

    // Top=bot border
    for (int x = 0; x < num_x; x++) {
        render_tile(2, 0, x, 0);
        render_tile(2, 3, x, num_y - 1);
    }

    // Left-right border
    for (int y = 0; y < num_y; y++) {
        render_tile(1, 1, 0, y);
        render_tile(4, 1, num_x-1, y);
    }

    // Corner
    render_tile(5, 0, 0, 0);
    render_tile(6, 0, num_x-1, 0);
    render_tile(5, 1, 0, num_y-1);
    render_tile(6, 1, num_x-1, num_y-1);

    // midle line
    for(int y=1; y<num_y-1; y++) render_tile(3, 1, num_x / 2, y);
    render_tile(3, 0, num_x / 2, 0);
    render_tile(3, 3, num_x / 2, num_y-1);
    render_tile(2, 1, num_x / 2, num_y / 2);

    // circle
    render_tile(4, 7, num_x / 2 - 2, num_y / 2 - 2);
    render_tile(5, 7, num_x / 2 - 1, num_y / 2 - 2);
    render_tile(7, 7, num_x / 2 + 1, num_y / 2 - 2);
    render_tile(8, 7, num_x / 2 + 2, num_y / 2 - 2);

    render_tile(5, 11, num_x / 2 - 1, num_y / 2 + 2);
    render_tile(4, 11, num_x / 2 - 2, num_y / 2 + 2);
    render_tile(7, 11, num_x / 2 + 1, num_y / 2 + 2);
    render_tile(8, 11, num_x / 2 + 2, num_y / 2 + 2);

    render_tile(4, 8, num_x / 2 - 2, num_y / 2 - 1);
    render_tile(4, 9, num_x / 2 - 2, num_y / 2 - 0);
    render_tile(4, 10, num_x / 2 - 2, num_y / 2 + 1);

    render_tile(8, 8, num_x / 2 + 2, num_y / 2 - 1);
    render_tile(8, 9, num_x / 2 + 2, num_y / 2 - 0);
    render_tile(8, 10, num_x / 2 + 2, num_y / 2 + 1);

    render_tile(6, 8, num_x / 2 - 0, num_y / 2 - 2);
    render_tile(6, 10, num_x / 2 - 0, num_y / 2 + 2);

    // Left goal
    render_tile(2, 0, 1, num_y / 2 - 3);
    render_tile(2, 3, 1, num_y / 2 + 3);
    render_tile(4, 0, 2, num_y / 2 - 3);
    render_tile(4, 3, 2, num_y / 2 + 3);
    for(int y = num_y/2 - 2; y <= num_y / 2 + 2; y++) render_tile(4, 1, 2, y);
    
    render_tile(12, 2, 0, num_y / 2 - 2);
    render_tile(12, 3, 0, num_y / 2 + 2);
    for(int y = num_y / 2 - 1; y <= num_y / 2 + 1; y++) render_tile(4, 1, 0, y);

    // Right goal
    render_tile(2, 0, num_x - 2, num_y / 2 - 3);
    render_tile(2, 3, num_x - 2, num_y / 2 + 3);
    render_tile(1, 0, num_x - 3, num_y / 2 - 3);
    render_tile(1, 3, num_x - 3, num_y / 2 + 3);
    for(int y = num_y/2 - 2; y <= num_y / 2 + 2; y++) render_tile(1, 1, num_x - 3, y);
    
    render_tile(12, 2, num_x - 1, num_y / 2 - 2);
    render_tile(12, 3, num_x - 1, num_y / 2 + 2);
    for(int y = num_y / 2 - 1; y <= num_y / 2 + 1; y++) render_tile(1, 1, num_x - 1, y);

    // Don't destroy static texture - it's reused
}

void draw_ball(Ball* ball, SDL_Window* window, SDL_Renderer* renderer)
{
    // Static texture to avoid loading every frame
    static SDL_Texture* ball_texture = nullptr;

    // Load texture once
    if (!ball_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "ball_soccer2.bmp";
        ball_texture = getTexture(window, renderer, ospath.str());
    }

    if (!ball_texture) return;

    // Enable blending for alpha transparency
    SDL_SetTextureBlendMode(ball_texture, SDL_BLENDMODE_BLEND);

    // Draw trail effect with alpha transparency
    float speed = sqrt(ball->velocity.x * ball->velocity.x + ball->velocity.y * ball->velocity.y);
    if (speed > 15.0f) { // Only show trail when moving fast enough
        int base_width = ball->display_rect.w; // Cache for performance

        // Draw particles as trail effect
        for(int i = 0; i < ball->MAX_PARTICLES; i++) {
            float life = ball->particle_life[i];
            if(life > 0.0f) {
                // Calculate alpha based on particle life (newer = more opaque)
                Uint8 alpha = (Uint8)(life * 150); // 0.0-1.0 -> 0-150
                if (alpha < 20) alpha = 20; // Minimum visibility

                SDL_SetTextureAlphaMod(ball_texture, alpha);

                // Calculate particle size based on life (smaller as they fade)
                int particle_size = (int)(base_width * life * 0.8f);

                if (particle_size > 4) { // Only draw if big enough
                    int half_size = particle_size / 2;
                    SDL_Rect particle_rect = {
                        (int)(ball->particle_x[i]) - half_size,
                        (int)(ball->particle_y[i]) - half_size,
                        particle_size, particle_size
                    };
                    SDL_RenderCopy(renderer, ball_texture, NULL, &particle_rect);
                }
            }
        }
    }

    // Reset alpha for main ball
    SDL_SetTextureAlphaMod(ball_texture, 255);

    // Make ball bigger (1.2x size as modified)
    int bigger_size = ball->display_rect.w * 1.2f;
    SDL_Rect bigger_rect = {
        ball->display_rect.x - (bigger_size - ball->display_rect.w) / 2,
        ball->display_rect.y - (bigger_size - ball->display_rect.h) / 2,
        bigger_size, bigger_size
    };

    // Draw main ball with rotation
    SDL_Point center = {bigger_size/2, bigger_size/2};
    SDL_RenderCopyEx(renderer, ball_texture, NULL, &bigger_rect, ball->rotation_angle, &center, SDL_FLIP_NONE);

    // Don't destroy static texture - it's reused
}

void draw_goals(SDL_Window* window, SDL_Renderer* renderer)
{
    // Static textures to avoid loading every frame
    static SDL_Texture* goal_left_texture = nullptr;
    static SDL_Texture* goal_left_top_texture = nullptr;
    static SDL_Texture* goal_left_bot_texture = nullptr;
    static SDL_Texture* goal_right_texture = nullptr;
    static SDL_Texture* goal_right_top_texture = nullptr;
    static SDL_Texture* goal_right_bot_texture = nullptr;

    // Load textures once
    if (!goal_left_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "goal_left.bmp";
        goal_left_texture = getTexture(window, renderer, ospath.str());
    }
    if (!goal_left_top_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "goal_left_top.bmp";
        goal_left_top_texture = getTexture(window, renderer, ospath.str());
    }
    if (!goal_left_bot_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "goal_left_bot.bmp";
        goal_left_bot_texture = getTexture(window, renderer, ospath.str());
    }
    if (!goal_right_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "goal_right.bmp";
        goal_right_texture = getTexture(window, renderer, ospath.str());
    }
    if (!goal_right_top_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "goal_right_top.bmp";
        goal_right_top_texture = getTexture(window, renderer, ospath.str());
    }
    if (!goal_right_bot_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "goal_right_bot.bmp";
        goal_right_bot_texture = getTexture(window, renderer, ospath.str());
    }

    // Field dimensions (matching draw_field exactly)
    int field_width = SCREEN_WIDTH;
    int field_height = SCREEN_HEIGHT - TOP_PADDING;
    int field_x = 0;
    int field_y = TOP_PADDING;
    int tile_size = 64;

    int num_x = field_width / tile_size;
    int num_y = field_height / tile_size;

    auto draw_goal_tile = [&](SDL_Texture* texture, int tile_x, int tile_y) {
        if (texture) {
            SDL_Rect dst_rect = {
                field_x + tile_x * tile_size,
                field_y + tile_y * tile_size,
                tile_size,
                tile_size
            };
            SDL_RenderCopy(renderer, texture, NULL, &dst_rect);
        }
    };

    // Left goal posts - draw individual 64x64 tiles using cached textures
    draw_goal_tile(goal_left_top_texture, 0, num_y / 2 - 2);
    draw_goal_tile(goal_left_texture, 0, num_y / 2 - 1);
    draw_goal_tile(goal_left_texture, 0, num_y / 2 - 0);
    draw_goal_tile(goal_left_texture, 0, num_y / 2 + 1);
    draw_goal_tile(goal_left_bot_texture, 0, num_y / 2 + 2);

    // Right goal posts - draw individual 64x64 tiles using cached textures
    draw_goal_tile(goal_right_top_texture, num_x - 1, num_y / 2 - 2);
    draw_goal_tile(goal_right_texture, num_x - 1, num_y / 2 - 1);
    draw_goal_tile(goal_right_texture, num_x - 1, num_y / 2 - 0);
    draw_goal_tile(goal_right_texture, num_x - 1, num_y / 2 + 1);
    draw_goal_tile(goal_right_bot_texture, num_x - 1, num_y / 2 + 2);
}

void draw_timer(Gameplay* game, SDL_Window* window, SDL_Renderer* renderer)
{
    // Static textures to avoid loading every frame
    static SDL_Texture* timer_bg_texture = nullptr;
    static SDL_Texture* info_window_texture = nullptr;
    static SDL_Texture* text_sheet_texture = nullptr;

    // Load textures once
    if (!timer_bg_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "timer_background.bmp";
        timer_bg_texture = getTexture(window, renderer, ospath.str());
    }
    if (!info_window_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "info_window.bmp";
        info_window_texture = getTexture(window, renderer, ospath.str());
    }

    // Convert seconds to minutes:seconds format
    int minutes = (int)(game->half_time_remaining) / 60;
    int seconds = (int)(game->half_time_remaining) % 60;

    // Draw timer background using cached texture
    if (timer_bg_texture) {
        SDL_Rect timer_bg = {SCREEN_WIDTH/2 - 200, 10, 400, 120};
        SDL_RenderCopy(renderer, timer_bg_texture, NULL, &timer_bg);
    }

    // Draw half indicator text
    if (game->current_half == 1) {
        draw_text("FIRST HALF", SCREEN_WIDTH/2 - 80, 25, window, renderer, 0.35f);
    } else {
        draw_text("SECOND HALF", SCREEN_WIDTH/2 - 85, 25, window, renderer, 0.35f);
    }

    // Draw time remaining
    char time_str[6];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", minutes, seconds);
    draw_text(time_str, SCREEN_WIDTH/2 - 40, 55, window, renderer, 0.4f);

    // Draw scores
    char red_score_str[10];
    char blue_score_str[10];
    snprintf(red_score_str, sizeof(red_score_str), "RED: %d", game->red.score);
    snprintf(blue_score_str, sizeof(blue_score_str), "BLUE: %d", game->blue.score);

    draw_text(red_score_str, SCREEN_WIDTH/2 - 105, 85, window, renderer, 0.3f);
    draw_text(blue_score_str, SCREEN_WIDTH/2 + 30, 85, window, renderer, 0.3f);

    // Show countdown timer (3-2-1) when active
    if (game->countdown_active) {
        int countdown_number = (int)ceil(game->countdown_timer);
        if (countdown_number > 0) {
            // Draw large countdown number in center of screen
            char countdown_str[2];
            snprintf(countdown_str, sizeof(countdown_str), "%d", countdown_number);

            // Large countdown display
            draw_text(countdown_str, SCREEN_WIDTH/2 - 20, SCREEN_HEIGHT/2 - 40, window, renderer, 2.0f);
        }
    }

    // Show half time break message using cached info window texture
    if (game->half_time_break) {
        if (info_window_texture) {
            SDL_Rect info_bg = {SCREEN_WIDTH/2 - 200, 200, 400, 150};
            SDL_RenderCopy(renderer, info_window_texture, NULL, &info_bg);
        }

        draw_text("HALF TIME", SCREEN_WIDTH/2 - 70, 230, window, renderer, 0.4f);
        draw_text("PRESS ANY KEY", SCREEN_WIDTH/2 - 80, 270, window, renderer, 0.35f);
        draw_text("TO CONTINUE", SCREEN_WIDTH/2 - 75, 300, window, renderer, 0.35f);
    }
}

void draw_text(const std::string& text, int x, int y, SDL_Window* window, SDL_Renderer* renderer, float scale)
{
    // Static texture to avoid loading every frame
    static SDL_Texture* text_sheet_texture = nullptr;

    // Load texture once
    if (!text_sheet_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "text_sheet.bmp";
        text_sheet_texture = getTexture(window, renderer, ospath.str());
    }

    if (!text_sheet_texture) return;

    const int CHAR_WIDTH = 70;
    const int CHAR_HEIGHT = 80;
    const int CHAR_RENDER_WIDTH = 40; // Use center 40px of 70px cell
    const int CHAR_OFFSET_X = 15; // Start 15px from left to center the 40px
    const int COLS = 16;

    int current_x = x;

    for (char c : text) {
        int row = -1, col = -1;

        // Map character to row and column based on the character map
        if (c >= '!' && c <= '/') {
            row = 0;
            col = c - '!';
        } else if (c >= '0' && c <= '?') {
            row = 1;
            col = c - '0';
        } else if (c >= '@' && c <= 'O') {
            row = 2;
            col = c - '@';
        } else if (c >= 'P' && c <= '_') {
            row = 3;
            col = c - 'P';
        } else if (c >= '`' && c <= 'o') {
            row = 4;
            col = c - '`';
        } else if (c >= 'p' && c <= '~') {
            row = 5;
            col = c - 'p';
        } else if (c == ' ') {
            // Space - just advance cursor
            current_x += (int)(CHAR_RENDER_WIDTH * scale * 0.5f); // Half width for space
            continue;
        } else {
            // Unknown character - skip
            continue;
        }

        // Calculate source rectangle - use center 50px of 70px cell
        SDL_Rect src_rect = {
            col * CHAR_WIDTH + CHAR_OFFSET_X,
            row * CHAR_HEIGHT,
            CHAR_RENDER_WIDTH,
            CHAR_HEIGHT
        };

        // Calculate destination rectangle
        SDL_Rect dst_rect = {
            current_x,
            y,
            (int)(CHAR_RENDER_WIDTH * scale),
            (int)(CHAR_HEIGHT * scale)
        };

        // Render character
        SDL_RenderCopy(renderer, text_sheet_texture, &src_rect, &dst_rect);

        // Advance cursor
        current_x += (int)(CHAR_RENDER_WIDTH * scale);
    }

    // Don't destroy static texture - it's reused
}

void draw_text_white(const std::string& text, int x, int y, SDL_Window* window, SDL_Renderer* renderer, float scale)
{
    // Static texture to avoid loading every frame
    static SDL_Texture* text_sheet_white_texture = nullptr;

    // Load texture once
    if (!text_sheet_white_texture) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << "text_sheet_white.bmp";
        text_sheet_white_texture = getTexture(window, renderer, ospath.str());
    }

    if (!text_sheet_white_texture) return;

    const int CHAR_WIDTH = 70;
    const int CHAR_HEIGHT = 80;
    const int CHAR_RENDER_WIDTH = 40; // Use center 40px of 70px cell
    const int CHAR_OFFSET_X = 15; // Start 15px from left to center the 40px
    const int COLS = 16;

    int current_x = x;

    for (char c : text) {
        int row = -1, col = -1;

        // Map character to row and column based on the character map
        if (c >= '!' && c <= '/') {
            row = 0;
            col = c - '!';
        } else if (c >= '0' && c <= '?') {
            row = 1;
            col = c - '0';
        } else if (c >= '@' && c <= 'O') {
            row = 2;
            col = c - '@';
        } else if (c >= 'P' && c <= '_') {
            row = 3;
            col = c - 'P';
        } else if (c >= '`' && c <= 'o') {
            row = 4;
            col = c - '`';
        } else if (c >= 'p' && c <= '~') {
            row = 5;
            col = c - 'p';
        } else if (c == ' ') {
            // Space - just advance cursor
            current_x += (int)(CHAR_RENDER_WIDTH * scale * 0.5f); // Half width for space
            continue;
        } else {
            // Unknown character - skip
            continue;
        }

        // Calculate source rectangle - use center 40px of 70px cell
        SDL_Rect src_rect = {
            col * CHAR_WIDTH + CHAR_OFFSET_X,
            row * CHAR_HEIGHT,
            CHAR_RENDER_WIDTH,
            CHAR_HEIGHT
        };

        // Calculate destination rectangle
        SDL_Rect dst_rect = {
            current_x,
            y,
            (int)(CHAR_RENDER_WIDTH * scale),
            (int)(CHAR_HEIGHT * scale)
        };

        // Render character
        SDL_RenderCopy(renderer, text_sheet_white_texture, &src_rect, &dst_rect);

        // Advance cursor
        current_x += (int)(CHAR_RENDER_WIDTH * scale);
    }

    // Don't destroy static texture - it's reused
}