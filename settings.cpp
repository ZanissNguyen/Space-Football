#include "settings.h"

const std::string IMAGE_PATH = R"(assets\image\)";
const std::string AUDIO_PATH = R"(assets\audio\)";

const int SCREEN_WIDTH = 1472; // 64 * 23
const int SCREEN_HEIGHT = 832; // 64 * 11 + 64 * 2
const int TOP_PADDING = 128;   // 64 * 2
const int FPS = 60;
const Uint64 GAME_TIME = 180000;
const float HALF_DURATION = 4.0f; // 90 seconds per half

const int PLAYER_SIZE = 40;
const int PLAYER_SPACE_SIZE = 20;
const int PLAYER_SPRITE_WIDTH = 29;  // Actual game size
const int PLAYER_SPRITE_HEIGHT = 43; // Actual game size
const int PLAYER_SPRITE_SRC_WIDTH = 21;  // Source image dimensions
const int PLAYER_SPRITE_SRC_HEIGHT = 31; // Source image dimensions

// Blue team limb positions
const int BLUE_ARM_X = 95;
const int BLUE_ARM_Y = 185;
const int BLUE_ARM_WIDTH = 18;  // 113 - 95
const int BLUE_ARM_HEIGHT = 13; // 198 - 185
const int BLUE_LEG_X = 168;
const int BLUE_LEG_Y = 151;
const int BLUE_LEG_WIDTH = 17;
const int BLUE_LEG_HEIGHT = 12;

// Red team limb positions
const int RED_ARM_X = 57;
const int RED_ARM_Y = 186;
const int RED_ARM_WIDTH = 17;  // 74 - 57
const int RED_ARM_HEIGHT = 12; // 198 - 186
const int RED_LEG_X = 0;
const int RED_LEG_Y = 186;
const int RED_LEG_WIDTH = 17;  // 17 - 0
const int RED_LEG_HEIGHT = 12; // 198 - 186
const int BALL_SIZE = 20;
const int BALL_SIZE_EARTH = 10;

const float BASE_ACCELERATION = 1500.0f;
const int NUMBER_OF_PLAYER = 2;

// friction / fps; 6.0 mean decease 10% vel per frame
const float FRICTION_EARTH = 2.0f;
const float FRICTION_MOON = 0.6f;
const float FRICTION_SPACE = 0.1f; // mars
