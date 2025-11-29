#include "player.h"
#include "cprocessing.h"
#include "map.h"
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
}

//Move player on 1 axis
void move_player(float* pos, float speed) {
	*pos += speed * dt;
}

//detects collision between player & window borders || player and & obstacles
void detect_collision_player(float *posx, float *posy, Tile* tiles, float speed) {
	float tile_width = (float)CP_System_GetWindowWidth() / TILE_COLS;
	float tile_height = (float)CP_System_GetWindowHeight() / TILE_ROWS;

	//prevents player moving out of screen
	*posx = CP_Math_ClampFloat(*posx, (PLAYER_WIDTH * PLAYER_SCALE / 2.0f),
		CP_System_GetWindowWidth() - (PLAYER_WIDTH * PLAYER_SCALE / 2.0f));
	*posy = CP_Math_ClampFloat(*posy, (PLAYER_HEIGHT * PLAYER_SCALE / 2.0f),
		CP_System_GetWindowHeight() - (PLAYER_HEIGHT * PLAYER_SCALE / 2.0f));

	//grid that player is in
	int row = (int)(*posy / tile_height);
	int col = (int)(*posx / tile_width);
	Tile* tile = (tiles + row * TILE_COLS + col);

	//fake collision (prevents player passing through obstacles on y axis)
	if (!tile->walkable) {
		*posy += (*posy < tile->y) ? -speed * dt : speed * dt;
	}
}

//These are for player animation
float time_between_frame = 0.0f; //to check time between each frame
int current_frame = 0; //to check current frame

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

	CP_Settings_ImageMode(CP_POSITION_CENTER);
	//render player
	CP_Image_DrawSubImage(spritesheet, pos_x, pos_y, PLAYER_WIDTH * PLAYER_SCALE, PLAYER_HEIGHT * PLAYER_SCALE, //sprite settings
		img_x + (current_frame * PLAYER_WIDTH * 4), 0.0f,  //start of sub img
		img_x + (current_frame * PLAYER_WIDTH * 4) + PLAYER_WIDTH, PLAYER_HEIGHT, 255); //end of sub img
}
