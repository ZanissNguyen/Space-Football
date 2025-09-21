#pragma once

#include <SDL.h>
#include <math.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

enum ERROR_CODE {
    IMAGE_NOT_FOUND = 0,
    CANNOT_CREATE_TEXTURE
};

enum GAME_STATE {
    MENU = 0,
    CHOOSE_MAP,
    CHOOSE_PLAYER,
    RECHECK,
    PLAYING,
    SCORING,
    HELP
};

enum GAME_MODE {
    PVP = 0,
    PVE
};

enum TEAM_CODE {
    RED = 0,
    BLUE,
};

enum GAME_MAP {
    EARTH = 0,
    MOON,
    SPACE
};

extern const std::string IMAGE_PATH;
extern const std::string AUDIO_PATH;

extern const int SCREEN_WIDTH;
extern const int TOP_PADDING;
extern const int SCREEN_HEIGHT;
extern const int FPS;

extern const Uint64 GAME_TIME;

extern const int PLAYER_SIZE;
extern const int PLAYER_SIZE_SPACE;
extern const int PLAYER_SPRITE_WIDTH;
extern const int PLAYER_SPRITE_HEIGHT;
extern const int PLAYER_SPRITE_SRC_WIDTH;
extern const int PLAYER_SPRITE_SRC_HEIGHT;

// Limb sprite positions
extern const int BLUE_ARM_X;
extern const int BLUE_ARM_Y;
extern const int BLUE_ARM_WIDTH;
extern const int BLUE_ARM_HEIGHT;
extern const int BLUE_LEG_X;
extern const int BLUE_LEG_Y;
extern const int BLUE_LEG_WIDTH;
extern const int BLUE_LEG_HEIGHT;
extern const int RED_ARM_X;
extern const int RED_ARM_Y;
extern const int RED_ARM_WIDTH;
extern const int RED_ARM_HEIGHT;
extern const int RED_LEG_X;
extern const int RED_LEG_Y;
extern const int RED_LEG_WIDTH;
extern const int RED_LEG_HEIGHT;

extern const float BASE_ACCELERATION;
extern const int NUMBER_OF_PLAYER;

extern const int BALL_SIZE;
extern const int BALL_SIZE_SPACE;

extern const float FRICTION_EARTH;
extern const float FRICTION_MOON;
extern const float FRICTION_SPACE; // mars