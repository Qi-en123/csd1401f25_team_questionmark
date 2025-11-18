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

	player->position.x = CP_Math_ClampFloat(player->position.x, 0, CP_System_GetWindowWidth() - 
		(PLAYER_WIDTH * PLAYER_SCALE));
	player->position.y = CP_Math_ClampFloat(player->position.y, 0, CP_System_GetWindowHeight() - 
		(PLAYER_HEIGHT * PLAYER_SCALE));
}

//Move player on 1 axis
void move_player(float* pos, float speed) {
	*pos += speed * dt;
}

//These are for player animation
float time_between_frame = 0.0f; //to check time between each frame
int current_frame = 0; //to check current frame

//#define FRAMES 12
//To draw the player
void render_player(Player* player, CP_Image spritesheet) {
	//--------------------- MOVEMENT ---------------------//
	float pos_x = player->position.x;
	float pos_y = player->position.y;

	float img_x = 0.0f;

	//IDLE ANIMATION
	if (player->player_state == IDLE) {
		switch (player->rotation)
		{
		case 0: //down
			img_x = PLAYER_WIDTH;
			break;

		case 90: //left
			img_x = 0.0f;
			break;

		case 180: //up
			img_x = PLAYER_WIDTH * 2;
			break;

		case 270: //right
			img_x = PLAYER_WIDTH * 3;
			break;

		default:
			img_x = PLAYER_WIDTH;
			break;
		}
	}

	//WALKING ANIMATION
	if (player->player_state == WALKING) {
		switch (player->rotation)
		{
		case 0: //down
			img_x = PLAYER_WIDTH * 5; //90,162 => frames (2) => currframe 0&1 => currframe *4*18 + offset;
			break;

		case 90: //left
			img_x = PLAYER_WIDTH * 4;
			break;

		case 180: //up
			img_x = PLAYER_WIDTH * 6;
			break;

		case 270: //right
			img_x = PLAYER_WIDTH * 7;
			break;

		default:
			break;
		}

		time_between_frame += dt;
		if (time_between_frame > 0.3f) {
			time_between_frame = 0.0f;
			current_frame = (1 + current_frame) % 2;
		}
	}

	//Player shadow
	CP_Settings_EllipseMode(CP_POSITION_CENTER);
	CP_Settings_NoStroke();
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 120));

	float shadow_x = pos_x + (PLAYER_WIDTH * PLAYER_SCALE / 2);
	float shadow_y = pos_y + PLAYER_HEIGHT * PLAYER_SCALE - 4.0f;
	CP_Graphics_DrawEllipse(shadow_x, shadow_y, PLAYER_WIDTH * PLAYER_SCALE * 0.65f, 20.0f);


	CP_Settings_ImageMode(CP_POSITION_CORNER);
	//render player
	CP_Image_DrawSubImage(spritesheet, pos_x, pos_y, PLAYER_WIDTH * PLAYER_SCALE, PLAYER_HEIGHT * PLAYER_SCALE, //sprite settings
		img_x + (current_frame * PLAYER_WIDTH * 4), 0.0f,  //start of sub img
		img_x + (current_frame * PLAYER_WIDTH * 4) + PLAYER_WIDTH, PLAYER_HEIGHT, 255); //end of sub img
}
