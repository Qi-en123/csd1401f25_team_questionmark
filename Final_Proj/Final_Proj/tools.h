#pragma once

#include "utils.h" //to use Vector2
#include "cprocessing.h" //to use CP_color
#include "plants.h"
#include "map.h"

#define TOOL_RANGE 6
#define TOOL_RANGE_Y_OFFSET 30
#define TOOL_SCALE 3
#define TOOL_WIDTH 16
#define TOOL_HEIGHT 16

typedef enum Tool_name {
	WATERING_CAN,
	SOIL,
	SEED_1,
	SEED_2,
	SEED_3,
}Tool_name;

typedef struct Tool {
	Tool_name tool_name;
	Vector2 position;
	float rotation;
	CP_Image tool_sprite;
	bool in_range; //checks if player is within range of tool
	bool rotatable;
	bool selected;
	int uses;
	Tile* occupied_tile; //pointer to tile that its occupying, used to de-occupy when picked up
}Tool;

//initialize tool values for each different tool the level has
//assign tools with names, spawn position, type, sprite scale etc.
void initialize_tools(Tool* tools, int size, Tool_name* tool_names, Tile* tile, int uses);

//Draw the tool except the one held by player
void render_tools(Tool* tools, int size, float player_posx, float player_posy, Tool* held_tool, void* held_pot);

//Draw the tool that is held by player, takes in pointer to tool held
void render_held_tool(Tool* tool);