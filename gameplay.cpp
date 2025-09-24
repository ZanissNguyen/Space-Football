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

void Team::change_control()
{
    active_player = (active_player == 0) ? 1: 0;
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

    // AI moving
    for (int i = 0; i<NUMBER_OF_PLAYER; i++)
    {
        // printf ("%d\n", red.active_player);
        if (i!=red.active_player) red.members[i]->AI_Support(this);
        if (mode == PVP)
        {
            if (i!=blue.active_player)
            {
                blue.members[i]->AI_Support(this);
            }
        }
        else blue.members[i]->AI_Support(this);
    }

    // moving
    for (int i = 0; i<NUMBER_OF_PLAYER; i++)
    {
        red.members[i]->move(this, delay);
        blue.members[i]->move(this, delay);
    }

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
    process_player_collision(this, red.members[0], red.members[1]);
    process_player_collision(this, blue.members[0], blue.members[1]);

    for (int i = 0; i<red.members.size(); i++)
    {
        for (int j = 0; j<blue.members.size(); j++)
        {
            process_player_collision(this, red.members[i], blue.members[j]);
        }
    }

    for (int i = 0; i<NUMBER_OF_PLAYER; i++)
    {
        process_shoot_collision(this, red.members[i], &ball);
        process_shoot_collision(this, blue.members[i], &ball);
    }
    ball.move(this, delay);

    
    for (int i = 0; i<NUMBER_OF_PLAYER; i++)
    {
        process_player_hit_border(this, red.members[i]);
        process_player_hit_border(this, blue.members[i]);
    }
    process_ball_hit_border(this, &ball);
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
void process_player_collision(Gameplay * game, Player* player1, Player* player2) {
    SDL_Rect p1Rect = player1->rect;
    SDL_Rect p2Rect = player2->rect;
    if (!SDL_HasIntersection(&p1Rect, &p2Rect)) return;

    Vec2 delta = Vec2(player2->position.x - player1->position.x
        , player2->position.y - player1->position.y);
    float distance = delta.magnitude();

    // Normalize direction vector
    Vec2 direction = delta.normalize();

    // Minimum separation distance (sum of half-widths + small buffer)
    float min_distance = (PLAYER_SPRITE_WIDTH*2) / 2.0f + 2.0f;
    float overlap = min_distance - distance; 
    
    float total_toughness = player1->toughness + player2->toughness;

    if (overlap > 0) {
        // Separate players smoothly based on their toughness
        float separation_factor = 1.5f; // Increased from 0.5f to prevent phase-through
        float p1_ratio = player2->toughness / total_toughness;
        float p2_ratio = player1->toughness / total_toughness;

        if (total_toughness > 0) {
            
            if (player1->team == player2->team)
            {
                p1_ratio = p2_ratio = 0.5;
            }

            player1->change_position(player1->position.x-direction.x*overlap * p1_ratio *separation_factor,
                player1->position.y-direction.y*overlap * p1_ratio *separation_factor);

            player2->change_position(player2->position.x+direction.x*overlap * p2_ratio *separation_factor,
                player2->position.y+direction.y*overlap * p2_ratio *separation_factor);

        }

        // // Apply collision response to velocities (bounce effect)
        float bounce_factor = (game->map == MOON) ? BOUNCE_FACTOR_MOON : BOUNCE_FACTOR_EARTH;

        // Calculate relative velocity
        float rel_vx = player2->velocity.x - player1->velocity.x;
        float rel_vy = player2->velocity.y - player1->velocity.y;

        // Calculate relative velocity along collision normal
        float vel_along_normal = rel_vx * direction.x + rel_vy * direction.y;

        // Only resolve if objects are moving towards each other
        if (vel_along_normal > 0) return;

        // Apply collision impulse
        float impulse = bounce_factor * vel_along_normal; 

        player1->velocity.x += (impulse/p1_ratio) * direction.x;
        player1->velocity.y += (impulse/p1_ratio) * direction.y;
        player2->velocity.x -= (impulse/p2_ratio) * direction.x;
        player2->velocity.y -= (impulse/p2_ratio) * direction.y;
    }

}

void process_shoot_collision(Gameplay * game, Player* player, Ball* ball) {
    SDL_Rect pRect = player->rect;
    Circle bCircle = ball->circle;

    // Find closest point on rect to circle center
    float closestX = clamp(bCircle.x, (float)pRect.x, (float)(pRect.x + PLAYER_SPRITE_WIDTH));
    float closestY = clamp(bCircle.y, (float)pRect.y, (float)(pRect.y + PLAYER_SPRITE_SRC_HEIGHT));

    // Vector from closest point to ball center
    Vec2 delta(bCircle.x - closestX, bCircle.y - closestY);
    float dist = delta.magnitude();

    // Check for overlap
    float overlap = ball->radius - dist;
    if (overlap <= 0.0f) return; // no collision

    // Normalization (direction)
    Vec2 normal = delta.normalize();
    // printf("%f, %f\n", normal.x, normal.y);

    // Separate objects (prevent sinking) 
    float separation_factor = 1.2f; // tweak this for stability
    float player_ratio = 0.1f;      // how much player is pushed back
    float ball_ratio   = 1.0f - player_ratio;

    // Proposed new ball position
    float newBallX = ball->position.x + normal.x * overlap * ball_ratio * separation_factor;
    float newBallY = ball->position.y + normal.y * overlap * ball_ratio * separation_factor;

    // Check if ball would go out of bounds
    bool ballBlocked = (newBallX - ball->radius < 0) ||
                    (newBallX + ball->radius > SCREEN_WIDTH) ||
                    (newBallY - ball->radius < TOP_PADDING) ||
                    (newBallY + ball->radius > SCREEN_HEIGHT);

    if (ballBlocked)
    {
        ball->change_position(
            player->position.x - normal.x * overlap * 2 * separation_factor,
            player->position.y - normal.y * overlap * 2 * separation_factor
        );
    }
    else
    {
        player->change_position(
            player->position.x - normal.x * overlap * player_ratio * separation_factor,
            player->position.y - normal.y * overlap * player_ratio * separation_factor
        );

        ball->change_position(newBallX, newBallY);
    }
    

    // Velocity response (impulse)
    // Relative velocity
    Vec2 relVel = ball->velocity - player->velocity;
    float velAlongNormal = dot(relVel, normal);
    
    float bounce = (game->map == MOON) ? BOUNCE_FACTOR_MOON : BOUNCE_FACTOR_EARTH; // restitution (0 = no bounce, 1 = full bounce)

    bounce = 0;
    float impulse = -(1.0f + bounce/2) * velAlongNormal;
    // You can divide by "mass" if you simulate it. For now we just apply to ball.
    ball->velocity.x += impulse * normal.x;
    ball->velocity.y += impulse * normal.y;
    // printf ("%f, %f\n", ball->velocity.x, ball->velocity.y);

    // Gameplay tweak (kick if player is fast) ---
    float playerSpeed = player->velocity.magnitude();
    if (playerSpeed > SHOOT_THRESHOLD) { // strong shoot threshold
        ball->velocity += normal * (playerSpeed * 0.5f); // add extra force
    }
    else // dribbing (move slowly with ball)
    {
        ball->velocity *= (1 - player->ball_control);
    }

    if (is_in_opponent_field(player))
    {
        ball->velocity *= player->power;
    }
    else
    {
        ball->velocity *= 1/player->ball_control;
    }

    // if (ballBlocked) ball->velocity*=-3;
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

bool is_in_opponent_field(Player * player)
{
    TEAM_CODE team = player->team;
    int x = (team == RED) ? SCREEN_WIDTH*3/4 : 0;
    int y = 0;

    SDL_Rect opponent_field = {x, y, SCREEN_WIDTH/4, SCREEN_HEIGHT - TOP_PADDING};

    return SDL_HasIntersection(&player->rect, &opponent_field);
}

void process_player_hit_border(Gameplay * game, Player * player)
{
    SDL_Rect rect = player->rect;
    float bounce = (game->map == MOON) ? BOUNCE_FACTOR_MOON : BOUNCE_FACTOR_EARTH;
    // position process:
    // printf ("%d, %d\n", rect.x, rect.y);
    if (rect.x <=0) 
    { 
        player->change_x(1+PLAYER_SPRITE_WIDTH/2); 
        player->velocity.x = -player->velocity.x*(1-bounce); 
    }
    if (rect.y <=TOP_PADDING) 
    { 
        player->change_y(TOP_PADDING+1+PLAYER_SPRITE_HEIGHT/2); 
        player->velocity.y = -player->velocity.y*(1-bounce); 
    }
    if (rect.x + PLAYER_SPRITE_WIDTH >= SCREEN_WIDTH)
    {
        player->change_x(SCREEN_WIDTH - PLAYER_SPRITE_WIDTH/2 - 1);
        player->velocity.x = -player->velocity.x*(1-bounce);
    }
    if (rect.y + PLAYER_SPRITE_HEIGHT >= SCREEN_HEIGHT)
    { 
        player->change_y(SCREEN_HEIGHT - PLAYER_SPRITE_HEIGHT/2 - 1); 
        player->velocity.y = -player->velocity.y*(1-bounce);
    }
}
void process_ball_hit_border(Gameplay * game, Ball * ball)
{
    Vec2 tangent(0,0);
    bool ball_blocked = false;

    SDL_Rect display_rect = ball->display_rect;
    float radius = ball->radius;
    float bounce = (game->map == MOON) ? BOUNCE_FACTOR_MOON : BOUNCE_FACTOR_EARTH;
    // position process:
    if (display_rect.x <=0) 
    { 
        ball->change_x(1+radius);
        tangent.x = 0;
        tangent.y = 1;// vertical
        ball_blocked = true;
    }
    if (display_rect.y <=TOP_PADDING)
    { 
        ball->change_y(1+TOP_PADDING+radius); 
        tangent.x = 0;
        tangent.y = 1;// vertical
        ball_blocked = true;
    }
    if (display_rect.x + radius*2 >= SCREEN_WIDTH)
    { 
        ball->change_x(SCREEN_WIDTH - radius-1); 
        tangent.x = 1;
        tangent.y = 0;// vertical
        ball_blocked = true;
    }
    if (display_rect.y + radius*2 >= SCREEN_HEIGHT)
    { 
        ball->change_y(SCREEN_HEIGHT - radius-1); 
        tangent.x = 1;
        tangent.y = 0;// vertical
        ball_blocked = true;
    }

    if (ball_blocked) 
        ball->velocity = tangent * dot(ball->velocity, tangent) * -1;
}

Player * player_hold_ball(Gameplay * game)
{
    int min_index = 0;
    float min = SCREEN_WIDTH;
    TEAM_CODE min_team = RED;

    for (int i = 0; i<NUMBER_OF_PLAYER; i++)
    {
        Player * red_member = game->red.members[i];
        Player * blue_member = game->blue.members[i];
        float red_dis = Vec2(game->ball.position.x - red_member->position.x, 
            game->ball.position.y - red_member->position.y).magnitude();
        // printf("red: %d, %f\n", i, red_dis);
        float blue_dis = Vec2(game->ball.position.x - blue_member->position.x, 
            game->ball.position.y - blue_member->position.y).magnitude();
        // printf("blue: %d, %f\n", i, blue_dis); 
    
        if (red_dis<min) 
        {
            min_index = i;
            min = red_dis;
            min_team = RED;
        }

        if (blue_dis<min) 
        {
            min_index = i;
            min = blue_dis;
            min_team = BLUE;
        }
    }

    if (min_team == RED)
    {
        return game->red.members[min_index];
    }
    else 
    {
        return game->blue.members[min_index];
    }
}

Player * get_teammate(Player * player, Gameplay * game)
{
    if (player->team == RED)
    {
        for (int i = 0; i<NUMBER_OF_PLAYER; i++)
        {
            if (game->red.members[i]!=player) return game->red.members[i];
        }
    }
    else
    {
        for (int i = 0; i<NUMBER_OF_PLAYER; i++)
        {
            if (game->blue.members[i]!=player) return game->blue.members[i];
        }
    }
}