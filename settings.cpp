#include "settings.h"

const std::string IMAGE_PATH = R"(assets\image\)";
const std::string AUDIO_PATH = R"(assets\audio\)";

const int SCREEN_WIDTH = 1440;
const int SCREEN_HEIGHT = 800;
const int FPS = 60;
const Uint64 GAME_TIME = 180000;

const int PLAYER_SIZE = 40;
const int PLAYER_SPACE_SIZE = 20;
const int BALL_SIZE = 20;
const int BALL_SIZE_EARTH = 10;

const float BASE_ACCELERATION = 1500.0f;
const int NUMBER_OF_PLAYER = 2;

// friction / fps; 6.0 mean decease 10% vel per frame
const float FRICTION_EARTH = 3.0f;
const float FRICTION_MOON = 0.6f;
const float FRICTION_SPACE = 0.1f; // mars
