#include "object.h"
#include "gameplay.h"

// ======================= Player Class ==========================
void Ball::move(float dt)
{
    extern Gameplay game;
    float friction = FRICTION_EARTH;
    if (game.map == MOON) {
        friction = FRICTION_MOON;
    }
    velocity *= (1.0f - friction * dt);

    // dx = x + dv
    Vec2 new_position = position + velocity * dt;
    change_position(new_position.x, new_position.y);

    // Calculate rotation based on velocity
    float speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (speed > 1.0f) {
        rotation_angle += speed * dt * 2.0f; // Rotation speed based on ball speed
        if (rotation_angle > 360.0f) rotation_angle -= 360.0f;
    }

    // Update particle system for comet trail
    updateParticles(dt, speed);

    float bounce_friction = (game.map == MOON) ? FRICTION_MOON : FRICTION_EARTH;
    // position process:
    if (display_rect.x <=0) { change_x(1+radius); velocity.x = -velocity.x*(1-bounce_friction); }
    if (display_rect.y <=120) { change_y(121+radius); velocity.y = -velocity.y*(1-bounce_friction); }
    if (display_rect.x + radius*2 >= SCREEN_WIDTH)
        { change_x(SCREEN_WIDTH - radius); velocity.x = -velocity.x*(1-bounce_friction);}
    if (display_rect.y + radius*2 >= SCREEN_HEIGHT)
        { change_y(SCREEN_HEIGHT - radius); velocity.y = -velocity.y*(1-bounce_friction);}

    // printf("Circle: position (%f, %f) | vel (%f, %f)\n", position.x, position.y, velocity.x, velocity.y);
}

void Ball::setRadius(float init_r)
{
    radius = init_r;
}

void Ball::place(int init_x, int init_y)
{
    position = Vec2(init_x, init_y);
    velocity = Vec2(0, 0);
    int width = 2*radius;
    display_rect = {(init_x-width/2), (init_y-width/2), width, width};
    circle = Circle(position.x, position.y, radius);
    last_touch = RED;
    rotation_angle = 0.0;
    particle_spawn_timer = 0.0f;

    // Clear all particles when ball is placed
    for(int i = 0; i < MAX_PARTICLES; i++) {
        particle_life[i] = 0.0f;
    }
}

void Ball::change_x(int init_x)
{
    position.x = init_x;
    display_rect.x = init_x-radius;
    circle.x = position.x;
}

void Ball::change_y(int init_y)
{
    position.y = init_y;
    display_rect.y = init_y-radius;
    circle.y = position.y;
}

void Ball::change_position(int init_x, int init_y)
{
    change_x(init_x);
    change_y(init_y);
}

void Ball::updateParticles(float dt, float speed)
{
    // Safety check
    if (dt <= 0.0f || dt > 1.0f) return;

    // Update existing particles
    for(int i = 0; i < MAX_PARTICLES; i++) {
        if(particle_life[i] > 0.0f) {
            particle_life[i] -= dt * 3.0f; // Fade out over ~0.33 seconds
            if(particle_life[i] < 0.0f) particle_life[i] = 0.0f;
        }
    }

    // Spawn new particles when ball is moving fast
    if(speed > 15.0f) { // Add upper bound safety
        particle_spawn_timer += dt;

        // Spawn particles at ~30 FPS when moving fast
        if(particle_spawn_timer >= 1.0f/30.0f) {
            particle_spawn_timer = 0.0f;

            // Find a dead particle to reuse
            for(int i = 0; i < MAX_PARTICLES; i++) {
                if(particle_life[i] <= 0.0f) {
                    // Safety check position values
                    if(position.x >= 0 && position.x < 10000 && position.y >= 0 && position.y < 10000) {
                        particle_x[i] = position.x;
                        particle_y[i] = position.y;
                        particle_life[i] = 1.0f; // Full life
                    }
                    break;
                }
            }
        }
    }
}

// ======================= Player Class ==========================
void Player::place(int init_x, int init_y)
{
    position = Vec2(init_x, init_y);
    velocity = Vec2(0, 0);
    acceleration = Vec2(0, 0);
    rect = {init_x-PLAYER_SPRITE_WIDTH/2, init_y-PLAYER_SPRITE_HEIGHT/2, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT};
    rotation_angle = 0.0;
    animation_time = 0.0f;
    is_moving = false;
}

void Player::move(float dt)
{
    extern Gameplay game;
    float friction = FRICTION_EARTH;
    float accel_scale = 1.0f;
    if (game.map == MOON) {
        friction = FRICTION_MOON;
        accel_scale = 0.5f; // slower acceleration
    }
    // dv = a * dt
    velocity += acceleration * dt * accel_scale;
    velocity *= (1.0f - friction * dt);

    // dx = x + dv
    Vec2 new_position = position += velocity * dt;
    change_position(new_position.x, new_position.y);

    // Calculate rotation and animation based on velocity
    if (velocity.x != 0 || velocity.y != 0) {
        rotation_angle = atan2(velocity.y, velocity.x) * 180.0 / M_PI;
        is_moving = true;
        animation_time += dt * 8.0f; // Animation speed multiplier
    } else {
        is_moving = false;
        animation_time = 0.0f; // Reset animation when not moving
    }

    float bounce_friction = (game.map == MOON) ? FRICTION_MOON : FRICTION_EARTH;
    // position process:
    if (rect.x <=0) { change_x(1+PLAYER_SPRITE_WIDTH/2.0); velocity.x = -velocity.x*(1-bounce_friction); }
    if (rect.y <=120) { change_y(121+PLAYER_SPRITE_HEIGHT/2.0); velocity.y = -velocity.y*(1-bounce_friction); }
    if (rect.x + PLAYER_SPRITE_WIDTH >= SCREEN_WIDTH)
        { change_x(SCREEN_WIDTH - PLAYER_SPRITE_WIDTH/2.0); velocity.x = -velocity.x*(1-bounce_friction);}
    if (rect.y + PLAYER_SPRITE_HEIGHT >= SCREEN_HEIGHT)
        { change_y(SCREEN_HEIGHT - PLAYER_SPRITE_HEIGHT/2.0); velocity.y = -velocity.y*(1-bounce_friction);}

    // printf("position (%f, %f) | vel (%f, %f)\n", position.x, position.y, velocity.x, velocity.y);

    acceleration = Vec2(0, 0);
}

void Player::change_x(int init_x)
{
    position.x = init_x;
    rect.x = init_x - PLAYER_SPRITE_WIDTH/2.0;
}

void Player::change_y(int init_y)
{
    position.y = init_y;
    rect.y = init_y - PLAYER_SPRITE_HEIGHT/2.0;
}

void Player::change_position(int init_x, int init_y)
{
    change_x(init_x);
    change_y(init_y);
}

// void GameAI::updateAI(Player* aiPlayer, Ball* ball, int team, int fieldWidth, int fieldHeight, bool ballInPlay) {
//     SDL_Rect playerRect = aiPlayer->getRect();
//     SDL_Rect ballRect = ball->getRect();
//     int dx = ballRect.x - playerRect.x;
//     int dy = ballRect.y - playerRect.y;
//     float dist = std::sqrt(dx*dx + dy*dy);
//     if (ballInPlay && dist < 60) {
//         // Nếu gần bóng và bóng đang lăn, ưu tiên tấn công (di chuyển về phía khung thành đối phương)
//         int targetX = (team == 2) ? 10 : fieldWidth - 60;
//         int targetY = ballRect.y;
//         int tx = targetX - playerRect.x;
//         int ty = targetY - playerRect.y;
//         if (std::abs(tx) > 5) aiPlayer->move((tx > 0) ? 2 : -2, 0);
//         if (std::abs(ty) > 5) aiPlayer->move(0, (ty > 0) ? 2 : -2);
//     } else {
//         // Nếu không có bóng, di chuyển về phía bóng
//         if (dist > 5) {
//             aiPlayer->move((dx > 0) ? 2 : (dx < 0) ? -2 : 0, (dy > 0) ? 2 : (dy < 0) ? -2 : 0);
//         }
//     }
// }

// void GameAI::supportAI(Player* supportPlayer, Ball* ball, int team, int fieldWidth, int fieldHeight, bool ballInPlay) {
//     // AI hỗ trợ: di chuyển về vị trí phòng thủ/hỗ trợ
//     int targetX = (team == 1) ? fieldWidth/4 : 3*fieldWidth/4;
//     int targetY = fieldHeight/2;
//     SDL_Rect playerRect = supportPlayer->getRect();
//     int dx = targetX - playerRect.x;
//     int dy = targetY - playerRect.y;
//     if (std::abs(dx) > 5) supportPlayer->move((dx > 0) ? 1 : -1, 0);
//     if (std::abs(dy) > 5) supportPlayer->move(0, (dy > 0) ? 1 : -1);
// }

// void GameAI::pressingAI(Player* presser, Ball* ball, const std::vector<Player*>& opponents, int fieldWidth, int fieldHeight) {
//     // Tìm cầu thủ đối phương gần bóng nhất
//     Player* target = nullptr;
//     float minDist = 1e9;
//     SDL_Rect ballRect = ball->getRect();
//     for (auto* opp : opponents) {
//         SDL_Rect oRect = opp->getRect();
//         float dx = oRect.x - ballRect.x;
//         float dy = oRect.y - ballRect.y;
//         float dist = std::sqrt(dx*dx + dy*dy);
//         if (dist < minDist) { minDist = dist; target = opp; }
//     }
//     if (target) {
//         SDL_Rect pRect = presser->getRect();
//         int dx = target->x - pRect.x;
//         int dy = target->y - pRect.y;
//         if (std::abs(dx) > 5) presser->move((dx > 0) ? 2 : -2, 0);
//         if (std::abs(dy) > 5) presser->move(0, (dy > 0) ? 2 : -2);
//     }
// }

// void GameAI::runSupportAI(Player* runner, Ball* ball, int team, int fieldWidth, int fieldHeight) {
//     // Khi có bóng, cầu thủ còn lại chạy chỗ về phía khung thành đối phương
//     int targetX = (team == 1) ? fieldWidth - 100 : 100;
//     int targetY = (ball->y < fieldHeight/2) ? fieldHeight - 100 : 100;
//     SDL_Rect pRect = runner->getRect();
//     int dx = targetX - pRect.x;
//     int dy = targetY - pRect.y;
//     if (std::abs(dx) > 5) runner->move((dx > 0) ? 2 : -2, 0);
//     if (std::abs(dy) > 5) runner->move(0, (dy > 0) ? 2 : -2);
// }
