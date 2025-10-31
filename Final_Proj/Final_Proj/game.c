#include "cprocessing.h" // for all CP function
#include "mainmenu.h" // to go back with key Q
#include <math.h>   // calculations for speed and all  
#include "utils.h" //for intersecting function
#include "game.h"
#include <stdio.h>

#include "player.h"
#include "tools.h"

CP_Font myFont;
float stage_timer = 35.0f;

//--------------------- Player ---------------------//
Player player;
float speed = 300.0f;
float starting_pos_x = 100.0f, starting_pos_y = 100.0f;
CP_Image player_spritesheet;

//--------------------- Tools ---------------------//
#define l1_tool_size 3
Tool level1_tools[l1_tool_size];
Tool_name level1_tool_names[l1_tool_size] = { WATERING_CAN, SOIL, SEEDS };
Tool held_tool;

//--------------------- Timer ---------------------//
CP_Image timer_bg_blue = NULL, timer_bg_red = NULL;
float timer_pos_x, timer_pos_y;
float timer_width, timer_height;
float timer_font_size;
CP_Color timer_font_color;


int carrying = 0;


void Game_Init(void)
{
	CP_Settings_RectMode(CP_POSITION_CENTER);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	myFont = CP_Font_Load("Assets/Kenney Pixel Square.ttf");

	//Initialize player with default values
	initialize_player(&player, speed, starting_pos_x, starting_pos_y);
	player_spritesheet = CP_Image_Load("Assets/farmer_spritesheet.png");

	//Tools
	initialize_tools(level1_tools, l1_tool_size, level1_tool_names, &held_tool);

	//Timer
#pragma region Timer Initialization

	timer_bg_blue = CP_Image_Load("Assets/timer_bg_blue.png");
	timer_bg_red = CP_Image_Load("Assets/timer_bg_red.png");
	timer_width = CP_System_GetWindowWidth() / 8.0f;
	timer_height = 70.0f;
	timer_font_size = 50.0f;
	timer_pos_x = 30.0f + timer_width / 2;
	timer_pos_y = 30.0f + timer_height / 2;
	timer_font_color = CP_Color_Create(255, 255, 255, 255);

#pragma endregion
}

void Game_Update(void)
{
	CP_Graphics_ClearBackground(CP_Color_Create(150, 150, 150, 255));

	//--------------------- Tools ---------------------//
	render_tools(level1_tools, l1_tool_size, player.position.x, player.position.y, held_tool.tool_name);
	//Render tool behind player if player facing Left or Up
	if (held_tool.tool_name != HAND && (90 == player.rotation || 180 == player.rotation)) {
		render_held_tool(&held_tool);
	}

	//--------------------- Player ---------------------//
	//EVERYTH PLAYER RELATED IN THIS FUNCTION
	simulate_player(&player, player_spritesheet);
	//Render tool infront if player facing Right or Down
	if (held_tool.tool_name != HAND && (0 == player.rotation || 270 == player.rotation)) {
		render_held_tool(&held_tool);
	}

	//--------------------- Hold Tool ---------------------//
	//If player is not holding anything
	if (HAND == held_tool.tool_name) {
		if (CP_Input_KeyReleased(KEY_Q)) {
			//Picks up the first tool that is in range
			for (int i = 0; i < l1_tool_size; i++) {
				if (level1_tools[i].in_range) {
					held_tool = level1_tools[i];
					break;
				}
			}
		}
	}
	//If player is holding something
	else {
		move_tool(&held_tool.position.x, &held_tool.position.y, &held_tool.rotation);
		handle_tool_interaction(&held_tool);
	}

	//--------------------- Timer ---------------------//
	//render timer on top
	start_timer();
}

void Game_Exit(void)
{
	CP_Image_Free(player_spritesheet);
	CP_Font_Free(myFont);
	CP_Image_Free(timer_bg_blue);
	CP_Image_Free(timer_bg_red);
	for (int i = 0; i < l1_tool_size; i++) {
		CP_Image_Free(level1_tools[i].tool_sprite);
	}
}

void start_timer() {
	//countdown
	stage_timer -= dt;

	static float timer_red_alpha;

	//--------------------- Timer Code ---------------------//
	//clamp timer
	int minutes = ((int)stage_timer / 60.0f) >= 0 ? ((int)stage_timer / 60) : 0;
	int seconds = ((int)stage_timer % 60) >= 0 ? ((int)stage_timer % 60) : 0;

	//Big enough to store formatted string
	char countdown[50] = { 0 };
	//converts stage_timer into a string/char*
	sprintf_s(countdown, 50, "%d:%02d", minutes, seconds); // seconds formatted to have leading 0 if 1 digit

	//blinking effect
	if (stage_timer < 30.0f) {
		if ((int)stage_timer % 2) {
			timer_red_alpha += 255.0f * dt;
			timer_width += timer_width * dt * 0.09f;
			timer_height += timer_height * dt * 0.09f;
			timer_font_size += timer_font_size * dt * 0.25f;
		}
		else {
			timer_red_alpha -= 200.0f * dt;
			timer_width -= timer_width * dt * 0.09f;
			timer_height -= timer_height * dt * 0.09f;
			timer_font_size -= timer_font_size * dt * 0.25f;
		}
		timer_red_alpha = CP_Math_ClampFloat(timer_red_alpha, 0.0f, 255.0f);
		timer_font_color = ((int)(stage_timer * 1.5f) % 2) ? CP_Color_Create(50, 50, 50, 255) : CP_Color_Create(255, 255, 255, 255);
		timer_width = CP_Math_ClampFloat(timer_width, CP_System_GetWindowWidth() / 8.0f * 0.9f, CP_System_GetWindowWidth() / 8.0f * 1.1f);
		timer_height = CP_Math_ClampFloat(timer_height, 70.0f * 0.9f, 70.0f * 1.1f);
	}

	//Timer background
	CP_Image_Draw(timer_bg_blue, timer_pos_x, timer_pos_y, timer_width, timer_height, 255);
	CP_Image_Draw(timer_bg_red, timer_pos_x, timer_pos_y, timer_width, timer_height, timer_red_alpha);
	//Timer 
	CP_Settings_TextSize(timer_font_size);
	CP_Settings_Fill(timer_font_color);
	CP_Font_DrawText(countdown, timer_pos_x, timer_pos_y);
}

//moves the tool along with the player
void move_tool(float* posx, float* posy, float* rotation) {
	float offset_x = 0.0f;
	float offset_y = 0.0f;

	//Have to make it look like the player is holding it (right handed)
	switch (player.rotation) {
	case 0: //DOWN
		offset_x = -20.0f;
		offset_y = 28.0f;
		break;
	case 90: //LEFT
		offset_x = -12.0f;
		offset_y = 15.0f;
		break;
	case 180: //UP
		offset_x = 20.0f;
		offset_y = 10.0f;
		break;
	case 270: //RIGHT
		offset_x = 10.0f;
		offset_y = 20.0f;
		break;
	default:
		break;
	}
	*posx = player.position.x + offset_x;
	*posy = player.position.y + offset_y;
	*rotation = player.rotation - 180.0f;
}

void handle_tool_interaction(Tool* tool) {
	//if player is back at where he picked up the tool
	if (AreCirclesIntersecting(player.position.x, player.position.y, 9.0f,
		tool->initial_pos.x, tool->initial_pos.y, 40.0f + tool_range)) {

		tool->in_range = true;

		if (CP_Input_KeyReleased(KEY_Q)) {
			tool->tool_name = HAND;
			tool->position.x = tool->initial_pos.x;
			tool->position.y = tool->initial_pos.y;
		}
	}
	else tool->in_range = false;

	//--------------------- Plant Interaction ---------------------//
	//will check for plant intersection, nxt time
	//if (CP_Input_KeyDown(KEY_E)) {

	//}
}