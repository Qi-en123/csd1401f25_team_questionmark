/* Everything related to player
Movements, Interactions, Animation, Stats
*/

#include "player.h"
#include "cprocessing.h"
#include <stdio.h>
#include <math.h>

//intialize the player object
void initialize_player(Player* player, float speed, float posx, float posy) {
	player->position.x = posx;
	player->position.y = posy;
	player->player_state = IDLE;
	player->speed = speed;
	player->rotation = 0;
	player->held_tool = HAND;
}

//This is to handle inputs and change the player states accordingly
void handle_player_input(Player* player) {
	player->player_state = IDLE;

	//--------------------- MOVEMENT ---------------------//
	if (CP_Input_KeyDown(KEY_A)) { //left
		move_player(&player->position.x, -player->speed);
		player->rotation = 90;
		player->player_state = WALKING;
	}
	else if (CP_Input_KeyDown(KEY_D)) { //right
		move_player(&player->position.x, player->speed);
		player->rotation = 270;
		player->player_state = WALKING;

	}
	else if (CP_Input_KeyDown(KEY_S)) { //down
		move_player(&player->position.y, player->speed);
		player->rotation = 0;
		player->player_state = WALKING;

	}
	else if (CP_Input_KeyDown(KEY_W)) { //up
		move_player(&player->position.y, -player->speed);
		player->rotation = 180;
		player->player_state = WALKING;
	}
}

//The actual movement is from here
void move_player(float* pos, float speed) {
	float dt = CP_System_GetDt();
	*pos += speed * dt;
}

//All of the player functions in here
void simulate_player(Player* player, CP_Image spritesheet) {
	handle_player_input(player);
	render_player(player, spritesheet);
}

//These are for player animation
float time_between_frame = 0.0f; //to check time between each frame
int current_frame = 0; //to check current frame

//To draw the player
void render_player(Player* player, CP_Image spritesheet) {
#define SPRITE_WIDTH 18
#define SPRITE_HEIGHT 18
#define FRAMES 12

	//--------------------- MOVEMENT ---------------------//
	float pos_x = player->position.x;
	float pos_y = player->position.y;

	float img_x = 0.0f;

	//IDLE ANIMATION
	if (player->player_state == IDLE) {
		switch (player->rotation)
		{
		case 0: //down
			img_x = SPRITE_WIDTH;
			break;

		case 90: //left
			img_x = 0.0f;
			break;

		case 180: //up
			img_x = SPRITE_WIDTH * 2;
			break;

		case 270: //right
			img_x = SPRITE_WIDTH * 3;
			break;

		default:
			img_x = SPRITE_WIDTH;
			break;
		}
	}

	//WALKING ANIMATION
	if (player->player_state == WALKING) {
		switch (player->rotation)
		{
		case 0: //down
			img_x = SPRITE_WIDTH * 5; //90,162 => frames (2) => currframe 0&1 => currframe *4*18 + offset;
			break;

		case 90: //left
			img_x = SPRITE_WIDTH * 4;
			break;

		case 180: //up
			img_x = SPRITE_WIDTH * 6;
			break;

		case 270: //right
			img_x = SPRITE_WIDTH * 7;
			break;

		default:
			break;
		}

		time_between_frame += CP_System_GetDt();
		if (time_between_frame > 0.3f) {
			time_between_frame = 0.0f;
			current_frame = (1 + current_frame) % 2;
		}
	}

	//render player
	CP_Image_DrawSubImage(spritesheet, pos_x, pos_y, SPRITE_WIDTH*4.0f, SPRITE_HEIGHT*4.0f, //sprite settings
		img_x + (current_frame * SPRITE_WIDTH * 4), 0.0f,  //start of sub img
		img_x + (current_frame * SPRITE_WIDTH * 4) + SPRITE_WIDTH, SPRITE_HEIGHT, 255); //end of sub img
}
