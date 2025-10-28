#include "cprocessing.h" // for all CP function
#include "mainmenu.h" // to go back with key Q
#include <math.h>   // calculations for speed and all  
#include "utils.h" //for intersecting function
#include "game.h"
#include <stdio.h>

#include "player.h"
#include "tools.h"

CP_Font myFont; 
float stage_timer = 180.0f;

//--------------------- Player ---------------------//
Player player;
float speed = 300.0f;
float starting_pos_x = 100.0f, starting_pos_y = 100.0f;
CP_Image player_spritesheet;

//--------------------- Tools ---------------------//
#define l1_tool_size 3
tool level1_tools[l1_tool_size];
tool_name level1_tool_names[l1_tool_size] = { WATERING_CAN, SOIL, SEEDS };

int carrying = 0;


void Game_Init(void)
{
	CP_Settings_RectMode(CP_POSITION_CORNER);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	myFont = CP_Font_Load("Assets/Exo2-Regular.ttf");

	//Initialize player with default values
	initialize_player(&player, speed, starting_pos_x, starting_pos_y);
	player_spritesheet = CP_Image_Load("Assets/farmer_spritesheet.png");

	//tools
	initialize_tools(level1_tools, l1_tool_size, level1_tool_names);
}

void Game_Update(void)
{
	CP_Graphics_ClearBackground(CP_Color_Create(150, 150, 150, 255));

	//--------------------- Tools ---------------------//
	render_tools(level1_tools, l1_tool_size, player.position.x, player.position.y, player.held_tool);
	if (90 == player.rotation || 180 == player.rotation) render_held_tool(level1_tools, l1_tool_size, player.held_tool);

	//--------------------- Player ---------------------//
	//EVERYTH PLAYER RELATED IN THIS FUNCTION
	simulate_player(&player, player_spritesheet);
	if (0 == player.rotation || 270 == player.rotation) render_held_tool(level1_tools, l1_tool_size, player.held_tool);

	//--------------------- Hold Tool ---------------------//
	//sry for this mess
	//If player is not holding anything
	if (HAND == player.held_tool) {
		if (CP_Input_KeyReleased(KEY_Q)) {
			//Picks up the first tool that is in range
			for (int i = 0; i < l1_tool_size; i++) {
				if (true == level1_tools[i].in_range) {
					player.held_tool = level1_tools[i].tool_name;
					break;
				}
			}
		}
	}
	//If player is holding something
	else {
		for (int i = 0; i < l1_tool_size; i++) {
			//Held tool
			if (level1_tools[i].tool_name == player.held_tool) {
				move_tool(&level1_tools[i].position.x, &level1_tools[i].position.y);

				//if player is back at where he picked up the tool
				if (AreCirclesIntersecting(player.position.x, player.position.y, 9.0f,
					level1_tools[i].initial_pos.x, level1_tools[i].initial_pos.y, 40.0f + tool_range)) {

					level1_tools[i].in_range = true;

					if (CP_Input_KeyReleased(KEY_Q)) {
						player.held_tool = HAND;
						level1_tools[i].position.x = level1_tools[i].initial_pos.x;
						level1_tools[i].position.y = level1_tools[i].initial_pos.y;
					}
				}
				else level1_tools[i].in_range = false;
			}
		}
	}

	//--------------------- Timer ---------------------//
	//render timer on top
	start_timer();
}

void Game_Exit(void)
{
	CP_Image_Free(player_spritesheet);
	CP_Font_Free(myFont);
}

void start_timer() {
	//countdown
	stage_timer -= CP_System_GetDt();

	//TEMP BORDER -- replaced with timer background
	CP_Settings_Fill(CP_Color_Create(0, 255, 0, 255));
	CP_Graphics_DrawRect(30.0f, 30.0f, CP_System_GetWindowWidth() / 8.0f, 80.0f);

	//--------------------- Timer Code ---------------------//

	//clamp timer
	int minutes = ((int)stage_timer / 60.0f) >= 0 ? ((int)stage_timer / 60) : 0;
	int seconds = ((int)stage_timer % 60) >= 0 ? ((int)stage_timer % 60) : 0;

	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	//Big enough to store formatted string
	char countdown[50] = { 0 };
	//converts stage_timer into a string/char*
	sprintf_s(countdown, 50, "%d:%02d", minutes, seconds); // seconds formatted to have leading 0 if 1 digit
	CP_Font_DrawText(countdown, 30.0f + CP_System_GetWindowWidth() / 8.0f / 2, 30.0f + 80.0f/2);
}

//moves the tool along with the player
void move_tool(float *posx, float *posy) {
#define offset 30
	float offset_x = 0.0f;
	float offset_y = 0.0f;

	//These are rough offsets -- will have to change it once the tool sprite is up
	//Have to make it look like the player is holding it (right handed)
	switch (player.rotation) {
	case 0: //DOWN
		offset_x = 0.0f;
		offset_y = offset;
		break;
	case 90: //LEFT
		offset_x = -offset;
		offset_y = 0.0f;
		break;
	case 180: //UP
		offset_x = 0.0f;
		offset_y = -offset;
		break;
	case 270: //RIGHT
		offset_x = offset;
		offset_y = 0.0f;
		break;
	default:
		break;
	}
	*posx = player.position.x + offset_x;
	*posy = player.position.y + offset_y;
}