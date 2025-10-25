#include "cprocessing.h" // for all CP function
#include "mainmenu.h" // to go back with key Q
#include <math.h>   // calculations for speed and all  
#include "utils.h" //for intersecting function
#include "game.h"
#include <stdio.h>

#include "player.h"

CP_Font myFont; 
float stage_timer = 180.0f;

//--------------------- Player ---------------------//
Player player;
float speed = 300.0f;
float starting_pos_x = 100.0f, starting_pos_y = 100.0f;
CP_Image player_spritesheet;


void Game_Init(void)
{
	CP_Settings_RectMode(CP_POSITION_CORNER);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	myFont = CP_Font_Load("Assets/Exo2-Regular.ttf");

	//Initialize player with default values
	create_player(&player, speed, starting_pos_x, starting_pos_y);
	player_spritesheet = CP_Image_Load("Assets/farmer_spritesheet.png");

}

void Game_Update(void)
{
	CP_Graphics_ClearBackground(CP_Color_Create(255,255,255,0));
	//EVERYTH PLAYER RELATED IN THIS FUNCTION
	simulate_player(&player, player_spritesheet);

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