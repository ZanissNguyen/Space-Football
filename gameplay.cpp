#include "gameplay.h"

void Team::set_members(std::vector<Player*> players)
{
    // Delete old players to avoid memory leaks and reset state
    for (int i = 0; i < members.size(); i++) {
        if (members[i] != nullptr) {
            delete members[i];
        }
    }
    members.clear();
    // Add new players
    for (int i = 0; i < players.size(); i++) {
        members.push_back(players[i]);
    }
}

void Team::set_team(TEAM_CODE t)
{
    code = t;
}

// ---------------- Gameplay ----------------
void Gameplay::process(float delay) {
    // Handle countdown
    if (countdown_active) {
        countdown_timer -= delay;
        if (countdown_timer <= 0.0f) {
            countdown_active = false;
            // Countdown finished - game can start
        }
        return; // Don't process game logic during countdown
    }

    // Update timer
    if (!half_time_break) {
        half_time_remaining -= delay;

        // Check if half time ended
        if (half_time_remaining <= 0.0f) {
            if (current_half == 1) {
                // End of first half - start break
                current_half = 2;
                half_time_remaining = HALF_DURATION; // Reset for second half
                half_time_break = true;
                printf("Half Time! Press any key to continue to second half...\n");
                return; // Don't process game logic during break
            } else {
                // End of second half - game over
                half_time_remaining = 0.0f;
                printf("Full Time! Final Score - Red: %d, Blue: %d\n", red.score, blue.score);
                return; // Game finished
            }
        }
    }

    // moving
    for (int i = 0; i<NUMBER_OF_PLAYER; i++)
    {
        red.members[i]->move(delay);
        blue.members[i]->move(delay);
    }

    // AI moving

    // checking goal
    if (is_ball_in_goal(&ball, &red.score, &blue.score))
    {
        printf("Goal! Score (Red - Blue): (%d, %d)", red.score, blue.score);
        new_play();
        start_countdown(); // Start countdown after goal
    }

    // collision process
    // players collision
    // red vs blue
    if (is_player_collision(red.members[0], red.members[1]))
    {
        process_player_collision(red.members[0], red.members[1]);
    }
    if (is_player_collision(blue.members[0], blue.members[1]))
    {
        process_player_collision(blue.members[0], blue.members[1]);
    }

    for (int i = 0; i<red.members.size(); i++)
    {
        for (int j = 0; j<blue.members.size(); j++)
        {
            if (is_player_collision(red.members[i], blue.members[j]))
            {
                process_player_collision(red.members[i], blue.members[j]);
            }
        }
    }

    for (int i = 0; i<NUMBER_OF_PLAYER; i++)
    {
        if (is_player_shoot(red.members[i], &ball))
        {
            // printf("red hit ball");
            process_shoot_collision(red.members[i], &ball);
        }
        if (is_player_shoot(blue.members[i], &ball))
        {
            // printf("blue hit ball");
            process_shoot_collision(blue.members[i], &ball);
        }
    }
    ball.move(delay);
}

void Gameplay::init(GAME_MAP init_map, std::vector<Player*> red_members, std::vector<Player*> blue_members)
{
    // TODO: set team member run new_play
    map = init_map;
    // setup members
    red.set_team(RED);
    red.set_members(red_members);
    blue.set_team(BLUE);
    blue.set_members(blue_members);
    start_time = SDL_GetTicks64();
    ball.setRadius(BALL_SIZE/2);

    // Initialize timer for first half
    current_half = 1;
    half_time_remaining = HALF_DURATION;
    half_time_break = false;

    // Initialize countdown
    countdown_timer = 3.0f; // Start from 3
    countdown_active = true;

    // printf("assign complete!");
    new_play();
    // printf("init completeeeeee");
}

void Gameplay::new_play() 
{
    int field_width = SCREEN_WIDTH;
    int field_height = SCREEN_HEIGHT - 120;

    // TODO: place players into position and reset ball
    // printf("start new play!");
    if (red.members.size()==2)
    {
        // printf("yes1");
        red.members[0]->place(field_width/3, 120+field_height/3);
        red.members[1]->place(field_width/3, 120+2*field_height/3);
    }
    
    if (blue.members.size()==2)
    {
        // printf("yes2");
        blue.members[0]->place(2*field_width/3, 120+field_height/3);
        blue.members[1]->place(2*field_width/3, 120+2*field_height/3);
    }
    
    // printf("stated");
    ball.place(field_width/2, 120+field_height/2);
}

void Gameplay::rematch() {
    // TODO: reset everything for a new match
}

// ---------------- Collision Functions ----------------
void process_player_collision(Player* player1, Player* player2) {
    SDL_Rect p1Rect = player1->rect;
    SDL_Rect p2Rect = player2->rect;

    if (SDL_HasIntersection(&p1Rect, &p2Rect)) {
        // // Đảm bảo bóng không đi xuyên người: dịch bóng ra ngoài cầu thủ
        // // need redefine
        // if (p1Rect.x < p2Rect.x) player1->change_x(p2Rect.x - PLAYER_SIZE/2 - space);
        // else if (p1Rect.x > p2Rect.x) player1->change_x(p2Rect.x + PLAYER_SIZE*3/2 + space);
        // if (p1Rect.y < p2Rect.y) player1->change_y(p2Rect.y - PLAYER_SIZE/2 - space);
        // else if (p1Rect.y > p2Rect.y) player1->change_y(p2Rect.y + PLAYER_SIZE*3/2 + space);
        // // need redefine
        // player1->velocity.x *=-0.2;
        // player1->velocity.y *=-0.2;
        // player2->velocity.x *=-0.2;
        // player2->velocity.y *=-0.2;

        // Calculate distance between player centers
        float dx = player2->position.x - player1->position.x;
        float dy = player2->position.y - player1->position.y;
        float distance = sqrt(dx*dx + dy*dy);

        // Avoid division by zero
        if (distance < 1.0f) {
            dx = 1.0f; dy = 0.0f; distance = 1.0f;
        }

        // Normalize direction vector
        float nx = dx / distance;
        float ny = dy / distance;

        // Minimum separation distance (sum of half-widths + small buffer)
        float min_distance = (PLAYER_SPRITE_WIDTH + PLAYER_SPRITE_HEIGHT) / 2.0f + 2.0f;
        float overlap = min_distance - distance;

        if (overlap > 0) {
            // Separate players smoothly based on their toughness
            float total_toughness = player1->toughness + player2->toughness;
            float separation_factor = 1.0f; // Increased from 0.5f to prevent phase-through

            if (total_toughness > 0) {
                float p1_ratio = player2->toughness / total_toughness;
                float p2_ratio = player1->toughness / total_toughness;

                // Move players apart
                player1->position.x -= nx * overlap * p1_ratio * separation_factor;
                player1->position.y -= ny * overlap * p1_ratio * separation_factor;
                player2->position.x += nx * overlap * p2_ratio * separation_factor;
                player2->position.y += ny * overlap * p2_ratio * separation_factor;

                // Update rects
                player1->rect.x = player1->position.x - PLAYER_SPRITE_WIDTH/2;
                player1->rect.y = player1->position.y - PLAYER_SPRITE_HEIGHT/2;
                player2->rect.x = player2->position.x - PLAYER_SPRITE_WIDTH/2;
                player2->rect.y = player2->position.y - PLAYER_SPRITE_HEIGHT/2;
            }

            // Apply collision response to velocities (bounce effect)
            float bounce_factor = 0.3f;

            // Calculate relative velocity
            float rel_vx = player2->velocity.x - player1->velocity.x;
            float rel_vy = player2->velocity.y - player1->velocity.y;

            // Calculate relative velocity along collision normal
            float vel_along_normal = rel_vx * nx + rel_vy * ny;

            // Only resolve if objects are moving towards each other
            if (vel_along_normal > 0) return;

            // Apply collision impulse
            float impulse = bounce_factor * vel_along_normal;

            player1->velocity.x += impulse * nx;
            player1->velocity.y += impulse * ny;
            player2->velocity.x -= impulse * nx;
            player2->velocity.y -= impulse * ny;
        }
    }
}

bool is_player_collision(Player* player1, Player* player2) {
    // TODO: return true if players collide
    SDL_Rect p1Rect = player1->rect;
    SDL_Rect p2Rect = player2->rect;
    return SDL_HasIntersection(&p1Rect, &p2Rect);
}

void process_shoot_collision(Player* player, Ball* ball) {
    // TODO: handle player shooting ball
    // for (auto* p : red_team) {
    SDL_Rect pRect = player->rect;
    SDL_Rect bRect = ball->display_rect;
    int space = 0;
    if (SDL_HasIntersection(&pRect, &bRect)) {
        // Đảm bảo bóng không đi xuyên người: dịch bóng ra ngoài cầu thủ
        // need redefine
        if (bRect.x < pRect.x) ball->change_x(pRect.x - ball->radius + space);
        else if (bRect.x > pRect.x) ball->change_x(pRect.x + PLAYER_SIZE + ball->radius + space);
        if (bRect.y < pRect.y) ball->change_y(pRect.y - ball->radius + space);
        else if (bRect.y > pRect.y) ball->change_y(pRect.y + PLAYER_SIZE + ball->radius + space);
        // need redefine
        float dx = bRect.x - pRect.x;
        float dy = bRect.y - pRect.y;
        float mag = std::sqrt(dx*dx + dy*dy);
        if (mag > 0) {
            ball->velocity.x = player->velocity.x*3;
            ball->velocity.y = player->velocity.y*3;
        } else {
            ball->velocity.x = -player->velocity.x*3;
            ball->velocity.y = player->velocity.x*3;
        }
        ball->last_touch = player->team;
    }
}

bool is_player_shoot(Player* player, Ball* ball) {
    // TODO: return true if player shoots ball
    SDL_Rect pRect = player->rect;
    SDL_Rect bRect = ball->display_rect;
    return SDL_HasIntersection(&pRect, &bRect);
}

bool is_ball_in_goal(Ball* ball, int * red_score, int * blue_score) {
    // Match the visual goal range drawn by draw_goals
    int field_height = SCREEN_HEIGHT - TOP_PADDING;
    int tile_size = 64;
    int num_y = field_height / tile_size;

    // Goal Y range matches the visual tiles: (num_y/2 - 2) to (num_y/2 + 2)
    int goal_top_y = TOP_PADDING + (num_y/2 - 2) * tile_size + 40;
    int goal_bottom_y = TOP_PADDING + (num_y/2 + 2 + 1) * tile_size - 40; // +1 for inclusive range

    // Goal X range: within the goal post tiles
    int goal_depth = tile_size / 2; // One tile deep

    // Left goal (blue scores): x <= goal_depth, y in goal range
    if (ball->position.x <= goal_depth &&
        ball->position.y >= goal_top_y && ball->position.y <= goal_bottom_y) {
        *blue_score+=1; return true;
    }

    // Right goal (red scores): x >= screen_width - goal_depth, y in goal range
    if (ball->position.x >= SCREEN_WIDTH - goal_depth &&
        ball->position.y >= goal_top_y && ball->position.y <= goal_bottom_y) {
        *red_score+=1; return true;
    }

    return false;
}

void Gameplay::resume_second_half() {
    half_time_break = false;
    new_play(); // Reset player positions for second half
    start_countdown(); // Start countdown for second half
    printf("Second half started!\n");
}

void Gameplay::start_countdown() {
    countdown_timer = 3.0f; // Reset countdown from 3
    countdown_active = true;
}