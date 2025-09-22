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
    std::ostringstream ospath;
    ospath << IMAGE_PATH << "characters_sheet.bmp";
    std::string path = ospath.str();

    SDL_Texture * texture = NULL;
    texture = getTexture(window, renderer, path);

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
            draw_limb(window, renderer, texture,
                     BLUE_LEG_X, BLUE_LEG_Y, BLUE_LEG_WIDTH, BLUE_LEG_HEIGHT,
                     player->rect.x + leg_center_x + add_center_x, player->rect.y + leg_center_y - leg_dst_h - 4,
                     leg_dst_w, leg_dst_h,
                     player->rotation_angle + leg1_angle, flip);

            // Draw second leg from center
            draw_limb(window, renderer, texture,
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
            draw_limb(window, renderer, texture,
                     RED_LEG_X, RED_LEG_Y, RED_LEG_WIDTH, RED_LEG_HEIGHT,
                     player->rect.x + leg_center_x + add_center_x, player->rect.y + leg_center_y - leg_dst_h - 4,
                     leg_dst_w, leg_dst_h,
                     player->rotation_angle + leg1_angle, flip);

            // Draw second leg from center
            draw_limb(window, renderer, texture,
                     RED_LEG_X, RED_LEG_Y, RED_LEG_WIDTH, RED_LEG_HEIGHT,
                     player->rect.x + leg_center_x - add_center_x, player->rect.y + leg_center_y + 4,
                     leg_dst_w, leg_dst_h,
                     player->rotation_angle + leg2_angle, flip);
        }
    }

    // Draw main body AFTER legs (so body appears on top)
    SDL_RenderCopyEx(renderer, texture, &src_rect, dst_rect, player->rotation_angle, &center, flip);

    SDL_DestroyTexture(texture);
    texture = NULL;
}

void draw_field(SDL_Window * window, SDL_Renderer * renderer)
{
    // Load sprite sheet
    std::ostringstream ospath;
    ospath << IMAGE_PATH << "field_sprite_sheet.bmp";
    std::string path = ospath.str();

    SDL_Texture * texture = NULL;
    texture = getTexture(window, renderer, path);

    if (!texture) {
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

    auto get_rect_src = [&] (int x, int y) -> SDL_Rect {
        SDL_Rect re = {x * tile_size, y * tile_size, tile_size, tile_size};
        return re;
    };

    auto get_rect_dst = [&] (int x, int y) -> SDL_Rect {
        SDL_Rect re = {field_x + x * tile_size, field_y + y * tile_size, tile_size, tile_size};
        return re;
    };

    auto render_cp = [&] (int x1, int y1, int x2, int y2) -> void {
        auto src = get_rect_src(x1, y1);
        auto dst = get_rect_dst(x2, y2);
        SDL_RenderCopy(renderer, texture, &src, &dst);
    };

    for (int y = 0; y < num_y; y++) {
        for (int x = 0; x < num_x; x++) {
            render_cp(0, 0, x, y);
        }
    }

    // Top=bot border
    for (int x = 0; x < num_x; x++) {
        render_cp(2, 0, x, 0);
        render_cp(2, 3, x, num_y - 1);
    }

    // Left-right border
    for (int y = 0; y < num_y; y++) {
        render_cp(1, 1, 0, y);
        render_cp(4, 1, num_x-1, y);
    }

    // Corner
    render_cp(5, 0, 0, 0);
    render_cp(6, 0, num_x-1, 0);
    render_cp(5, 1, 0, num_y-1);
    render_cp(6, 1, num_x-1, num_y-1);

    // midle line
    for(int y=1; y<num_y-1; y++) render_cp(3, 1, num_x / 2, y);
    render_cp(3, 0, num_x / 2, 0);
    render_cp(3, 3, num_x / 2, num_y-1);
    render_cp(2, 1, num_x / 2, num_y / 2);

    // circle
    render_cp(4, 7, num_x / 2 - 2, num_y / 2 - 2);
    render_cp(5, 7, num_x / 2 - 1, num_y / 2 - 2);
    render_cp(7, 7, num_x / 2 + 1, num_y / 2 - 2);
    render_cp(8, 7, num_x / 2 + 2, num_y / 2 - 2);

    render_cp(5, 11, num_x / 2 - 1, num_y / 2 + 2);
    render_cp(4, 11, num_x / 2 - 2, num_y / 2 + 2);
    render_cp(7, 11, num_x / 2 + 1, num_y / 2 + 2);
    render_cp(8, 11, num_x / 2 + 2, num_y / 2 + 2);

    render_cp(4, 8, num_x / 2 - 2, num_y / 2 - 1);
    render_cp(4, 9, num_x / 2 - 2, num_y / 2 - 0);
    render_cp(4, 10, num_x / 2 - 2, num_y / 2 + 1);

    render_cp(8, 8, num_x / 2 + 2, num_y / 2 - 1);
    render_cp(8, 9, num_x / 2 + 2, num_y / 2 - 0);
    render_cp(8, 10, num_x / 2 + 2, num_y / 2 + 1);

    render_cp(6, 8, num_x / 2 - 0, num_y / 2 - 2);
    render_cp(6, 10, num_x / 2 - 0, num_y / 2 + 2);

    // Left goal
    render_cp(2, 0, 1, num_y / 2 - 3);
    render_cp(2, 3, 1, num_y / 2 + 3);
    render_cp(4, 0, 2, num_y / 2 - 3);
    render_cp(4, 3, 2, num_y / 2 + 3);
    for(int y = num_y/2 - 2; y <= num_y / 2 + 2; y++) render_cp(4, 1, 2, y);
    
    render_cp(12, 2, 0, num_y / 2 - 2);
    render_cp(12, 3, 0, num_y / 2 + 2);
    for(int y = num_y / 2 - 1; y <= num_y / 2 + 1; y++) render_cp(4, 1, 0, y);

    // Right goal
    render_cp(2, 0, num_x - 2, num_y / 2 - 3);
    render_cp(2, 3, num_x - 2, num_y / 2 + 3);
    render_cp(1, 0, num_x - 3, num_y / 2 - 3);
    render_cp(1, 3, num_x - 3, num_y / 2 + 3);
    for(int y = num_y/2 - 2; y <= num_y / 2 + 2; y++) render_cp(1, 1, num_x - 3, y);
    
    render_cp(12, 2, num_x - 1, num_y / 2 - 2);
    render_cp(12, 3, num_x - 1, num_y / 2 + 2);
    for(int y = num_y / 2 - 1; y <= num_y / 2 + 1; y++) render_cp(1, 1, num_x - 1, y);

    SDL_DestroyTexture(texture);
    texture = NULL;
}

void draw_ball(Ball* ball, SDL_Window* window, SDL_Renderer* renderer)
{
    std::ostringstream ospath;
    ospath << IMAGE_PATH << "ball_soccer2.bmp";
    std::string path = ospath.str();

    SDL_Texture * texture = NULL;
    texture = getTexture(window, renderer, path);

    // Enable blending for alpha transparency
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    // Draw trail effect with alpha transparency
    float speed = sqrt(ball->velocity.x * ball->velocity.x + ball->velocity.y * ball->velocity.y);
    if (speed > 15.0f) { // Only show trail when moving fast enough
        // Draw particles as trail effect
        for(int i = 0; i < ball->MAX_PARTICLES; i++) {
            if(ball->particle_life[i] > 0.0f) {
                // Calculate alpha based on particle life (newer = more opaque)
                Uint8 alpha = (Uint8)(ball->particle_life[i] * 150); // 0.0-1.0 -> 0-150
                if (alpha < 20) alpha = 20; // Minimum visibility

                SDL_SetTextureAlphaMod(texture, alpha);

                // Calculate particle size based on life (smaller as they fade)
                float size_factor = ball->particle_life[i] * 0.8f; // 0.0-0.8
                int particle_size = (int)(ball->display_rect.w * size_factor);

                if (particle_size > 4) { // Only draw if big enough
                    SDL_Rect particle_rect = {
                        (int)(ball->particle_x[i] - particle_size/2),
                        (int)(ball->particle_y[i] - particle_size/2),
                        particle_size, particle_size
                    };
                    SDL_RenderCopy(renderer, texture, NULL, &particle_rect);
                }
            }
        }
    }

    // Reset alpha for main ball
    SDL_SetTextureAlphaMod(texture, 255);

    // Make ball bigger (1.2x size as modified)
    int bigger_size = ball->display_rect.w * 1.2f;
    SDL_Rect bigger_rect = {
        ball->display_rect.x - (bigger_size - ball->display_rect.w) / 2,
        ball->display_rect.y - (bigger_size - ball->display_rect.h) / 2,
        bigger_size, bigger_size
    };

    // Draw main ball with rotation
    SDL_Point center = {bigger_size/2, bigger_size/2};
    SDL_RenderCopyEx(renderer, texture, NULL, &bigger_rect, ball->rotation_angle, &center, SDL_FLIP_NONE);

    SDL_DestroyTexture(texture);
    texture = NULL;
}

void draw_goals(SDL_Window* window, SDL_Renderer* renderer)
{
    // Field dimensions (matching draw_field exactly)
    int field_width = SCREEN_WIDTH;
    int field_height = SCREEN_HEIGHT - TOP_PADDING;
    int field_x = 0;
    int field_y = TOP_PADDING;
    int tile_size = 64;

    int num_x = field_width / tile_size;
    int num_y = field_height / tile_size;

    auto draw_goal_tile = [&](const std::string& filename, int tile_x, int tile_y) {
        std::ostringstream ospath;
        ospath << IMAGE_PATH << filename;
        std::string path = ospath.str();

        SDL_Texture* texture = getTexture(window, renderer, path);
        if (texture) {
            SDL_Rect dst_rect = {
                field_x + tile_x * tile_size,
                field_y + tile_y * tile_size,
                tile_size,
                tile_size
            };
            SDL_RenderCopy(renderer, texture, NULL, &dst_rect);
            SDL_DestroyTexture(texture);
        }
    };

    // Left goal posts - draw individual 64x64 tiles
    draw_goal_tile("goal_left_top.bmp", 0, num_y / 2 - 2);
    draw_goal_tile("goal_left.bmp", 0, num_y / 2 - 1);
    draw_goal_tile("goal_left.bmp", 0, num_y / 2 - 0);
    draw_goal_tile("goal_left.bmp", 0, num_y / 2 + 1);
    draw_goal_tile("goal_left_bot.bmp", 0, num_y / 2 + 2);


    // Right goal posts - draw individual 64x64 tiles
    draw_goal_tile("goal_right_top.bmp", num_x - 1, num_y / 2 - 2);
    draw_goal_tile("goal_right.bmp", num_x - 1, num_y / 2 - 1);
    draw_goal_tile("goal_right.bmp", num_x - 1, num_y / 2 - 0);
    draw_goal_tile("goal_right.bmp", num_x - 1, num_y / 2 + 1);
    draw_goal_tile("goal_right_bot.bmp", num_x - 1, num_y / 2 + 2);
}