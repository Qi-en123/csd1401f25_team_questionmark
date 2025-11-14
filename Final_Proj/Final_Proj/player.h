/* prasana say must have file level document
THIS FILE CONTAINS THE PLAYER FUNCTIONS AS WELL 
AS THE PLAYER STATS (none for now)
You can use this for player Movement & Interactions
*/
#ifndef PLAYER_H
#define PLAYER_H

#include "cprocessing.h"
#include "utils.h"

typedef enum Player_State {
	IDLE,		//0
	WALKING,	//1
	WATERING,	//2
	PLANTING,
} Player_State;


typedef struct Player {
	Vector2 position;
	int rotation;
	float speed;
	Player_State player_state;
} Player;

void initialize_player(Player* player, float speed, float posx, float posy);
void handle_player_input(Player* player);

void move_player(float* pos, float speed);
void render_player(Player* player, CP_Image spritesheet);

#endif // !PLAYER_H

