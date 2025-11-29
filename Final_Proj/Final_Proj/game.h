#pragma once

#include "tools.h"
#include "cprocessing.h"
#include "plants.h"
#include "player.h"

#pragma region Variables
//--------------------- Player ---------------------//
static Player player;
static float player_speed;
static float player_startpos_x, player_startpos_y;
static CP_Image player_spritesheet = NULL;

//--------------------- Pots ---------------------//
static CP_Image pot_spritesheet = NULL;

//--------------------- Timer ---------------------//
typedef struct Timer {
	float stage_timer;
	CP_Image timer_bg_blue, timer_bg_red;
	float pos_x, pos_y;
	float width, height;
	float font_size;
	CP_Color font_color;
}Timer;

//--------------------- Pests ---------------------//
static float pest_spawn_interval;

//--------------------- Requests ---------------------//
typedef struct Box {
	float x, y;
	float target_x;
	float width, height;
	float speed;
	int removing;
	float remove_speed;
	float late_timer;

	// New fields for plant requests
	int num_plants;       // how many plants requested (1-3)
	int plant_type;

} Box;
typedef struct Request_State{
	Box* boxes;
	int box_count;
	float spawn_timer;
	float start_delay;
	CP_Image berrybush;
	CP_Image cactus;
	CP_Image orangebush;
	CP_Image request_bg;
	CP_Sound customer_noti;
	float request_interval;
	float request_timer;
}Request_State;
// Maximum number of box
#define MAX_BOXES 5
static Box boxes[MAX_BOXES];
static int box_count;
static float spawn_timer;

//--------------------- Map/Background ---------------------//
static Tile map_tiles[TILE_ROWS][TILE_COLS];
static int placeable[] = { 1,2 }; //set tiles which are placeable

//--------------------- Game State ---------------------//
static int paused = 1; // 0 = running, 1 = paused
static int show_controls = 1; // 0 = hidden, 1 = showing
static int game_over = 0;   // 0 = running, 1 = ended
static int star_count = 0;  // 0-3 stars
static int score = 0;
extern int level;
static int played_lose = 0;
static int played_win = 0;

//--------------------- Menu sprites ---------------------//
static CP_Image guide1;
static CP_Image guide2;
static CP_Image button_white;
static CP_Image button_green;
static CP_Image button_score;
static CP_Image button_square_white;
static CP_Image button_square_green;
static CP_Image left;
static CP_Image right;
#pragma endregion

void Game_Init(void);

void Game_Update(void);

void Game_Exit(void);	

//starts the countdown
void start_timer(float timer, Timer* tp, CP_Font font);

//Updates held tool position along with player's with an offset based on player direction
void move_tool(float* posx, float* posy, float* rotation, Player* player);

//Handles tool interaction with pots and plants
//Handles placing tool down
void handle_tool_interaction(Tool* tool, Player* player, Tile* tiles, Pot* pots, Tool** held_tool, int size);

//Draws game over panel
void draw_game_over_panel(int* score, void* init, void* update, void* exit,
	void* next_init, void* next_update, void* next_exit, int curr_lvl, int *playedWin, int *playedLose);

//Updates held pot position along with player's with an offset based on player direction
void move_pot(float* posx, float* posy, Player* player);

//Lets player place down pot on grid
void handle_pot_interaction(Pot* pot, Player* player, Tile* tiles, Tool** held_tool, Pot** held_pot);

//Initializes next request box 
void spawn_box(Request_State* request, int tool_size);

//Spawns request box at an interval
void customer_request(Request_State* request, Pot* pots, Tile* tiles, int* score, int tool_size, int pot_size);

//Handles all pest functions in here: Spawning, Rendering, Movement & Pest interactions
void simulate_pests(float spawn_interval, Pot* pots, int pot_size, Player* player, CP_Image pest_spritesheet);