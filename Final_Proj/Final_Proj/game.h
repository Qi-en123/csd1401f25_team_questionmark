#ifndef GAME_H
#define GAME_H

#include "tools.h"
#include "cprocessing.h"
#include "plants.h"
#include "player.h"

#pragma region Variables
static CP_Image guide1;
static CP_Image guide2;
static CP_Font myFont;
static CP_Font Font;
static CP_Image button_white;
static CP_Image button_green;
static CP_Image button_score;
static CP_Image button_square_white;
static CP_Image button_square_green;
static CP_Image button_circle_white;
static CP_Image button_circle_green;
static CP_Image white_background;
static CP_Image repeat;
static CP_Image next;
static CP_Image home;
static CP_Image left;
static CP_Image right;

extern int level;
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

//--------------------- Requests ---------------------//
static CP_Image requestbg = NULL;
static CP_Image berrybush = NULL;
static CP_Image orangebush = NULL;
static CP_Image cactus = NULL;
static CP_Image pest_spritesheet = NULL;
static CP_Sound customerNoti;
static float startDelay;
#define REQUEST_TIMER 80.0f
#define REQUEST_INTERVAL 20.0f

//--------------------- Pests ---------------------//
#define PEST_SPAWN_INTERVAL 40.0f

typedef struct {
	float x, y;
	float targetX;
	float width, height;
	float speed;
	int removing;
	float removeSpeed;
	float lateTimer;

	// New fields for plant requests
	int numPlants;       // how many plants requested (1-3)
	//int plantTypes[3];   // 0 = cactus | 1 = berrybush | 2 = orangebush
	int plantType;

} Box;

// Maximum number of box
#define MAX_BOXES 5
static Box boxes[MAX_BOXES];
static int boxCount;
static float spawnTimer;

//--------------------- Map/Background ---------------------//
static Tile map_tiles[TILE_ROWS][TILE_COLS];
static int placeable[] = { 1,2 }; //set tiles which are placeable


static int paused = 1; // 0 = running, 1 = paused
static int showControls = 1; // 0 = hidden, 1 = showing
static int gameOver = 0;   // 0 = running, 1 = ended
static int starCount = 0;  // 0-3 stars
static int score = 0;
static CP_Image star_default;
static CP_Image star_achieved;
#pragma endregion


void Game_Init(void);

void Game_Update(void);

void Game_Exit(void);	

void start_timer(void* init, void* update, void* exit, float timer, Timer* tp, CP_Font font);
void move_tool(float* posx, float* posy, float* rotation, Player* player);
void handle_tool_interaction(Tool* tool, Player* player, Tile* tiles, Pot* pots, Tool** held_tool);
void spawn_box(void);


void draw_gameover_panel(int score, void* init, void *update, void *exit);
void move_pot(float* posx, float* posy, Player* player);
void handle_pot_interaction(Pot* pot);
void customer_request();
void simulate_pests();

void load_assets();
#endif // !GAME_H