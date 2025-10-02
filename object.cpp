#include "object.h"
#include "gameplay.h"

// ======================= Player Class ==========================
void Ball::move(Gameplay * game, float dt)
{
    float friction = FRICTION_EARTH;
    if (game->map == MOON) {
        friction = FRICTION_MOON;
    }
    velocity *= (1.0f - friction * dt);

    if (abs(velocity.x) < 0.05) {velocity.x = 0;}
    if (abs(velocity.y) < 0.05) {velocity.y = 0;}

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
    if(speed > 5.0f) { // Add upper bound safety
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

void Player::move(Gameplay * game, float dt)
{
    acceleration = acceleration.normalize() * BASE_ACCELERATION;

    if (role == "striker")
    {
        if (is_in_opponent_field(this))
        {
            acceleration *= 1.2;
        }
    }
    else // defender 
    {
        if (!is_in_opponent_field(this))
        {
            acceleration *= 1.2;
        }
    }

    float friction = FRICTION_EARTH;
    float accel_scale = 1.0f;
    if (game->map == MOON) {
        friction = FRICTION_MOON;
        accel_scale = 0.5f; // slower acceleration
    }
    // dv = a * dt
    velocity += acceleration * dt * accel_scale * movement_speed;
    velocity *= (1.0f - friction * dt);

    if (abs(velocity.x) < 0.05) {velocity.x = 0;}
    if (abs(velocity.y) < 0.05) {velocity.y = 0;}

    // Apply max speed limit
    float speed = velocity.magnitude();
    if (speed > MAX_PLAYER_SPEED*movement_speed) {
        float scale = MAX_PLAYER_SPEED*movement_speed / speed;
        velocity.x *= scale;
        velocity.y *= scale;
    }

    // dx = x + dv
    Vec2 new_position = position + velocity * dt;
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

    acceleration = Vec2(0, 0);
}

void Player::change_x(int init_x)
{
    position.x = init_x;
    rect.x = init_x - PLAYER_SPRITE_WIDTH/2;
}

void Player::change_y(int init_y)
{
    position.y = init_y;
    rect.y = init_y - PLAYER_SPRITE_HEIGHT/2;
}

void Player::change_position(int init_x, int init_y)
{
    change_x(init_x);
    change_y(init_y);
}

void Striker::AI_Support(Gameplay * game)
{
    if (is_stunned) return;

    // state don't have ball 
    // chasing opponent player that have ball
    // if is the player closest ball, find spot into opponent goal
    // if close enough chasing the ball
    Ball * ball = &game->ball;
    Player * closest_ball = player_hold_ball(game);

    float velocityMag = velocity.magnitude();
    float agility = 1.0f - (velocityMag / MAX_PLAYER_SPEED * movement_speed);
    agility = clamp(agility, 0.0f, 1.0f);

    if (closest_ball == this) // already closest to the ball, find a spot to control/shoot
    {
        Vec2 opponent_goal = Vec2((team == RED) ? SCREEN_WIDTH : 0,
            (SCREEN_HEIGHT-TOP_PADDING)/2.0 + TOP_PADDING);
        Vec2 ball_to_goal = Vec2(opponent_goal.x - ball->position.x
            , opponent_goal.y - ball->position.y);
        Vec2 spot_to_shoot = ball->position - ball_to_goal.normalize() * BALL_SIZE;

        Vec2 direction = Vec2(spot_to_shoot.x - position.x, spot_to_shoot.y - position.y).normalize();
        
        float alignment = dot(ball_to_goal.normalize(), (ball->position - position).normalize());
        if (alignment > 0.259f) {
            // Already on good side -> prepare to shoot
            Vec2 spot_to_shoot = ball->position - ball_to_goal.normalize() * BALL_SIZE;
            Vec2 direction = (spot_to_shoot - position).normalize();
            acceleration = direction * BASE_ACCELERATION;
        } 
        else {
            float minAngle = 35.0f; // sharp turns if agile
            float maxAngle = 60.0f; // wide arc if clumsy
            float rotationAngle = maxAngle - agility * (maxAngle - minAngle);

            // Reposition around ball (support spot behind it)
            Vec2 support_spot = ball->position - ball_to_goal.normalize() * 2.0f * BALL_SIZE;
            Vec2 direction = (support_spot - position).normalize();
            float opr = (position.y - support_spot.y < 0) ? -1.0f : 1.0f;
            Vec2 direction2 = rotate(direction, rotationAngle);
            acceleration = direction2 * BASE_ACCELERATION;
        }
    
    }
    else // other player hold ball
    {
        if (closest_ball->team != team)
        {
            Vec2 direction = Vec2(closest_ball->position.x - position.x
                , closest_ball->position.y - position.y).normalize();

            acceleration = direction * BASE_ACCELERATION;
            return;
        }
        else // your teammate hold the ball
        {
            Vec2 predicted_position = (closest_ball->position + closest_ball->velocity*60);
            Vec2 direction = Vec2(predicted_position.x - position.x, 
                predicted_position.y - position.y).normalize();

            acceleration = direction * BASE_ACCELERATION;
            return;
        }
    }
}

// defense
void Defender::AI_Support(Gameplay * game)
{
    if (is_stunned) return;

    // state don't have ball 
    //  chasing opponent player that have ball
    // if is the player closest ball, find spot into opponent goal
    // if close enough chasing the ball
    Ball * ball = &game->ball;
    Player * closest_ball = player_hold_ball(game);

    float velocityMag = velocity.magnitude();
    float agility = 1.0f - (velocityMag / MAX_PLAYER_SPEED * movement_speed);
    agility = clamp(agility, 0.0f, 1.0f);

    if (closest_ball == this) // already closest to the ball, find a spot to control/shoot
    {
        // like striker logic
        Vec2 opponent_goal = Vec2((team == RED) ? SCREEN_WIDTH : 0,
            (SCREEN_HEIGHT-TOP_PADDING)/2.0 + TOP_PADDING);
        Vec2 ball_to_goal = Vec2(opponent_goal.x - ball->position.x
            , opponent_goal.y - ball->position.y);
        Vec2 spot_to_shoot = ball->position - ball_to_goal.normalize() * BALL_SIZE;

        Vec2 direction = Vec2(spot_to_shoot.x - position.x, spot_to_shoot.y - position.y).normalize();
        
        float alignment = dot(ball_to_goal.normalize(), (ball->position - position).normalize());
        if (alignment > 0.259f) {
            // Already on good side -> prepare to shoot
            Vec2 spot_to_shoot = ball->position - ball_to_goal.normalize() * BALL_SIZE;
            Vec2 direction = (spot_to_shoot - position).normalize();
            acceleration = direction * BASE_ACCELERATION;
        } 
        else {
            float minAngle = 35.0f; // sharp turns if agile
            float maxAngle = 60.0f; // wide arc if clumsy
            float rotationAngle = maxAngle - agility * (maxAngle - minAngle);

            // Reposition around ball (support spot behind it)
            Vec2 support_spot = ball->position - ball_to_goal.normalize() * 2.0f * BALL_SIZE;
            Vec2 direction = (support_spot - position).normalize();
            float opr = (position.y - support_spot.y < 0) ? -1.0f : 1.0f;
            Vec2 direction2 = rotate(direction, rotation_angle * opr);
            acceleration = direction2 * BASE_ACCELERATION;
        }
    
    }
    else // other player hold ball
    {
        if (closest_ball->team != team)
        {
            Vec2 our_goal = Vec2((team == RED) ? 0 : SCREEN_WIDTH,
                (SCREEN_HEIGHT-TOP_PADDING)/2.0 + TOP_PADDING);
            Vec2 opponent_position = closest_ball->position;
            Vec2 opponent_to_goal = our_goal - opponent_position;
            
            if (opponent_to_goal.magnitude() > 300.0f)
            {
                Vec2 defend_position = opponent_position + opponent_to_goal * 0.5f;
                Vec2 direction = (defend_position - position).normalize();

                acceleration = direction * BASE_ACCELERATION;
            }
            else
            {
                Vec2 direction = (ball->position - position).normalize();

                acceleration = direction * BASE_ACCELERATION;
            }

            return;
        }
        else // your teammate hold the ball
        {
            // find closest opponent player and go to defend spot between opponent and your goal
            Vec2 our_goal = Vec2((team == RED) ? 0 : SCREEN_WIDTH,
                (SCREEN_HEIGHT-TOP_PADDING)/2.0 + TOP_PADDING);
            Vec2 opponent_position = get_closest_opponent(this, game)->position;
            Vec2 opponent_to_goal = our_goal - opponent_position;

            float opr = ((opponent_to_goal.magnitude() < (position-our_goal).magnitude())) 
                ? 1.0f : -1.0f;

            Vec2 defend_position = opponent_position + opponent_to_goal * 0.5f * opr;
            Vec2 direction = (defend_position - position).normalize();

            acceleration = direction * BASE_ACCELERATION;
            return;
        }
    }
}

void EffectManager::ApplyEffect(Uint32 duration, EffectFunc applyFunc, EffectFunc expiredFunc, void* object) {
    applyFunc(object);

    // Prepare data for callback
    EffectData* data = new EffectData{expiredFunc, object};

    // Schedule expiration after duration
    SDL_AddTimer(duration, TimerCallback, data);
}

Uint32 EffectManager::TimerCallback(Uint32 interval, void* param) {
    EffectData* data = static_cast<EffectData*>(param);

    // Call the expired function
    data->expired(data->object);

    // Cleanup
    delete data;

    return 0; // do not repeat
}

void applyStunEffect(Player * player)
{
    EffectManager::ApplyEffect(STUN_DURATION, applyStun, expireStun, player);
}

void applyStun(void * obj)
{
    Player * p = static_cast<Player*>(obj);
    p->is_stunned = true;
}
void expireStun(void * obj)
{
    Player * p = static_cast<Player*>(obj);
    p->is_stunned = false;
}

void applySlowEffect(Player * player)
{
    EffectManager::ApplyEffect(SLOW_DURATION, applySlow, expireSlow, player);
}

void applySlow(void * obj)
{
    Player * p = static_cast<Player*>(obj);
    p->movement_speed *= (1 - SLOW_EFFECT);
}
void expireSlow(void * obj)
{
    Player * p = static_cast<Player*>(obj);
    p->movement_speed /= (1 - SLOW_EFFECT);
}