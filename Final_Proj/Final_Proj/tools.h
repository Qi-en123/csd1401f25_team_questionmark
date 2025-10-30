#ifndef TOOLS_H
#define TOOLS_H

#define tool_range 30

#include "utils.h" //to use Vector2
#include "cprocessing.h" //to use CP_color

typedef enum Tool_name {
	WATERING_CAN,
	SOIL,
	SEEDS,//MIGHT BE IN ANOTHER FUNCTION???? 
	HAND, //means nothing (only used to check what player is holding)

}Tool_name;

typedef struct Tool {
	Tool_name tool_name;
	Vector2 position;
	Vector2 initial_pos;
	float rotation;
	CP_Image tool_sprite;
	bool in_range; //checks if player is within range of tool
	bool rotatable;
}Tool;

void initialize_tools(Tool* tools, int size, Tool_name* tool_names, Tool* held_tool);

void render_tools(Tool* tools, int size, float player_posx, float player_posy, Tool_name held_tool);
void render_held_tool(Tool* tool);

#endif // !TOOLS_H
