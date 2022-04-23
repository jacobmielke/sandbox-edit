#pragma once
bool WIREFRAME = false;
bool LEFT_PRESSED = false;

// 2 - View Mode
// 0 - Object Place
// 1 - Raise
// -1 - Lower
int CURRENT_TOOL = 1;
float TOOL_INTENSITY = 0.001f;
float TOOL_RADIUS = 3.0f;
float TOOL_OPACITY = 1.0f;

const float TOOL_INTENSITY_MAX = 100.0f;
const float TOOL_INTENSITY_MIN = 0.001f;
const float TOOL_RADIUS_MAX = 10.0f;
const float TOOL_RADIUS_MIN = 0.1f;
const float TOOL_OPACITY_MAX = 100.0f;
const float TOOL_OPACITY_MIN = 0.1f;
// World Options
float WORLD_SCALE = 1.0f;
int SELECTED_ITEM_INDEX = 0;
bool WORLD_PLACE_OBJECTS = false;
int WORLD_PLACE_INTENSITY = 5;
float WORLD_PLACE_RADIUS = 10.0f;
const int WORLD_MAX_TREE = 400;
const int WORLD_MAX_GRASS = 100000;
const int WORLD_PLACE_INTENSITY_MAX = 100;
const int WORLD_PLACE_INTENSITY_MIN = 1;

const float WORLD_PLACE_RADIUS_MIN = 0.01f;
const float WORLD_PLACE_RADIUS_MAX = 100.0f;