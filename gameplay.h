#pragma once

#include "settings.h"
#include "utils.h"
#include "object.h"

class Team
{
public:
    TEAM_CODE code;
    std::vector<Player*> members;
    int active_player;
    int score;

    Team()
        : code(RED), members(0), active_player(0), score(0){}
    Team(TEAM_CODE code, std::vector<Player*> players)
        : code(code), members(players), active_player(0), score(0) {}
    ~Team()
    {
        for (int i = 0; i<members.size(); i++)
        {
            if (members[i]!=nullptr)
            {
                delete members[i];
            }
        }
    }

    void set_members(std::vector<Player*> players);

    void set_team(TEAM_CODE t);
};

class Gameplay
{
public:
    GAME_MODE mode;
    GAME_MAP map;
    Team blue, red;
    Ball ball;

    Uint64 start_time;

    void process(float delay);
    // process collision
    //   moving ai
    //   player vs player
    //   player vs ball
    //   is ball in goal
    //   --> scoring

    void init(GAME_MAP map, std::vector<Player*> red_members, std::vector<Player*> blue_members);

    void new_play();
    // place player into position
    // place ball to center

    void rematch();
    // new play
    // reset score, clock
};

void process_player_collision(Player * player1, Player * player2);
bool is_player_collision(Player * player1, Player * player2);

void process_shoot_collision(Player* player, Ball * ball);
bool is_player_shoot(Player * player, Ball* ball);

bool is_ball_in_goal(Ball* ball, int * red_score, int * blue_score);

