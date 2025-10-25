#include "cprocessing.h" // for all CP function
#include "mainmenu.h" // to go back with key Q
#include <math.h>   // calculations for speed and all  
#include "utils.h" //for intersecting function

#include "player.h"

Player player;
float speed = 300.0f;
float starting_pos_x = 100.0f, starting_pos_y = 100.0f;
CP_Image player_spritesheet;


void Game_Init(void)
{
	create_player(&player, speed, starting_pos_x, starting_pos_y);
	player_spritesheet = CP_Image_Load("Assets/farmer_spritesheet.png");
}

void Game_Update(void)
{
	CP_Graphics_ClearBackground(CP_Color_Create(255,255,255,0));
	simulate_player(&player, player_spritesheet);
}

void Game_Exit(void)
{
	
}