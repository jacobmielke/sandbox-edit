#pragma once

// Global Options
extern bool WIREFRAME;
extern bool LEFT_PRESSED;
extern float WORLD_SCALE;
extern int HEIGHTMAP_INDEX;
extern bool HEIGHTMAP_RELOAD;
extern bool HEIGHTMAP_EXPORT;
extern char CUSTOM_FILEPATH[150];
// TOOL OPTIONS
extern int CURRENT_TOOL; // -1 LOWER, 0 PLACE, 1 RAISE, 2 VIEW MODE, 3 AVERAGE MODE
extern float TOOL_INTENSITY;
extern float TOOL_RADIUS;
extern float TOOL_OPACITY;
extern const float TOOL_INTENSITY_MAX;
extern const float TOOL_INTENSITY_MIN;
extern const float TOOL_RADIUS_MAX;
extern const float TOOL_RADIUS_MIN;
extern const float TOOL_OPACITY_MAX;
extern const float TOOL_OPACITY_MIN;

// MASS PLACE OPTIONS
extern bool POP_STACK;
extern int SELECTED_ITEM_INDEX_OBJECTS;
extern int SELECTED_ITEM_INDEX_FOLIAGE;
extern bool WORLD_PLACE_OBJECTS;
extern int WORLD_PLACE_INTENSITY;
extern float WORLD_PLACE_RADIUS;
extern float WORLD_SPAWN_MIN;
extern float WORLD_SPAWN_MAX;
extern float WORLD_PLACE_OFFSET;
extern int PLACE_NUMBER;

extern const int PLACE_NUMBER_MAX;
extern const int PLACE_NUMBER_MIN;
extern const int WORLD_PLACE_INTENSITY_MAX;
extern const int WORLD_PLACE_INTENSITY_MIN;
extern float WORLD_RENDER_DISTANCE;

extern const float WORLD_PLACE_MIN;
extern const float WORLD_PLACE_MAX;

// Placeable Options 
extern bool PLACEABLE_POP;
extern bool PLACEABLE_YSTRAIN;
extern bool PLACE_OBJECT;
extern float PLACEABLE_SCALE;
extern int PLACEABLBE_ROTATION;
extern int PLACEABLE_MENU_INDEX;
extern bool PLACEABLE_ROT_X;
extern bool PLACEABLE_ROT_Y;
extern bool PLACEABLE_ROT_Z;