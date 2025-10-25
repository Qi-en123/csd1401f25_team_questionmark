/* prasana say must have file level document
THIS FILE CONTAINS THE PLAYER FUNCTIONS AS WELL 
AS THE PLAYER STATS (none for now)
You can use this for player Movement & Interactions
*/
#ifndef PLAYER
#define PLAYER

#include "cprocessing.h"

#define friction 1.0f

typedef struct Vector2 {
	float x, y;
}Vector2;


typedef enum Player_State {
	IDLE,		//0
	WALKING,	//1
	WATERING,	//2
	PLANTING,
	CARRYING,
} Player_State;


typedef struct Player {
	Vector2 position;
	//Vector2 velocity;
	int rotation;
	float speed;
	Player_State player_state;

} Player;

void create_player(Player* player, float speed, float posx, float posy);
void handle_player_input(Player* player);

void move_player(float* pos, float speed);
void simulate_player(Player* player, CP_Image spritesheet);
void render_player(Player* player, CP_Image spritesheet);

#endif
