#include "tools.h"
#include "cprocessing.h"
#include "utils.h"

//Initializes the tools needed/unlocked for each level
void initialize_tools(Tool* tools, int size, Tool_name* tool_names, Tool* held_tool) {
#define toolStartPosX 100
#define toolStartPosY 200
	//each tool diff initialpos and different sprites
	//this function just to set the initial pos of each tool
	//nid to check how many tools are in the level and which ones

	for (int i = 0; i < size; i++) {
		//Setting the positions of the tools
		Tool *tool = &tools[i];
		tool->initial_pos.x = tool->position.x = toolStartPosX;
		tool->initial_pos.y = tool->position.y = toolStartPosY + 120.0f * i;
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

//Render unheld tools and checks if player is within range of them
void render_tools(Tool* tools, int size, float player_posx, float player_posy, Tool_name held_tool) {
#define SPRITE_WIDTH 16
#define SPRITE_HEIGHT 16

	for (int i = 0; i < size; i++) {
		//if tool is not held
		if (held_tool != tools[i].tool_name) {
			//if player is in range
			if (AreCirclesIntersecting(player_posx, player_posy, 9.0f,
				tools[i].position.x, tools[i].position.y, 40.0f + tool_range)) {
				tools[i].in_range = true;
			}
			else {
				tools[i].in_range = false;
			}
			CP_Image_Draw(tools[i].tool_sprite, tools[i].position.x, tools[i].position.y,
				SPRITE_WIDTH * 2.8f, SPRITE_HEIGHT * 2.8f, 255);
		}
	}
}

//renders the held tool seperately
void render_held_tool(Tool* tool) {
	
	CP_Image_DrawAdvanced(tool->tool_sprite, tool->position.x, tool->position.y,
		SPRITE_WIDTH * 2.1f, SPRITE_HEIGHT * 2.1f, 255, tool->rotation * tool->rotatable);
}

void use_tool(Tool* tool) {
	//change rotation
	//maybe do smth to plants here
}
