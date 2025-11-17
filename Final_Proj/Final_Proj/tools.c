#include "tools.h"
#include "cprocessing.h"
#include "utils.h"
#include "plants.h"

//Initializes the tools needed/unlocked for each level
void initialize_tools(Tool* tools, int tool_size, Tool_name* tool_names, Tile* tile) {
	//each tool diff initialpos and different sprites
	//this function just to set the initial pos of each tool
	for (int i = 0; i < tool_size; i++) {
		//Setting the positions of the tools
		Tool *tool = &tools[i];
		tool->initial_pos.x = tool->position.x = tile->x;
		tool->initial_pos.y = tool->position.y = tile[i+TILE_COLS*i*2].y;
		tool->tool_name = tool_names[i];
		tool->in_range = false;
		tool->rotation = 0.0f;
		tool->uses = 3; //only used for watering_can :p
		tool->selected = false; //NAO: change held_tool to pointer

		//Setting the sprites of the tools
		switch (tool_names[i]) {
		case WATERING_CAN:
			tool->tool_sprite = CP_Image_Load("Assets/tool_sprites/water_bucket_full.png");
			tool->rotatable = false;
			break;
		case SOIL:
			tool->tool_sprite = CP_Image_Load("Assets/tool_sprites/shovel.png");
			tool->rotatable = true;
			break;
		case SEED_1: //berrybush
			tool->tool_sprite = CP_Image_Load("Assets/tool_sprites/seed_berrybush.png");
			tool->rotatable = false;
			break;
		case SEED_2: //cactus
			tool->tool_sprite = CP_Image_Load("Assets/tool_sprites/seed_cactus.png");
			tool->rotatable = false;
			break;
		case SEED_3: //orange
			tool->tool_sprite = CP_Image_Load("Assets/tool_sprites/seed_orangebush.png");
			tool->rotatable = false;
			break;
		default:
			break;
		}
	}
	//held_tool->tool_name = HAND;
}

#define TOOL_WIDTH 16
#define TOOL_HEIGHT 16
//Render unheld tools and checks if player is within range of them
void render_tools(Tool* tools, int size, float player_posx, float player_posy, Tool* held_tool) {

	for (int i = 0; i < size; i++) {
		//if tool is not held
		if (&tools[i] == held_tool) { continue; }
		if (AreCirclesIntersecting(player_posx, player_posy, 9.0f,
			tools[i].position.x, tools[i].position.y - TOOL_RANGE_Y_OFFSET, 40.0f + TOOL_RANGE)) {
			tools[i].in_range = true;
		}
		else {
			tools[i].in_range = false;
		}
		CP_Image_Draw(tools[i].tool_sprite, tools[i].position.x, tools[i].position.y,
			TOOL_WIDTH * TOOL_SCALE, TOOL_HEIGHT * TOOL_SCALE, 255);

	}
}

//renders the held tool seperately in order to render it infront/back of player as needed
void render_held_tool(Tool* tool) {
	//if (tool->tool_name != POT) {
	//	CP_Image_DrawAdvanced(tool->tool_sprite, tool->position.x, tool->position.y,
	//		TOOL_WIDTH * TOOL_SCALE, TOOL_HEIGHT * TOOL_SCALE, 255, tool->rotation * tool->rotatable);
	//}
	//dun mind the hard code :,)
	if (tool->tool_name == WATERING_CAN && tool->uses <= 0) {
		tool->tool_sprite = CP_Image_Load("Assets/tool_sprites/water_bucket_empty.png");
	}
	CP_Image_DrawAdvanced(tool->tool_sprite, tool->position.x, tool->position.y,
		TOOL_WIDTH * TOOL_SCALE, TOOL_HEIGHT * TOOL_SCALE, 255, tool->rotation * tool->rotatable);
}
