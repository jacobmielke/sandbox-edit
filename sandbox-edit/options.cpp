#include "options.h"

// Global Options
bool WIREFRAME = false;
bool LEFT_PRESSED = false;
float WORLD_SCALE = 1.0f;
int HEIGHTMAP_INDEX = 0;
bool HEIGHTMAP_RELOAD = false;
bool HEIGHTMAP_EXPORT = false;
char CUSTOM_FILEPATH[150] = "export/";
// TOOL OPTIONS
int CURRENT_TOOL = 2; // -1 LOWER, 0 PLACE, 1 RAISE, 2 VIEW MODE
float TOOL_INTENSITY = 0.001f;
float TOOL_RADIUS = 3.0f;
float TOOL_OPACITY = 1.0f;
const float TOOL_INTENSITY_MAX = 1.0f;
const float TOOL_INTENSITY_MIN = 0.001f;
const float TOOL_RADIUS_MAX = 1000.0f;
const float TOOL_RADIUS_MIN = 0.1f;
const float TOOL_OPACITY_MAX = 1.0f;
const float TOOL_OPACITY_MIN = 0.1f;

// MASS PLACE OPTIONS
bool POP_STACK = false;
int SELECTED_ITEM_INDEX_OBJECTS = 0;
int SELECTED_ITEM_INDEX_FOLIAGE = 0;
bool WORLD_PLACE_OBJECTS = false;
int WORLD_PLACE_INTENSITY = 5;
float WORLD_PLACE_RADIUS = 10.0f;
float WORLD_SPAWN_MIN = -10.0f;
float WORLD_SPAWN_MAX = 10.0f;
float WORLD_PLACE_OFFSET = -0.2;
int PLACE_NUMBER = 10000;

const int PLACE_NUMBER_MAX = 300000;
const int PLACE_NUMBER_MIN = 1;
const int WORLD_PLACE_INTENSITY_MAX = 100;
const int WORLD_PLACE_INTENSITY_MIN = 1;
float WORLD_RENDER_DISTANCE = 100.0f;

const float WORLD_PLACE_MIN = -16.0f;
const float WORLD_PLACE_MAX = 32.0f;

// Placeable Options 
bool PLACEABLE_POP = false;
bool PLACEABLE_YSTRAIN = true;
bool PLACE_OBJECT = false;
float PLACEABLE_SCALE = 0.5f;
int PLACEABLBE_ROTATION = 0;
int PLACEABLE_MENU_INDEX = 0;
bool PLACEABLE_ROT_X = 0;
bool PLACEABLE_ROT_Y = 1;
bool PLACEABLE_ROT_Z = 0;