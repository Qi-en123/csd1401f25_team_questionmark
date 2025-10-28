#include "tools.h"
#include "cprocessing.h"
#include "utils.h"

//Initializes the tools needed/unlocked for each level
void initialize_tools(tool* tools, int size, tool_name* tool_names) {
#define toolStartPosX 100
#define toolStartPosY 200
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

//Render unheld tools and checks if player is within range of them
void render_tools(tool* tools, int size, float player_posx, float player_posy, tool_name held_tool) {
	for (int i = 0; i < size; i++) {
		//if tool is not held
		if (held_tool != tools[i].tool_name) {
			//if player is in range
			if (AreCirclesIntersecting(player_posx, player_posy, 9.0f,
				tools[i].position.x, tools[i].position.y, 40.0f + tool_range)) {

				CP_Settings_Fill(tintColor(tools[i].color, 1.0f / 3.0f));
				tools[i].in_range = true;
			}
			else {
				CP_Settings_Fill(tools[i].color);
				tools[i].in_range = false;
			}
			CP_Graphics_DrawCircle(tools[i].position.x, tools[i].position.y, 60.0f);
		}
		else { //if tool is being held
			//dont draw
		}
	}
}

//renders the held tool seperately
void render_held_tool(tool* tools, int size, tool_name tool_name) {
	for (int i = 0; i < size; i++) {
		if (tools[i].tool_name == tool_name) {
			tools[i].in_range ? CP_Settings_Fill(tintColor(tools[i].color, 1.0f/6.0f)) : CP_Settings_Fill(tools[i].color);
			CP_Graphics_DrawCircle(tools[i].position.x, tools[i].position.y, 60.0f);
		}
	}
}
