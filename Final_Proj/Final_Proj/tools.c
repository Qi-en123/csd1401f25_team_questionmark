#include "tools.h"
#include "cprocessing.h"
#include "utils.h"

//Initializes the tools needed/unlocked for each level
void initialize_tools(tool* tools, int size, tool_name* tool_names) {
#define toolStartPosX 100
#define toolStartPosY 100
	//each tool diff initialpos and different sprites
	//this function just to set the initial pos of each tool
	//nid to check how many tools are in the level and which ones

	for (int i = 0; i < size; i++) {
		//Setting the positions of the tools
		tools[i].initial_pos.x = tools[i].position.x = toolStartPosX;
		tools[i].initial_pos.y = tools[i].position.y = toolStartPosY + 120.0f * i;
		tools[i].tool_name = tool_names[i];
		tools[i].in_range = false;

		//Setting the sprites/color of the tools
		switch (tool_names[i]) {
		case WATERING_CAN:
			tools[i].color = CP_Color_Create(0, 0, 255, 255);
			break;
		case SOIL:
			tools[i].color = CP_Color_Create(255, 180, 20, 255);
			break;
		case SEEDS:
			tools[i].color = CP_Color_Create(30, 255, 100, 255);
			break;
		default:
			break;
		}
	}
}

// !!!! POTENTIAL BUGS: sometimes player within range of 2 tools !!!! // - i will fix next time - rod

//Render tools & check if player is within range
void simulate_tools(tool* tools, int size, float player_posx, float player_posy) {
	for (int i = 0; i < size; i++) {
		//TO BE SWAPPED WITH CP_IMAGE OF TOOL SPRITE
		if (AreCirclesIntersecting(player_posx, player_posy, 9.0f,
			tools[i].position.x, tools[i].position.y, 40.0f + tool_range)) {
			CP_Settings_Fill(tintColor(tools[i].color, 1.0f / 3.0f));
			tools[i].in_range = true;
		}
		else {
			CP_Settings_Fill(tools[i].color);
			tools[i].in_range = false;
		}

		CP_Graphics_DrawCircle(tools[i].position.x, tools[i].position.y, 80.0f);
	}
}
