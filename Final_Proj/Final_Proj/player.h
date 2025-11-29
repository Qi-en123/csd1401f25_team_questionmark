#pragma once

#include "cprocessing.h"
#include "utils.h"
#include "map.h"

#define PLAYER_WIDTH 18
#define PLAYER_HEIGHT 18
#define PLAYER_SCALE 5.0f

typedef enum Player_State {
	IDLE,		//0
	WALKING,	//1
	WATERING,	//2
	PLANTING,   //3
} Player_State;

typedef struct Player {
	Vector2 position;
	int rotation;
	float speed;
	Player_State player_state;
} Player;

//Initialize player values defined in each level: speed, spawn pos
void initialize_player(Player* player, float speed, float posx, float posy);

//Handles player inputs for movement
void handle_player_input(Player* player);

//Updates player position on the given axis (posx || posy)
void move_player(float* pos, float speed);

//detects collision between player & window borders || player and & obstacles
void detect_collision_player(float* posx, float* posy, Tile* tiles, float speed);

//Draws player
void render_player(Player* player, CP_Image spritesheet);