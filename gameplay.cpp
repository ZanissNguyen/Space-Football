#include "gameplay.h"

void Team::set_members(std::vector<Player*> players)
{
    for (int i = 0; i<players.size(); i++)
        members.push_back(players[i]);
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
    // TODO: implement collision and gameplay processing
    // is there have time remaining?
    Uint64 now = SDL_GetTicks64();
    if (now - start_time >= GAME_TIME)
    {

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
        // if (is_player_shoot(red.members[i], &ball))
        // {
        //     // printf("red hit ball");
            process_shoot_collision(red.members[i], &ball);
        // }
        // if (is_player_shoot(blue.members[i], &ball))
        // {
            // printf("blue hit ball");
            process_shoot_collision(blue.members[i], &ball);
        // }
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

    Vec2 delta = Vec2(player2->position.x - player1->position.x
        , player2->position.y - player1->position.y);
    float distance = delta.magnitude();

    // Normalize direction vector
    Vec2 direction = delta.normalize();

    // Minimum separation distance (sum of half-widths + small buffer)
    float min_distance = (PLAYER_SIZE + PLAYER_SIZE) / 2.0f + 2.0f;
    float overlap = min_distance - distance; 
    float total_toughness = player1->toughness + player2->toughness;

    if (overlap > 0) {
        // Separate players smoothly based on their toughness
        float separation_factor = 1.0f; // Increased from 0.5f to prevent phase-through

        if (total_toughness > 0) {
            float p1_ratio = player2->toughness / total_toughness;
            float p2_ratio = player1->toughness / total_toughness;

            player1->change_position(player1->position.x-direction.x*overlap * p1_ratio *separation_factor,
                player1->position.y-direction.y*overlap * p1_ratio *separation_factor);

            player2->change_position(player2->position.x+direction.x*overlap * p2_ratio *separation_factor,
                player2->position.y+direction.y*overlap * p2_ratio *separation_factor);

        }

        // // Apply collision response to velocities (bounce effect)
        float bounce_factor = 0.3f;

        // Calculate relative velocity
        float rel_vx = player2->velocity.x - player1->velocity.x;
        float rel_vy = player2->velocity.y - player1->velocity.y;

        // Calculate relative velocity along collision normal
        float vel_along_normal = rel_vx * direction.x + rel_vy * direction.y;

        // Only resolve if objects are moving towards each other
        if (vel_along_normal > 0) return;

        // Apply collision impulse
        float impulse = bounce_factor * vel_along_normal;

        player1->velocity.x += impulse * direction.x;
        player1->velocity.y += impulse * direction.y;
        player2->velocity.x -= impulse * direction.x;
        player2->velocity.y -= impulse * direction.y;
    }

}

bool is_player_collision(Player* player1, Player* player2) {
    // TODO: return true if players collide
    SDL_Rect p1Rect = player1->rect;
    SDL_Rect p2Rect = player2->rect;
    return SDL_HasIntersection(&p1Rect, &p2Rect);
}

void process_shoot_collision(Player* player, Ball* ball) {
    SDL_Rect pRect = player->rect;
    Circle bCircle = ball->circle;

    // Find closest point on rect to circle center
    float closestX = clamp(bCircle.x, (float)pRect.x, (float)(pRect.x + PLAYER_SIZE));
    float closestY = clamp(bCircle.y, (float)pRect.y, (float)(pRect.y + PLAYER_SIZE));

    // Vector from closest point to ball center
    Vec2 delta(bCircle.x - closestX, bCircle.y - closestY);
    float dist = delta.magnitude();

    // Check for overlap
    float overlap = ball->radius - dist;
    if (overlap <= 0.0f) return; // no collision

    // Normalization (direction)
    Vec2 normal = delta.normalize();

    // Separate objects (prevent sinking) 
    float separation_factor = 1.2f; // tweak this for stability
    float player_ratio = 0.2f;      // how much player is pushed back
    float ball_ratio   = 1.0f - player_ratio;

    // Proposed new ball position
    float newBallX = ball->position.x + normal.x * overlap * ball_ratio * separation_factor;
    float newBallY = ball->position.y + normal.y * overlap * ball_ratio * separation_factor;

    // Check if ball would go out of bounds
    bool ballBlocked = (newBallX - ball->radius < 0) ||
                    (newBallX + ball->radius > SCREEN_WIDTH) ||
                    (newBallY - ball->radius < 0) ||
                    (newBallY + ball->radius > SCREEN_HEIGHT);

    if (!ballBlocked) {
        // Normal case: move both
        player->change_position(
            player->position.x - normal.x * overlap * player_ratio * separation_factor,
            player->position.y - normal.y * overlap * player_ratio * separation_factor
        );

        ball->change_position(newBallX, newBallY);
    } else {
        // Ball can’t move, so push player back more
        player->change_position(
            player->position.x - normal.x * overlap * separation_factor,
            player->position.y - normal.y * overlap * separation_factor
        );
    }

    // Step 5: Velocity response (impulse)
    // Relative velocity
    Vec2 relVel = ball->velocity - player->velocity;
    float velAlongNormal = relVel.x * normal.x + relVel.y * normal.y;

    // Only resolve if moving toward each other
    if (velAlongNormal > 0) return;
    
    float bounce = 0.2f; // restitution (0 = no bounce, 1 = full bounce)

    float impulse = -(1.0f + bounce) * velAlongNormal;
    // You can divide by "mass" if you simulate it. For now we just apply to ball.
    ball->velocity.x += impulse * normal.x;
    ball->velocity.y += impulse * normal.y;
    

    // Gameplay tweak (kick if player is fast) ---
    // float playerSpeed = player->velocity.magnitude();
    // if (playerSpeed > 250) { // strong shoot threshold
    //     ball->velocity += normal * (playerSpeed * 0.5f); // add extra force
    // }
}

bool is_player_shoot(Player* player, Ball* ball) {
    // TODO: return true if player shoots ball
    SDL_Rect pRect = player->rect;
    SDL_Rect bRect = ball->display_rect;
    Circle bCircle = ball->circle;
    if (!SDL_HasIntersection(&pRect, &bRect)) return false;
    printf("rect collision\n");

    // ball circle position.x, position.y, radius
    // player rect  (rect.x, rect.y) 
    //              (rect.x + player.width, rect.y) 
    //              (rect.x, rect.y + player_height)
    //              (rect.x + player_width, rect.y + player_height)
    Vec2 delta = Vec2(bCircle.x - player->position.x, bCircle.y - player->position.y);
    float distance = delta.magnitude();
    Vec2 closest_corner = Vec2(0,0);

    if (delta.x > 0)
    {
        printf("player in the left\n");
        closest_corner.x = pRect.x + PLAYER_SIZE /*PLAYER WIDTH*/;
    }
    else closest_corner.x = pRect.x;
    if (delta.y > 0)
    {
        printf("player above\n");
        closest_corner.y = pRect.y + PLAYER_SIZE /*PLAYER_HEIGHT*/;
    }
    else closest_corner.y = pRect.y;

    float corner_distance = Vec2(bCircle.x - closest_corner.x, bCircle.y - closest_corner.y).magnitude();
    printf ("corner distance = %f\n", corner_distance);

    return corner_distance <= ball->radius;
}

bool is_ball_in_goal(Ball* ball, int * red_score, int * blue_score) {
    // TODO: check if ball is inside goal
    int field_width = SCREEN_WIDTH;
    int field_height = SCREEN_HEIGHT - 120;

    int goal_size = 30;
    // Khung thành trái: x <= 10, y trong [250,470]
    if (ball->position.x <= goal_size && 
        ball->position.y >= field_height/3 && ball->position.y <= 2*field_height/3) {
        *blue_score+=1; return true;
    }
    // Khung thành phải: x+size >= 1270, y trong [250,470]
    if (ball->position.x >= SCREEN_WIDTH-goal_size && 
        ball->position.y >= field_height/3 && ball->position.y <= 2*field_height/3) {
        *red_score+=1; return true;
    }
    return false;
}