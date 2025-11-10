#ifndef TOOLS_H
#define TOOLS_H

#define TOOL_RANGE 6
#define TOOL_RANGE_Y_OFFSET 30
#define TOOL_SCALE 3

#include "utils.h" //to use Vector2
#include "cprocessing.h" //to use CP_color
#include "plants.h"
#include "map.h"

typedef enum Tool_name {
	WATERING_CAN,
	SOIL,
	SEED_1, //dun judge, dis is easier
	SEED_2,
	SEED_3,
	//POT,
	//HAND, //means nothing (only used to check what player is holding)

}Tool_name;

typedef struct Tool {
	Tool_name tool_name;
	Vector2 position;
	Vector2 initial_pos;
	float rotation;
	CP_Image tool_sprite;
	bool in_range; //checks if player is within range of tool
	bool rotatable;
	bool selected;
	int uses;
}Tool;

void initialize_tools(Tool* tools, int size, Tool_name* tool_names, Tile* tile);

void render_tools(Tool* tools, int size, float player_posx, float player_posy, Tool* held_tool);
void render_held_tool(Tool* tool);

#endif // !TOOLS_H
