#ifndef TOOLS_H
#define TOOLS_H

#define tool_range 30

#include "utils.h" //to use Vector2
#include "cprocessing.h" //to use CP_color

typedef enum tool_name {
	WATERING_CAN,
	SOIL,
	SEEDS,//MIGHT BE IN ANOTHER FUNCTION???? 
	HAND, //means nothing (only used to check what player is holding)

}tool_name;

typedef struct tool {
	tool_name tool_name;
	Vector2 position;
	Vector2 initial_pos;
	CP_Color color;
	bool in_range;
}tool;

void initialize_tools(tool* tools, int size, tool_name* tool_names);

void simulate_tools(tool* tools, int size, float player_posx, float player_posy);
#endif // !TOOLS_H
