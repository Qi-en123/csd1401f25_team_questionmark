#include "tools.h"
#include "cprocessing.h"
#include "utils.h"
#include "plants.h"

//Initializes the tools needed/unlocked for each level
void initialize_tools(Tool* tools, int tool_size, Tool_name* tool_names, Tile* tile, int uses) {
	//each tool diff initialpos and different sprites
	//this function just to set the initial pos of each tool
	for (int i = 0; i < tool_size; i++) {
		//Setting the positions of the tools
		Tile* curr_tile = &tile[TILE_COLS * i * 2];

		Tool *tool = &tools[i];
		tool->position.x = curr_tile->x;
		tool->position.y = curr_tile->y;
		tool->occupied_tile = curr_tile;
		curr_tile->obstructed = true;
		tool->tool_name = tool_names[i];
		tool->in_range = false;
		tool->rotation = 0.0f;
		tool->uses = uses; //only used for watering_can :p
		tool->selected = false; 

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
}

//Render unheld tools and checks if player is within range of them
void render_tools(Tool* tools, int size, float player_posx, float player_posy, Tool* held_tool, void* held_pot) {

	//ensures only 1 tool can be selected at all times
	bool tool_selected = false;

	for (int i = 0; i < size; i++) {
		//if tool is not held
		if (&tools[i] == held_tool) { continue; }

		//highlight nearby tool
		if (AreCirclesIntersecting(player_posx, player_posy, 9.0f,
			tools[i].position.x, tools[i].position.y - TOOL_RANGE_Y_OFFSET, 40.0f + TOOL_RANGE) &&
			!held_tool && held_pot == NULL) {
			if (!tool_selected) {
				tools[i].in_range = true;
				tool_selected = true;
			}
			show_tooltip("Press 'J' to Pick Up");
		}
		else {
			tools[i].in_range = false;
		}

		//Tool shadow
		CP_Settings_EllipseMode(CP_POSITION_CENTER);
		CP_Settings_NoStroke();
		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 120));

		float shadow_x = tools[i].position.x;
		float shadow_y = tools[i].position.y + TOOL_HEIGHT * TOOL_SCALE / 2.0f - 2.0f;
		CP_Graphics_DrawEllipse(shadow_x, shadow_y, TOOL_WIDTH * TOOL_SCALE, 20.0f);

		//Tool
		CP_Settings_ImageMode(CP_POSITION_CENTER);
		if (tools[i].in_range) {
			CP_Image_Draw(tools[i].tool_sprite, tools[i].position.x, tools[i].position.y,
				TOOL_WIDTH * TOOL_SCALE, TOOL_HEIGHT * TOOL_SCALE, 100);
		}
		else {
			CP_Image_Draw(tools[i].tool_sprite, tools[i].position.x, tools[i].position.y,
				TOOL_WIDTH * TOOL_SCALE, TOOL_HEIGHT * TOOL_SCALE, 255);
		}
	}
}

//renders the held tool seperately in order to render it infront/back of player as needed
void render_held_tool(Tool* tool) {
	//dun mind the hard code :,)
	if (tool->tool_name == WATERING_CAN && tool->uses <= 0) {
		tool->tool_sprite = CP_Image_Load("Assets/tool_sprites/water_bucket_empty.png");
	}

	CP_Image_DrawAdvanced(tool->tool_sprite, tool->position.x, tool->position.y,
		TOOL_WIDTH * TOOL_SCALE, TOOL_HEIGHT * TOOL_SCALE, 255, tool->rotation * tool->rotatable);
}
