#include "../header/GameData.h"

// int GameData::PREV_WIDTH = 1024;
// int GameData::PREV_HEIGHT = 600;
// int GameData::WIDTH = 1024;
// int GameData::HEIGHT = 600;
int GameData::BASE_WIDTH = 1072;
int GameData::BASE_HEIGHT = 603;
int GameData::PREV_WIDTH = 1072;
int GameData::PREV_HEIGHT = 603;
int GameData::WIDTH = 1072;
int GameData::HEIGHT = 603;
const char* GameData::TITLE = "O Som Da Lua";

bool GameData::playerVictory = false;
bool GameData::isFullScreen = false;
Vec2 GameData::playerPos = Vec2(0, 0);
float GameData::targetFPS = 60.0f;
float GameData::currentFPS = 0.0f;
float GameData::delay = 1000.0f / targetFPS;

const string GameData::objectsPath = "./assets/image/world/";

int32_t GameData::fullscreenUpdateCounter = 0;
