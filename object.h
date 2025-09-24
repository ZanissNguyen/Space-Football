#pragma once
class Gameplay;

#include "settings.h"
#include "utils.h"

// ======================= Ball Class ==========================
class Ball
{
public:
    Vec2 position;
    Vec2 velocity;
    float radius;
    SDL_Rect display_rect;
    Circle circle;
    TEAM_CODE last_touch;
    double rotation_angle; // angle in degrees for ball rotation

    // Simple particle system for comet trail (using separate arrays for safety)
    static const int MAX_PARTICLES = 8;
    float particle_x[MAX_PARTICLES];
    float particle_y[MAX_PARTICLES];
    float particle_life[MAX_PARTICLES];
    float particle_spawn_timer;

    Ball()
        : position(Vec2(0,0)), velocity(Vec2(0,0)), radius(0.0f), display_rect({0,0,0,0}),
        circle(Circle(0,0,0)), last_touch(RED), rotation_angle(0.0), particle_spawn_timer(0.0f)
    {
        // Initialize all particles as dead
        for(int i = 0; i < MAX_PARTICLES; i++) {
            particle_life[i] = 0.0f;
        }
    }

    Ball(int init_x, int init_y, float init_radius)
    {
        position = Vec2(init_x, init_y);
        velocity = Vec2(0, 0);
        radius = init_radius;
        int width = 2*radius;
        display_rect = {(init_x-width/2), (init_y-width/2), width, width};
        circle = Circle(position.x, position.y, radius);
        last_touch = RED;
        rotation_angle = 0.0;
        particle_spawn_timer = 0.0f;

        // Initialize all particles as dead
        for(int i = 0; i < MAX_PARTICLES; i++) {
            particle_life[i] = 0.0f;
        }
    }

    void setRadius(float r);
    void move(Gameplay * game, float dt);
    void place(int init_x, int init_y);
    void change_position(int init_x, int init_y);
    void change_x(int init_x);
    void change_y(int init_y);
    void updateParticles(float dt, float speed);
};

// ======================= Player Class ==========================
class Player
{
public:
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;

    bool is_active;
    SDL_Rect rect;
    TEAM_CODE team;
    double rotation_angle; // angle in degrees for sprite rotation

    // Animation state
    float animation_time; // for limb animation timing
    bool is_moving; // track if player is currently moving

    float movement_speed;
    float toughness; // process collision with player
    float ball_control; // how easy to control ball
    float power; // increase ball velocity when near the opponent goal
    std::string type;

    Player(int init_x, int init_y, TEAM_CODE init_team, std::string init_type)
    {
        position = Vec2(init_x, init_y);
        velocity = Vec2(0, 0);
        acceleration = Vec2(0, 0);
        rect = {init_x-PLAYER_SPRITE_WIDTH/2, init_y-PLAYER_SPRITE_HEIGHT/2, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT};
        team = init_team;
        type = init_type;

        movement_speed = 1;
        ball_control = 1; 
        power = 1;
        toughness = 1;

        rotation_angle = 0.0; // Start facing right
        animation_time = 0.0f;
        is_moving = false;
    }

    void place(int init_x, int init_y);
    void move(Gameplay * game, float dt);
    void change_position(int init_x, int init_y);
    void change_x(int init_x);
    void change_y(int init_y);
    virtual void AI_Support(Gameplay * game) = 0;
};

class Striker: public Player
{
public: 
    Striker(int init_x, int init_y, TEAM_CODE team, std::string type) : Player(init_x, init_y, team, type)
    {
        movement_speed = 1;
        ball_control = 1; 

        power = 1.3;
        toughness = 1;
    }  
    
    void AI_Support(Gameplay * game);
};

class Defender: public Player
{
public:
    Defender(int init_x, int init_y, TEAM_CODE team, std::string type): Player(init_x, init_y, team, type)
    {
        movement_speed = 1;
        ball_control = 1; 

        power = 1;
        toughness = 3;
    }   

    void AI_Support(Gameplay * game);
};

class Power_Shooter: public Striker
{
public:
    Power_Shooter(int init_x, int init_y, TEAM_CODE team): Striker(init_x, init_y, team, "Power_Shooter")
    {
        movement_speed = 1;
        ball_control = 1; 

        power = 2;
        toughness = 1;
    }  
};

class Speeder: public Striker
{
public:
    Speeder(int init_x, int init_y, TEAM_CODE team): Striker(init_x, init_y, team, "Speeder")
    {
        movement_speed = 1.3;
        ball_control = 1; 

        power = 1.2;
        toughness = 0.8;
    }  
};

class Controller: public Striker
{
public:
    Controller(int init_x, int init_y, TEAM_CODE team): Striker(init_x, init_y, team, "Controller")
    {
        movement_speed = 1;
        ball_control = 2; 

        power = 1.2;
        toughness = 1.5;
    }  
};

class Tackle: public Defender
{
public:
    Tackle(int init_x, int init_y, TEAM_CODE team): Defender(init_x, init_y, team, "Tackle")
    {
        movement_speed = 0.9;
        ball_control = 1.3; 

        power = 1;
        toughness = 5;
    }  
};

class Shield: public Defender
{
public:
    Shield(int init_x, int init_y, TEAM_CODE team): Defender(init_x, init_y, team, "Shield")
    {
        movement_speed = 1.1;
        ball_control = 1.3; 

        power = 1;
        toughness = 3;
    }  
};