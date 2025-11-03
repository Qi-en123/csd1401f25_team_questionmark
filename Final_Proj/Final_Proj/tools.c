#include "tools.h"
#include "cprocessing.h"
#include "utils.h"

#define TOOLSTARTPOSX 100
#define TOOLSTARTPOSY 200
//Initializes the tools needed/unlocked for each level
void initialize_tools(Tool* tools, int size, Tool_name* tool_names, Tool* held_tool) {
	//each tool diff initialpos and different sprites
	//this function just to set the initial pos of each tool

	for (int i = 0; i < size; i++) {
		//Setting the positions of the tools
		Tool *tool = &tools[i];
		tool->initial_pos.x = tool->position.x = TOOLSTARTPOSX;
		tool->initial_pos.y = tool->position.y = TOOLSTARTPOSY + 120.0f * i;
		tool->tool_name = tool_names[i];
		tool->in_range = false;
		tool->rotation = 0.0f;

		//Setting the sprites/color of the tools
		switch (tool_names[i]) {
		case WATERING_CAN:
			tool->tool_sprite = CP_Image_Load("Assets/tool_sprites/water_bucket_full.png");
			tool->rotatable = false;
			break;
		case SOIL:
			tool->tool_sprite = CP_Image_Load("Assets/tool_sprites/shovel.png");
			tool->rotatable = true;
			break;
		case SEEDS:
			tool->tool_sprite = CP_Image_Load("Assets/tool_sprites/seed_pouch.png");
			tool->rotatable = false;
			break;
		default:
			break;
		}
	}

	held_tool->tool_name = HAND;
}

#define TOOL_WIDTH 16
#define TOOL_HEIGHT 16
//Render unheld tools and checks if player is within range of them
void render_tools(Tool* tools, int size, float player_posx, float player_posy, Tool_name held_tool) {

	for (int i = 0; i < size; i++) {
		//if tool is not held
		if (held_tool != tools[i].tool_name) {
			//if player is in range
			if (AreCirclesIntersecting(player_posx, player_posy, 9.0f,
				tools[i].position.x, tools[i].position.y - TOOL_RANGE_Y_OFFSET, 40.0f + TOOL_RANGE)) {
				tools[i].in_range = true;
			}
			else {
				tools[i].in_range = false;
			}
			CP_Image_Draw(tools[i].tool_sprite, tools[i].position.x, tools[i].position.y,
				TOOL_WIDTH * 2.8f, TOOL_HEIGHT * 2.8f, 255);
		}
	}
}

//renders the held tool seperately in order to render it infront/back of player as needed
void render_held_tool(Tool* tool) {
	if (tool->tool_name != POT) {
		CP_Image_DrawAdvanced(tool->tool_sprite, tool->position.x, tool->position.y,
			TOOL_WIDTH * 2.1f, TOOL_HEIGHT * 2.1f, 255, tool->rotation * tool->rotatable);
	}
}

void use_tool(Tool* tool) {
	//change rotation
	//maybe do smth to plants here
}
