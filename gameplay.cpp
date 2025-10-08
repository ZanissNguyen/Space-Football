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
    
    // Create NEW copies of players instead of sharing pointers
    for (int i = 0; i < players.size(); i++) {
        Player* newPlayer = nullptr;
        if (players[i]->type == "power_shooter") {
            newPlayer = new Power_Shooter(players[i]->position.x, players[i]->position.y, players[i]->team);
        } else if (players[i]->type == "speeder") {
            newPlayer = new Speeder(players[i]->position.x, players[i]->position.y, players[i]->team);
        } else if (players[i]->type == "controller") {
            newPlayer = new Controller(players[i]->position.x, players[i]->position.y, players[i]->team);
        } else if (players[i]->type == "tackle") {
            newPlayer = new Tackle(players[i]->position.x, players[i]->position.y, players[i]->team);
        } else if (players[i]->type == "shield") {
            newPlayer = new Shield(players[i]->position.x, players[i]->position.y, players[i]->team);
        }
        
        if (newPlayer != nullptr) {
            // Copy other properties from original player (velocity will be reset in new_play())
            newPlayer->velocity = players[i]->velocity;
            members.push_back(newPlayer);
        }
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

void Team::cleanup()
{
    for (int i = 0; i<members.size(); i++)
    {
        if (members[i]!=nullptr)
        {
            delete members[i];
        }
    }
    members.clear();
    score = 0;
    active_player = 0;
}

// ---------------- Event -----------------
void Event::init(EVENT_TYPE t, Uint64 dur, Vec2 pos, int rad, Vec2 vel, float stre)
{
    type = t;
    duration = dur;
    position = pos;
    // radius parameter added but not used in our implementation
    radius = rad;
    velocity = vel;
    strength = stre;
    start_time = 0;
    active = false;
}

void Event::process(Gameplay * game)
{
    // apply event effect
    // printf("Event active: %d\n", type); // Commented out to reduce spam

    if (type == WIND)
    {
        // Enhanced Wind Effect with visual representation
        // Wind strength varies with time for more dynamic effect
        float wind_intensity = 1.0f + 0.3f * sin(SDL_GetTicks() * 0.003f); // Oscillating intensity
        // printf("Wwind Strength %f\n", strength);
        Vec2 wind_force = velocity.normalize() * wind_intensity * strength; // Adjusted force multiplier
        
        // Apply wind to all players with slight variation based on mass/type
        for (int i = 0; i < NUMBER_OF_PLAYER; i++)
        {
            // Heavier players (defenders) are less affected by wind
            float player_mass_factor = (game->red.members[i]->type == "tackle" || game->red.members[i]->type == "shield") ? 0.7f : 1.0f;
            game->red.members[i]->velocity += wind_force * player_mass_factor;
            
            player_mass_factor = (game->blue.members[i]->type == "tackle" || game->blue.members[i]->type == "shield") ? 0.7f : 1.0f;
            game->blue.members[i]->velocity += wind_force * player_mass_factor;
        }
        
        // Ball is most affected by wind
        game->ball.velocity += wind_force * 1.2f;
        
        // TODO: Add wind particles/visual effects here
        // - Wind direction arrows
        // - Particle effects showing wind flow
        // - Screen shake effect for strong gusts
    }
    else if (type == BLACK_HOLE)
    {
        // Enhanced Black Hole with moderate physics for 3s event
        const float MAX_PULL_DISTANCE = 1.0f * radius;  // Moderate range
        const float MIN_SAFE_DISTANCE = 0.1f * radius;  // singularity
        const float CRITICAL_DISTANCE = 0.4f * radius;   // Moderate critical zone
        
        // Black hole effect on ball
        Vec2 to_blackhole = position - game->ball.position;
        float distance = to_blackhole.magnitude();
        
        if (distance > MIN_SAFE_DISTANCE)
        {
            // Physics: Moderate pull for 3s effect (1.5x stronger than original)
            float pull_strength;
            if (distance <= CRITICAL_DISTANCE) {
                // Strong pull near center - 1.5x strength
                pull_strength = (CRITICAL_DISTANCE - distance) * 1.5f + 22.5f;
            } else if (distance <= MAX_PULL_DISTANCE) {
                // Weaker pull farther away
                pull_strength = (MAX_PULL_DISTANCE - distance) / (distance * 0.067f) + 10.0f;
            }
            else pull_strength = (distance - MAX_PULL_DISTANCE) / (distance * 0.677f) + 2.0f;
            
            Vec2 pull_direction = to_blackhole.normalize();
            Vec2 pull_force = pull_direction * pull_strength;
            game->ball.velocity += pull_force;
        }
        
        // Black hole effect on players
        for (int i = 0; i < NUMBER_OF_PLAYER; i++)
        {
            // Red team players
            Vec2 to_bh_red = position - game->red.members[i]->position;
            float dist_red = to_bh_red.magnitude();
            
            if (dist_red < MAX_PULL_DISTANCE && dist_red > MIN_SAFE_DISTANCE)
            {
                float pull_strength;
                if (dist_red <= CRITICAL_DISTANCE) {
                    pull_strength = (CRITICAL_DISTANCE - dist_red) * 0.75f + 15.0f;  // Moderate pull (1.5x)
                } else {
                    pull_strength = (MAX_PULL_DISTANCE - dist_red) / (dist_red * 0.1f);  // Moderate strength (1.5x)
                }
                
                // Players resist black hole based on their type - moderate resistance
                float resistance = (game->red.members[i]->type == "tackle" || game->red.members[i]->type == "shield") ? 0.65f : 0.85f;
                
                Vec2 pull_direction = to_bh_red.normalize();
                Vec2 pull_force = pull_direction * pull_strength * resistance;
                game->red.members[i]->velocity += pull_force;
            }
            
            // Blue team players
            Vec2 to_bh_blue = position - game->blue.members[i]->position;
            float dist_blue = to_bh_blue.magnitude();
            
            if (dist_blue < MAX_PULL_DISTANCE && dist_blue > MIN_SAFE_DISTANCE)
            {
                float pull_strength;
                if (dist_blue <= CRITICAL_DISTANCE) {
                    pull_strength = (CRITICAL_DISTANCE - dist_blue) * 0.75f + 15.0f;  // Moderate pull (1.5x)
                } else {
                    pull_strength = (MAX_PULL_DISTANCE - dist_blue) / (dist_blue * 0.1f);  // Moderate strength (1.5x)
                }
                
                float resistance = (game->blue.members[i]->type == "tackle" || game->blue.members[i]->type == "shield") ? 0.65f : 0.85f;
                
                Vec2 pull_direction = to_bh_blue.normalize();
                Vec2 pull_force = pull_direction * pull_strength * resistance;
                game->blue.members[i]->velocity += pull_force;
            }
        }
        
        // TODO: Add black hole visual effects here
        // - Swirling particle effects around the black hole
        // - Distortion effect on screen near black hole
        // - Gravitational lensing visual effect
        // - Warning indicators when objects get too close
    }
}

// ---------------- Gameplay ----------------
void Gameplay::process(float delay) {
    // Handle countdown
    if (countdown_active) {
        countdown_timer -= delay;
        paused_time += delay * 1000;
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
    else return;

    // event handling:
    Uint64 current_time = SDL_GetTicks64();
    Uint64 elapsed = current_time - paused_time - start_time;
    // printf("%d\n", elapsed);
    for (int i = 0; i<events.size(); i++)
    {
        if (elapsed >= events[i].start_time && elapsed < events[i].start_time + events[i].duration)
        {
            // printf("%d, %d, %d\n", elapsed, events[i].start_time, events[i].start_time + events[i].duration);
            events[i].process(this);
            events[i].active = true;
        }
        else events[i].active = false; 
    }

    // AI moving
    for (int i = 0; i<NUMBER_OF_PLAYER; i++)
    {
        // printf ("%d\n", red.active_player);
        if (i!=red.active_player)
            red.members[i]->AI_Support(this);
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
    // player, ball vs goalpost
    // player, ball vs border
    process_player_collision(this, red.members[0], red.members[1]);
    process_player_collision(this, blue.members[0], blue.members[1]);
    for (int i = 0; i<NUMBER_OF_PLAYER; i++)
    {
        process_player_hit_goalposts(this, red.members[i]);
        process_player_hit_goalposts(this, blue.members[i]);
    }
    process_ball_hit_goalposts(this, &ball);

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
    start_time = SDL_GetTicks64();
    paused_time = 0;
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

    // init events
    events.clear();
    // if map earth, always wind, moon always blackhole
    if (map == EARTH)
    {
        int num_winds = random_int(MIN_EVENTS, MAX_EVENTS);
        int period = GAME_TIME / num_winds;
        for (int i = 0; i<num_winds; i++)
        {
            Event wind;
            int x = random_int(-100, 100);
            int y = random_int(-100, 100);
            int strength = random_int(8, 16);
            wind.init(Event::WIND, WIND_DURATION, Vec2(0,0), 0, Vec2(x,y), strength);
            wind.start_time = random_int(i*period, (i+1)*period);
            printf("wind: %d - strength: %d - direction x: %d - y: %d\n", i, strength, x, y);
            printf("start time: %ds\n", wind.start_time/1000);
            events.push_back(wind);
        }
    }
    else if (map == MOON)
    {
        // random number of blackhole 
        int num_blackholes = random_int(MIN_EVENTS, MAX_EVENTS);
        int period = GAME_TIME / num_blackholes;
        for (int i = 0; i<num_blackholes; i++)
        {
            int radius = random_int(200, 288); // random radius between 200 to 288
            Event blackhole;
            int x = SCREEN_WIDTH/2;
            int y = random_int(0, SCREEN_HEIGHT) + TOP_PADDING;
            blackhole.init(Event::BLACK_HOLE, BLACK_HOLE_DURATION, Vec2(x,y), radius, Vec2(0,0), 0);
            blackhole.start_time = random_int(i*period, (i+1)*period);

            printf("Blackhole: %d - Radius: %d\n", i, radius);
            printf("start time: %ds\n", blackhole.start_time/1000);
            events.push_back(blackhole);
        }
    }

    new_play();
}

void Gameplay::new_play() 
{
    int field_width = SCREEN_WIDTH;
    int field_height = SCREEN_HEIGHT - 120;

    // TODO: place players into position and reset ball
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

void Gameplay::cleanup() {
    // remove player *
    // reset score
    // reset clock
    red.cleanup();
    blue.cleanup();
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

    if (player1->team == player2->team) return;

    if (!player1->is_stunned && player2->type=="tackle")
        applyStunEffect(player1);
    
    if (!player2->is_stunned && player1->type=="tackle")
        applyStunEffect(player2);
    
    if (!player1->is_slowed && player2->type=="shield")
        applySlowEffect(player1);
    
    if (!player2->is_stunned && player1->type=="shield")
        applySlowEffect(player2);
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
        player->change_position(
            player->position.x - normal.x * overlap * separation_factor,
            player->position.y - normal.y * overlap * separation_factor
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
        ball->velocity *= 1/player->ball_control; 
    }

    if (is_in_opponent_field(player))
    {
        ball->velocity *= player->power;
    }
    // else
    // {
    //     // ball->velocity *= 1/player->ball_control;
    // }

    if (ballBlocked) ball->velocity*=-1.5;
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
    int goal_depth =  tile_size / 2; // One tile deep

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
    SDL_Rect display_rect = ball->display_rect;
    float radius = ball->radius;
    float bounce = (game->map == MOON) ? BOUNCE_FACTOR_MOON/2.0 : BOUNCE_FACTOR_EARTH/2.0;

    Vec2 normal(0,0);
    bool ball_blocked = false;

    // left border
    if (display_rect.x <= 0) {
        ball->change_x(1 + radius);
        normal = Vec2(1,0); // push right
        ball_blocked = true;
    }
    // top border
    if (display_rect.y <= TOP_PADDING) {
        ball->change_y(1 + TOP_PADDING + radius);
        normal = Vec2(0,1); // push down
        ball_blocked = true;
    }
    // right border
    if (display_rect.x + radius*2 >= SCREEN_WIDTH) {
        ball->change_x(SCREEN_WIDTH - radius - 1);
        normal = Vec2(-1,0); // push left
        ball_blocked = true;
    }
    // bottom border
    if (display_rect.y + radius*2 >= SCREEN_HEIGHT) {
        ball->change_y(SCREEN_HEIGHT - radius - 1);
        normal = Vec2(0,-1); // push up
        ball_blocked = true;
    }

    // reflect velocity if collision happened
    if (ball_blocked) {
        // reflection formula: v' = v - 2*(v·n)*n
        float dotp = dot(ball->velocity, normal);
        ball->velocity = ball->velocity - normal * (2.0f * dotp);

        // apply bounce factor
        ball->velocity *= (1.0f - bounce);
    }
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
    return nullptr;
}

Player * get_closest_opponent(Player * player, Gameplay * game)
{
    if (player->team == RED)
    {
        float player1_distance = Vec2(game->blue.members[0]->position - player->position).magnitude();
        float player2_distance = Vec2(game->blue.members[1]->position - player->position).magnitude();
        return (player1_distance < player2_distance) ? game->blue.members[0] : game->blue.members[1];
    }
    else
    {
        float player1_distance = Vec2(game->red.members[0]->position - player->position).magnitude();
        float player2_distance = Vec2(game->red.members[1]->position - player->position).magnitude();
        return (player1_distance < player2_distance) ? game->red.members[0] : game->red.members[1];
    }
}

void process_player_hit_goalposts(Gameplay * game, Player * player)
{
    int tile_size = 64;
    int field_height = SCREEN_HEIGHT - TOP_PADDING;
    int num_y = field_height / tile_size;
    int goal_top_y = TOP_PADDING + (num_y/2 - 2) * tile_size + 40;
    int goal_bottom_y = TOP_PADDING + (num_y/2 + 2 + 1) * tile_size - 40;

    SDL_Rect left_top_goal = {0, goal_top_y, tile_size-20, 5};
    SDL_Rect left_bottom_goal = {0, goal_bottom_y + 1, tile_size-20, 5};
    SDL_Rect right_top_goal = {SCREEN_WIDTH - tile_size + 20, goal_top_y, tile_size-20, 5};
    SDL_Rect right_bottom_goal = {SCREEN_WIDTH - tile_size + 20, goal_bottom_y + 1, tile_size-20, 5};

    SDL_Rect object = {0, 0, 0, 0};
    bool hit = false;

    // implement collision later
    if (SDL_HasIntersection(&left_top_goal, &player->rect))
    {
        object = left_top_goal;
        hit = true;
    }

    if (SDL_HasIntersection(&left_bottom_goal, &player->rect))
    {
        object = left_bottom_goal;
        hit = true;
    }

    if (SDL_HasIntersection(&right_top_goal, &player->rect))
    {
        object = right_top_goal;
        hit = true;
    }

    if (SDL_HasIntersection(&right_bottom_goal, &player->rect))
    {
        object = right_bottom_goal;
        hit = true;
    }

    if (!hit) return;

    SDL_Rect rect = player->rect;
    float bounce = (game->map == MOON) ? BOUNCE_FACTOR_MOON : BOUNCE_FACTOR_EARTH;
    // position process:
    // printf ("%d, %d\n", rect.x, rect.y);
    // calculate overlap distances
    int overlapLeft   = (rect.x + rect.w) - object.x;
    int overlapRight  = (object.x + object.w) - rect.x;
    int overlapTop    = (rect.y + rect.h) - object.y;
    int overlapBottom = (object.y + object.h) - rect.y;

    // pick smallest overlap
    int minOverlapX = std::min(overlapLeft, overlapRight);
    int minOverlapY = std::min(overlapTop, overlapBottom);

    if (minOverlapX < minOverlapY) {
        // resolve on X axis
        if (overlapLeft < overlapRight) {
            // push player left
            player->change_x(player->position.x - minOverlapX);
        } else {
            // push player right
            player->change_x(player->position.x + minOverlapX);
        }
        // bounce X velocity
        player->velocity.x = -player->velocity.x * (1 - bounce);
    } else {
        // resolve on Y axis
        if (overlapTop < overlapBottom) {
            // push player up
            player->change_y(player->position.y - minOverlapY);
        } else {
            // push player down
            player->change_y(player->position.y + minOverlapY);
        }
        // bounce Y velocity
        player->velocity.y = -player->velocity.y * (1 - bounce);
    }
}

void process_ball_hit_goalposts(Gameplay * game, Ball * ball)
{
    int tile_size = 64;
    int field_height = SCREEN_HEIGHT - TOP_PADDING;
    int num_y = field_height / tile_size;
    int goal_top_y = TOP_PADDING + (num_y/2 - 2) * tile_size + 40;
    int goal_bottom_y = TOP_PADDING + (num_y/2 + 2 + 1) * tile_size - 40;

    SDL_Rect left_top_goal = {0, goal_top_y, tile_size-20, 5};
    SDL_Rect left_bottom_goal = {0, goal_bottom_y + 1, tile_size-20, 5};
    SDL_Rect right_top_goal = {SCREEN_WIDTH - tile_size + 20, goal_top_y, tile_size-20, 5};
    SDL_Rect right_bottom_goal = {SCREEN_WIDTH - tile_size + 20, goal_bottom_y + 1, tile_size-20, 5};

    SDL_Rect object = {0, 0, 0, 0};

    // implement collision later
    if (SDL_HasIntersection(&left_top_goal, &ball->display_rect))
    {
        object = left_top_goal;
    }

    if (SDL_HasIntersection(&left_bottom_goal, &ball->display_rect))
    {
        object = left_bottom_goal;
    }

    if (SDL_HasIntersection(&right_top_goal, &ball->display_rect))
    {
        object = right_top_goal;
    }

    if (SDL_HasIntersection(&right_bottom_goal, &ball->display_rect))
    {
        object = right_bottom_goal;
    }

    float cx = ball->position.x;
    float cy = ball->position.y;
    float r  = ball->radius;

    // 1. Closest point on object
    float closestX = clamp(cx, (float) object.x, (float) (object.x + object.w));
    float closestY = clamp(cy, (float) object.y, (float) (object.y + object.h));

    // 2. Vector from closest point to ball center
    Vec2 delta(cx - closestX, cy - closestY);
    float dist = delta.magnitude();

    float bounce = (game->map == MOON) ? BOUNCE_FACTOR_MOON/2.0 : BOUNCE_FACTOR_EARTH/2.0;

    // 3. Check collision
    if (dist <= r*r) {
        float dist = std::sqrt(dist);
        Vec2 normal(delta.x, delta.y);

        if (dist != 0)
            normal = normal.normalize(); // normalize
        else
            normal = Vec2(1, 0); // arbitrary normal if exactly overlapping

        // Push ball out (separate them)
        float penetration = r - dist;
        ball->change_x(cx + normal.x * penetration);
        ball->change_y(cy + normal.y * penetration);

        // reflect velocity
        float dotp = dot(ball->velocity, normal);
        ball->velocity = ball->velocity - normal * (2.0f * dotp);

        // Apply bounce factor
        ball->velocity *= (1.0f - bounce);
    }
}