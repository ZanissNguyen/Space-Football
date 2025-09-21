#include "object.h"

// ======================= Player Class ==========================
void Ball::move(float dt)
{
    velocity *= (1.0f - FRICTION_EARTH * dt);

    // dx = x + dv
    Vec2 new_position = position + velocity * dt;
    change_position(new_position.x, new_position.y);

    int FRICTION_EARTH = 0.5;

    // position process:
    if (display_rect.x <=0) { change_x(1+radius); velocity.x = -velocity.x*(1-FRICTION_EARTH); }
    if (display_rect.y <=120) { change_y(121+radius); velocity.y = -velocity.y*(1-FRICTION_EARTH); }
    if (display_rect.x + radius*2 >= SCREEN_WIDTH) 
        { change_x(SCREEN_WIDTH - radius); velocity.x = -velocity.x*(1-FRICTION_EARTH);}
    if (display_rect.y + radius*2 >= SCREEN_HEIGHT) 
        { change_y(SCREEN_HEIGHT - radius); velocity.y = -velocity.y*(1-FRICTION_EARTH);}

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
    // dv = a * dt
    velocity += acceleration * dt;
    velocity *= (1.0f - FRICTION_EARTH * dt);

    // dx = x + dv
    Vec2 new_position = position += velocity * dt;
    change_position(new_position.x, new_position.y);

    int FRICTION_EARTH = 0.5;

    // Calculate rotation and animation based on velocity
    if (velocity.x != 0 || velocity.y != 0) {
        rotation_angle = atan2(velocity.y, velocity.x) * 180.0 / M_PI;
        is_moving = true;
        animation_time += dt * 8.0f; // Animation speed multiplier
    } else {
        is_moving = false;
        animation_time = 0.0f; // Reset animation when not moving
    }

    // position process:
    if (rect.x <=0) { change_x(1+PLAYER_SPRITE_WIDTH/2.0); velocity.x = -velocity.x*(1-FRICTION_EARTH); }
    if (rect.y <=120) { change_y(121+PLAYER_SPRITE_HEIGHT/2.0); velocity.y = -velocity.y*(1-FRICTION_EARTH); }
    if (rect.x + PLAYER_SPRITE_WIDTH >= SCREEN_WIDTH)
        { change_x(SCREEN_WIDTH - PLAYER_SPRITE_WIDTH/2.0); velocity.x = -velocity.x*(1-FRICTION_EARTH);}
    if (rect.y + PLAYER_SPRITE_HEIGHT >= SCREEN_HEIGHT)
        { change_y(SCREEN_HEIGHT - PLAYER_SPRITE_HEIGHT/2.0); velocity.y = -velocity.y*(1-FRICTION_EARTH);}

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
