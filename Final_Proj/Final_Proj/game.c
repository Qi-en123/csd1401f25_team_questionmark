#include "cprocessing.h" // for all CP function
#include "mainmenu.h" // to go back with key Q
#include <math.h>   // calculations for speed and all  
#include "utils.h" //for intersecting function
#include "game.h"
#include <stdio.h>

#include "player.h"
#include "tools.h"
#include "plants.h"
#include "map.h"
#include <stdlib.h>
#include <time.h>
#include "ui.h"

#include "Pest.h"

CP_Image guide1;
CP_Image guide2;
CP_Font myFont;
CP_Font Font;
CP_Image button_white;
CP_Image button_green;
CP_Image button_score;
CP_Image button_square_white;
CP_Image button_square_green;
CP_Image button_circle_white;
CP_Image button_circle_green;
CP_Image white_background;
CP_Image repeat;
CP_Image next;
CP_Image home;
CP_Image left;
CP_Image right;
CP_Sound BtnSound;

extern int level;
extern int flashTimer; // timer for flashing
extern int flashInterval; // frames between flashes
extern bool showText;
//--------------------- Player ---------------------//
Player player;
float player_speed;
float player_startpos_x, player_startpos_y;
CP_Image player_spritesheet;

//--------------------- Tools ---------------------//
#define L1_TOOL_SIZE 4
Tool level1_tools[L1_TOOL_SIZE];
/*!!!!Due to time constraint thrs a bug, DO NOT skip seed numbers : !!!!
  !!!!		e.g. do SEED_1,2,3, DO NOT DO SEED_2,3 or SEED_1,3		!!!!
  !!!!Tools must always have WATERING_CAN, SOIL & SEED_1 (Minimum)  !!!!*/
Tool_name level1_tool_names[L1_TOOL_SIZE] = { WATERING_CAN, SOIL, SEED_1, SEED_2};
Tool* held_tool; // a pointer to the tool held

//--------------------- Pots ---------------------//
#define L1_POT_SIZE 5
Pot level1_pots[L1_POT_SIZE];
Pot* held_pot; // a pointer to the pot held
CP_Image pot_spritesheet = NULL;


//--------------------- Timer ---------------------//
#define STAGE_TIMER 480.0f
float stage_timer;
CP_Image timer_bg_blue = NULL, timer_bg_red = NULL;
float timer_pos_x, timer_pos_y;
float timer_width, timer_height;
float timer_font_size;
CP_Color timer_font_color;

//--------------------- Requests ---------------------//
CP_Image requestbg = NULL;
CP_Image berrybush = NULL;
CP_Image orangebush = NULL;
CP_Image cactus = NULL;
CP_Image pest_spritesheet = NULL;
CP_Sound customerNoti;
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
	int numPlants;       // how many plants requested (1�3)
	//int plantTypes[3];   // 0 = cactus | 1 = berrybush | 2 = orangebush
	int plantType;

} Box;

// Maximum number of box
#define MAX_BOXES 5
Box boxes[MAX_BOXES];
int boxCount;
float spawnTimer;

//--------------------- Map/Background ---------------------//
Tile map_tiles[TILE_ROWS][TILE_COLS];
int placeable[] = { 1,2 }; //set tiles which are placeable


int paused = 1; // 0 = running, 1 = paused
int showControls = 1; // 0 = hidden, 1 = showing
int gameOver = 0;   // 0 = running, 1 = ended
int starCount = 0;  // 0�3 stars
int score = 0;
CP_Image star_default;
CP_Image star_achieved;


//Frequently used colors
CP_Color red, green, blue, gray;

void Game_Init(void)
{
	level = loadLevel();
	showText = false;
	flashTimer = 0;
	flashInterval = 30;

	paused = 1;
	showControls = 1;
	gameOver = 0;
	score = 0;
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	Font = CP_Font_Load("Assets/Kenney Pixel Square.ttf");
	myFont = CP_Font_Load("Assets/Kenney Future Narrow.ttf");
	star_default = CP_Image_Load("Assets/star_outline.png");
	star_achieved = CP_Image_Load("Assets/star.png");
	guide1 = CP_Image_Load("Assets/guide1.png");
	guide2 = CP_Image_Load("Assets/guide2.png");
	left = CP_Image_Load("Assets/left.png");
	right = CP_Image_Load("Assets/right.png");

	CP_Settings_ImageMode(CP_POSITION_CORNER);

	spriteSheetTiles = CP_Image_Load("Assets/tilemap_packed.png");
	draw_tile_map(map_tiles);
	set_tile_placeable(map_tiles, &placeable, sizeof(placeable) / sizeof(placeable[0]));

	//Request
	srand((unsigned int)time(NULL));
	requestbg = CP_Image_Load("Assets/button_square_gradient-1.png");
	customerNoti = CP_Sound_Load("Assets/BellBrass CTE01_28.5.wav");
	berrybush = CP_Image_Load("Assets/tool_sprites/seed_berrybush.png");
	cactus = CP_Image_Load("Assets/tool_sprites/seed_cactus.png");
	orangebush = CP_Image_Load("Assets/tool_sprites/seed_orangebush.png");
	CP_System_SetFrameRate(60);
	boxCount = 0;
	spawnTimer = 0;
	startDelay = 10.0f;

	//Initialize player with default values
	player_speed = 400.0f;
	player_startpos_x = CP_System_GetWindowWidth() / 2;
	player_startpos_y = CP_System_GetWindowHeight() / 2;
	initialize_player(&player, player_speed, player_startpos_x, player_startpos_y);
	player_spritesheet = CP_Image_Load("Assets/farmer_spritesheet.png");

	// buttons 
	button_white = CP_Image_Load("Assets/button_rectangle_line.png");
	white_background = CP_Image_Load("Assets/button_rectangle_line-1.png");
	button_green = CP_Image_Load("Assets/button_rectangle_gloss.png");
	button_score = CP_Image_Load("Assets/timer_bg_yellow.png");
	button_circle_white = CP_Image_Load("Assets/button_round_line.png");
	button_circle_green = CP_Image_Load("Assets/button_round_gloss.png");
	button_square_white = CP_Image_Load("Assets/button_square_depth_line.png");
	button_square_green = CP_Image_Load("Assets/button_square_depth_gloss.png");
	next = CP_Image_Load("Assets/next.png");
	repeat = CP_Image_Load("Assets/icon_repeat_light.png");
	home = CP_Image_Load("Assets/menuList.png");
	BtnSound = CP_Sound_Load("Assets/ADDING-MACHINE-ELECTRONIC_GEN-HDF-00038.wav");
	//Tools
	initialize_tools(level1_tools, L1_TOOL_SIZE, level1_tool_names, &map_tiles[4][1]);
	held_tool = NULL;


	//Timer
#pragma region Timer Initialization

	stage_timer = STAGE_TIMER;
	timer_bg_blue = CP_Image_Load("Assets/timer_bg_blue.png");
	timer_bg_red = CP_Image_Load("Assets/timer_bg_red.png");
	timer_width = CP_System_GetWindowWidth() / 8.0f;
	timer_height = 70.0f;
	timer_font_size = 50.0f;
	timer_pos_x = 30.0f + timer_width / 2;
	timer_pos_y = 30.0f + timer_height / 2;
	timer_font_color = CP_Color_Create(255, 255, 255, 255);

#pragma endregion

	//Pots
	initialize_pots(level1_pots, L1_POT_SIZE, &map_tiles[5][5]);
	pot_spritesheet = CP_Image_Load("Assets/tool_sprites/pot_spritesheet.png");
	held_pot = NULL;

	//pest
	initialize_pests();
	pest_spritesheet = CP_Image_Load("Assets/rat_spritesheet.png");

	//Colors
	blue = CP_Color_Create(0, 0, 255, 255);
	red = CP_Color_Create(220, 100, 100, 255);
	green = CP_Color_Create(100, 220, 100, 255);
	gray = CP_Color_Create(210, 210, 210, 255);
}

void Game_Update(void)
{
	// Don't allow pause if game is over
	if (!gameOver && CP_Input_KeyReleased(KEY_ESCAPE)) {
		paused = !paused;

		if (paused) {
			showControls = 0; // Reset controls overlay when opening pause menu
		}
	}
	if (CP_Input_KeyReleased(KEY_2)) {
		gameOver = 1;
		starCount = 3;

	}

	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);

	draw_tile_map(map_tiles);
	//set_tile_placeable(map_tiles, &placeable, sizeof(placeable) / sizeof(placeable[0]));

	draw_modified_map(map_tiles);

	//Player shadow
	CP_Settings_EllipseMode(CP_POSITION_CENTER);
	CP_Settings_NoStroke();
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 120));

	float shadow_x = player.position.x;
	float shadow_y = player.position.y + (PLAYER_HEIGHT - 2.0f) * PLAYER_SCALE / 2.0f;
	CP_Graphics_DrawEllipse(shadow_x, shadow_y, PLAYER_WIDTH * PLAYER_SCALE * 0.65f, 20.0f);

	//--------------------- Pots ---------------------//
	render_pots(level1_pots, pot_spritesheet, L1_POT_SIZE, player.position.x, player.position.y, held_pot, held_tool);

	//--------------------- Tools ---------------------//
	render_tools(level1_tools, L1_TOOL_SIZE, player.position.x, player.position.y, held_tool, held_pot);
	//Render tool behind player if player facing Left or Up
	if (90 == player.rotation || 180 == player.rotation) {
		if (held_tool) { render_held_tool(held_tool); }
		if (held_pot) { render_held_pot(held_pot, pot_spritesheet); }
	}

	//--------------------- Hold Tool ---------------------//
	//If player is not holding anything
	if (!held_tool && !held_pot) {
		if (CP_Input_KeyReleased(KEY_J)) {
			//Picks up the first tool that is in range
			for (int i = 0; i < L1_TOOL_SIZE; ++i) {
				if (level1_tools[i].in_range) {
					level1_tools[i].occupied_tile->obstructed = false;
					held_tool = &level1_tools[i];
					break;
				}
			}
			//Picks up first pot that is in range
			for (int i = 0; i < L1_POT_SIZE; ++i) {
				if (level1_pots[i].selected) {					
					held_pot = &level1_pots[i];
					move_pot(&held_pot->position.x, &held_pot->position.y); //intialize held pot pos
					//"unobstructs" grid, makes it placeable
					level1_pots[i].occupied_tile->obstructed = false;
					break;
				}
			}
		}
	}
	//If player is holding something
	else {
		if (held_pot) {
			move_pot(&held_pot->position.x, &held_pot->position.y);
			handle_pot_interaction(held_pot);
		}
		else {
			move_tool(&held_tool->position.x, &held_tool->position.y, &held_tool->rotation);
			handle_tool_interaction(held_tool);
		}
	}
	//draw player
	render_player(&player, player_spritesheet);
	//------------------------------Paused -----------------------------------------------
// If paused, draw pause overlay and skip game logic
	if (paused) {
		// Semi-transparent black overlay

		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 150));
		CP_Settings_RectMode(CP_POSITION_CENTER);
		CP_Graphics_DrawRect(CP_System_GetWindowWidth() / 2,
			CP_System_GetWindowHeight() / 2,
			CP_System_GetWindowWidth(),
			CP_System_GetWindowHeight());




		if (showControls == 1 || showControls == 2) {
			// Draw the controls overlay
			CP_Settings_ImageMode(CP_POSITION_CENTER);
			float panelWidth = 1600;
			float panelHeight = 900;
			float panelX = (float)CP_System_GetWindowWidth() / 2;
			float panelY = (float)CP_System_GetWindowHeight() / 2;

			if (showControls == 1) {
				CP_Image_Draw(guide1, panelX, panelY, panelWidth, panelHeight, 255);

				// Draw right button
				float btnX = 1550, btnY = 420, btnSize = 75;
				if (IsCircleHovered(btnX, btnY, btnSize))
					CP_Image_Draw(button_square_green, btnX, btnY, btnSize, btnSize, 255);
				else
					CP_Image_Draw(button_square_white, btnX, btnY, btnSize, btnSize, 255);

				CP_Image_Draw(right, btnX + 3, btnY - 5, 67, 67, 255);

				// Navigate
				if (IsButtonClicked_Center(btnX, btnY, btnSize, btnSize)) {
					CP_Sound_Play(BtnSound);
					showControls = 2;
				}

			}
			else { // showControls == 2
				CP_Image_Draw(guide2, panelX, panelY, panelWidth, panelHeight, 255);

				// Draw left button
				float btnX = 50, btnY = 420, btnSize = 75;
				if (IsCircleHovered(btnX, btnY, btnSize))
					CP_Image_Draw(button_square_green, btnX, btnY, btnSize, btnSize, 255);
				else
					CP_Image_Draw(button_square_white, btnX, btnY, btnSize, btnSize, 255);

				CP_Image_Draw(left, btnX - 3, btnY - 5, 67, 67, 255);

				// Navigate
				if (IsButtonClicked_Center(btnX, btnY, btnSize, btnSize)) {
					CP_Sound_Play(BtnSound);
					showControls = 1;
				}
			}

			// Close overlay with key 1
			if (CP_Input_KeyReleased(KEY_1))
				showControls = 0;

		}
		else { // showControls == 0
			// Draw normal pause menu
			int x_coord = 650; // shared x coordinate for all buttons
			float btnWidth = 500.0f;
			float btnHeight = 90.0f;
			float btnX = x_coord - 100;
			float btnY[3] = { 225.0f, 425.0f, 625.0f };
			const char* btnText[3] = { "Resume","Controls", "Quit" };

			CP_Settings_ImageMode(CP_POSITION_CORNER);

			for (int i = 0; i < 3; i++) {
				if (IsButtonHovered(btnX, btnY[i], btnWidth, btnHeight))
					CP_Image_Draw(button_green, btnX, btnY[i], btnWidth, btnHeight, 255);
				else
					CP_Image_Draw(button_white, btnX, btnY[i], btnWidth, btnHeight, 255);
			}

			for (int i = 0; i < 3; i++) {
				if (IsButtonClicked(btnX, btnY[i], btnWidth, btnHeight)) {
					CP_Sound_Play(BtnSound);
					break;
				}
			}

			if (IsButtonClicked(btnX, btnY[0], btnWidth, btnHeight)) paused = 0;
			if (IsButtonClicked(btnX, btnY[1], btnWidth, btnHeight)) showControls = 1;
			if (IsButtonClicked(btnX, btnY[2], btnWidth, btnHeight))
				CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);

			// Draw button texts
			CP_Font_Set(Font);
			CP_Settings_TextSize(64.0f);
			CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

			for (int i = 0; i < 3; i++) {
				float textX = btnX + btnWidth / 2;
				float textY = btnY[i] + btnHeight / 2;
				CP_Font_DrawText(btnText[i], textX, textY);
			}
		}

		return; // skip the rest of the game logic
	}
	//---------------- GAME OVER -----------------------------
	if (gameOver)
	{
		draw_gameover_panel(score);
		return;   // Stop all other game updates
	}



	//--------------------- Player ---------------------//
	handle_player_input(&player);
	detect_collision_player(&player.position.x, &player.position.y, map_tiles, player.speed);

	//Render tool infront if player facing Right or Down
	if (0 == player.rotation || 270 == player.rotation) {
		if (held_tool) { render_held_tool(held_tool); }
		if (held_pot) { render_held_pot(held_pot, pot_spritesheet); }
	}

	/*-----------------------------------------------------------------------*/
	customer_request();
	simulate_pests();

	//--------------------- Timer ---------------------//
	//render timer on top
	start_timer();

	if (stage_timer <= 0 && !gameOver)
	{
		gameOver = 1;
	}

	//--------------------- Score ---------------------//
	char buffer_score[50];
	sprintf_s(buffer_score, 50, "%d", score);
	CP_Image_Draw(button_score, timer_pos_x, 135, 200, 60, 255);
	CP_Font_Set(myFont);
	CP_Settings_TextSize(40);
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText(buffer_score, timer_pos_x, 130);


	// Flashing logic
	if (showText) {
		flashTimer++;
		if (flashTimer % flashInterval < flashInterval / 2) {
			CP_Font_Set(Font);
			CP_Settings_TextSize(64.0f);
			CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
			CP_Font_DrawText("Level not unlocked", CP_System_GetWindowWidth() / 2.0f, 100.0f);
		}

		//  stop flashing after some time
		if (flashTimer > flashInterval * 2.5) { // 
			showText = false;
		}
	}

}

void Game_Exit(void)
{
	//--------------------- Free Fonts ---------------------//
	CP_Font_Free(myFont);
	CP_Font_Free(Font);

	//--------------------- Free Sprites ---------------------//
	//Spritesheets
	CP_Image_Free(spriteSheetTiles);
	CP_Image_Free(player_spritesheet);
	CP_Image_Free(pest_spritesheet);

	//Button Sprites
	CP_Image_Free(button_white);
	CP_Image_Free(button_white);
	CP_Image_Free(button_score);
	CP_Image_Free(button_circle_white);
	CP_Image_Free(button_circle_green);
	CP_Image_Free(white_background);
	CP_Image_Free(repeat);
	CP_Image_Free(next);
	CP_Image_Free(home);

	//Timer sprite
	CP_Image_Free(timer_bg_blue);
	CP_Image_Free(timer_bg_red);

	//Tool sprites
	for (int i = 0; i < L1_TOOL_SIZE; ++i) {
		CP_Image_Free(level1_tools[i].tool_sprite);
	}
	if (held_tool) { CP_Image_Free(held_tool->tool_sprite); }

	//Pot spritesheet
	CP_Image_Free(pot_spritesheet);

	//Plant sprites
	for (int i = 0; i < L1_POT_SIZE; ++i) {
		CP_Image_Free(level1_pots[i].plant.plant_spritesheet);
	}

	//Request sprites
	CP_Image_Free(requestbg);
	CP_Image_Free(berrybush);
	CP_Image_Free(cactus);
	CP_Image_Free(orangebush);

	CP_Image_Free(star_default);
	CP_Image_Free(star_achieved);

	//--------------------- Free Audio ---------------------//
	CP_Sound_Free(customerNoti);
	CP_Sound_Free(BtnSound);
}

void start_timer() {
	//countdown
	stage_timer -= dt;

	static float timer_red_alpha;

	//--------------------- Timer Code ---------------------//
	//clamp timer
	int minutes = ((int)stage_timer / 60.0f) >= 0 ? ((int)stage_timer / 60) : 0;
	int seconds = ((int)stage_timer % 60) >= 0 ? ((int)stage_timer % 60) : 0;

	//Big enough to store formatted string
	char countdown[50] = { 0 };
	//converts stage_timer into a string/char*
	sprintf_s(countdown, 50, "%d:%02d", minutes, seconds); // seconds formatted to have leading 0 if 1 digit

	if (stage_timer <= 0.0f) {
		draw_gameover_panel(score);
	}

	//blinking effect
	if (stage_timer < 30.0f) {
		if ((int)stage_timer % 2) {
			timer_red_alpha += 255.0f * dt;
			timer_width += timer_width * dt * 0.09f;
			timer_height += timer_height * dt * 0.09f;
			timer_font_size += timer_font_size * dt * 0.25f;
		}
		else {
			timer_red_alpha -= 200.0f * dt;
			timer_width -= timer_width * dt * 0.09f;
			timer_height -= timer_height * dt * 0.09f;
			timer_font_size -= timer_font_size * dt * 0.25f;
		}
		timer_red_alpha = CP_Math_ClampFloat(timer_red_alpha, 0.0f, 255.0f);
		timer_font_color = ((int)(stage_timer * 1.5f) % 2) ? CP_Color_Create(50, 50, 50, 255) : CP_Color_Create(255, 255, 255, 255);
		timer_width = CP_Math_ClampFloat(timer_width, CP_System_GetWindowWidth() / 8.0f * 0.9f, CP_System_GetWindowWidth() / 8.0f * 1.1f);
		timer_height = CP_Math_ClampFloat(timer_height, 70.0f * 0.9f, 70.0f * 1.1f);
	}
	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE); // center text
	//Timer background
	CP_Image_Draw(timer_bg_blue, timer_pos_x, timer_pos_y, timer_width, timer_height, 255);
	CP_Image_Draw(timer_bg_red, timer_pos_x, timer_pos_y, timer_width, timer_height, timer_red_alpha);
	//Timer 
	CP_Font_Set(Font);
	CP_Settings_TextSize(timer_font_size);
	CP_Settings_Fill(timer_font_color);
	CP_Font_DrawText(countdown, timer_pos_x, timer_pos_y);
}

//moves the tool along with the player
void move_tool(float* posx, float* posy, float* rotation) {
	float offset_x = 0.0f;
	float offset_y = 0.0f;

	//Have to make it look like the player is holding it (right handed)
	switch (player.rotation) {
	case 0: //DOWN
		offset_x = -3.5f;
		offset_y = 6.2f;
		break;
	case 90: //LEFT
		offset_x = -3.0f;
		offset_y = 3.0f;
		break;
	case 180: //UP
		offset_x = 3.5f;
		offset_y = 1.0f;
		break;
	case 270: //RIGHT
		offset_x = 2.5f;
		offset_y = 5.0f;
		break;
	default:
		break;
	}
	*posx = player.position.x + offset_x * PLAYER_SCALE;
	*posy = player.position.y + offset_y * PLAYER_SCALE;
	*rotation = player.rotation - 180.0f;
}

//only called when player is holding tool
void handle_tool_interaction(Tool* tool) {
	for (int row = 0; row < TILE_ROWS; ++row) {
		for (int col = 0; col < TILE_COLS; ++col) {
			Tile* const tile = &map_tiles[row][col];

			if (IsWithinRect(tile->x, tile->y,
				CP_System_GetWindowWidth() / TILE_COLS, CP_System_GetWindowHeight() / TILE_ROWS,
				player.position.x, player.position.y) &&
				!(tile->obstructed) && //if no pots on tile
				tile->placeable) { //if tile is placeable (set manually)

				if (CP_Input_KeyReleased(KEY_J)) {
					tile->obstructed = true;
					held_tool = NULL;
					tool->position.x = tile->x;
					tool->position.y = tile->y;
					tool->occupied_tile = tile;
					return;
				}

				//show_tooltip("Press 'J' to Place Down");
			}
		
		}
	}	

	// Progress bar logic
	float rectWidth = 40.0f;
	float rectHeight = 24.0f;
	int potBarSegments = 5;
	float spacing = 0.0f;
	float fillSpeed = 0.4f;
	//--------------------- Plant Interaction ---------------------//
	for (int i = 0; i < L1_POT_SIZE; ++i) {
		if (level1_pots[i].selected) {
			switch (held_tool->tool_name) {
			case WATERING_CAN:
				// If pot isn't empty and plant isn't fully grown
				if (level1_pots[i].plant.seed_type != EMPTY &&
					level1_pots[i].plant.growth_stage != ADULT &&
					!level1_pots[i].plant.wateredOnce) {

					// Only proceed if tool has uses
					if (held_tool->uses > 0) {
						float rectX = level1_pots[i].position.x - rectWidth * potBarSegments / 2.0f;
						float rectY = level1_pots[i].position.y - POT_HEIGHT * POT_SCALE - rectHeight;

						CP_Settings_RectMode(CP_POSITION_CORNER);
						CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
						CP_Settings_StrokeWeight(3.0f);

						// HANDLE WATERING LOGIC
						if (CP_Input_KeyDown(KEY_K)) {
							// Increase fill while holding E
							level1_pots[i].plant.fillProgress += fillSpeed * dt;
							if (level1_pots[i].plant.fillProgress > 1.0f)
								level1_pots[i].plant.fillProgress = 1.0f;
						}

						// DRAW PROGRESS BAR (always draw, even if E is not held)
						float totalBarWidth = potBarSegments * rectWidth + (potBarSegments - 1) * spacing;
						float filledWidth = totalBarWidth * level1_pots[i].plant.fillProgress;
						float thresholdWidth = level1_pots[i].plant.water_needed * rectWidth;

						for (int seg = 0; seg < potBarSegments; seg++) {
							float x = rectX + seg * (rectWidth + spacing);

							// Background color
							if (seg == level1_pots[i].plant.water_needed)
								CP_Settings_Fill(CP_Color_Create(255, 0, 0, 180)); // Red bar
							else
								CP_Settings_Fill(CP_Color_Create(255, 255, 255, 100));

							CP_Graphics_DrawRect(x, rectY, rectWidth, rectHeight);

							// Fill segment
							float segmentStart = seg * (rectWidth + spacing);
							float segmentFill = filledWidth - segmentStart;
							if (segmentFill > 0.0f) {
								if (segmentFill > rectWidth)
									segmentFill = rectWidth;

								CP_Color fillColor;
								if (filledWidth <= thresholdWidth)
									fillColor = CP_Color_Create(0, 0, 255, 180); // Blue
								else {
									float redAmount = (filledWidth - thresholdWidth) / (totalBarWidth - thresholdWidth);
									if (redAmount > 1.0f) redAmount = 1.0f;
									int r = (int)(255.0f * redAmount);
									int g = 0;
									int b = (int)(255.0f * (1.0f - redAmount));
									fillColor = CP_Color_Create(r, g, b, 255);
								}

								CP_Settings_Fill(fillColor);
								CP_Graphics_DrawRect(x, rectY, segmentFill, rectHeight);
							}
						}

						// LOCK POT WHEN E IS RELEASED OR USES RUN OUT
						if (CP_Input_KeyReleased(KEY_K) || held_tool->uses <= 0) {
							level1_pots[i].plant.wateredOnce = true;

							// Grow plant only if fill is within the target segment
							int targetSegment = level1_pots[i].plant.water_needed;
							float segmentStart = targetSegment * (rectWidth + spacing);
							float segmentEnd = segmentStart + rectWidth;

							held_tool->uses--;

							if (filledWidth >= segmentStart && filledWidth <= segmentEnd) {
								level1_pots[i].plant.water_amount = 1.0f;
							}
							else {
								kill_plant(&level1_pots[i]);
							}
						}
					}
				}
				//Controls tool tip at btm right
				CP_Font_Set(myFont);
				show_tooltip("Press & Hold 'K' to Water");

				break;
				
				
			case SOIL:
				if (level1_pots[i].plant.seed_type == EMPTY) {
					if (CP_Input_KeyReleased(KEY_K)) {
						level1_pots[i].filled = true;
					}
					show_tooltip("Press 'K' to Dig Soil");
				}
				if (level1_pots[i].plant.seed_type != EMPTY) {
					if (CP_Input_KeyReleased(KEY_K)) {
						kill_plant(&level1_pots[i]);
					}
					show_tooltip("Press 'K' to Remove Plant");
				}
				break;
			case SEED_1:
				if (level1_pots[i].filled && level1_pots[i].plant.seed_type == EMPTY) {
					if (CP_Input_KeyReleased(KEY_K)) {
						initialize_plant(&level1_pots[i].plant, BERRY_BUSH);
					}
					show_tooltip("Press 'K' to Plant Berry Bush");
				}
				else { show_tooltip("Pot must be empty before planting seed"); }
				break;
			case SEED_2:
				if (level1_pots[i].filled && level1_pots[i].plant.seed_type == EMPTY) {
					if (CP_Input_KeyReleased(KEY_K)) {
						initialize_plant(&level1_pots[i].plant, CACTUS);
					}
					show_tooltip("Press 'K' to Plant Cactus");
				}
				else { show_tooltip("Pot must be empty before planting seed"); }
				break;
			case SEED_3:
				if (level1_pots[i].filled && level1_pots[i].plant.seed_type == EMPTY){
					if (CP_Input_KeyReleased(KEY_K)) {
						initialize_plant(&level1_pots[i].plant, ORANGE_BUSH);
					}
					show_tooltip("Press 'K' to Plant Orange Bush");
				}
				else { show_tooltip("Pot must be empty before planting seed"); }
				break;

			default:
				break;
			}
		}
	}

	//--------------------- Refill Water ---------------------//
	if (held_tool->tool_name == WATERING_CAN &&
		IsWithinRect(map_tiles[2][3].x, map_tiles[2][3].y, // <-- COORDS TO BE CHANGED WITH WELL COORDS
			CP_System_GetWindowWidth() / TILE_COLS, CP_System_GetWindowHeight() / TILE_ROWS,
			player.position.x, player.position.y)) {

		if (CP_Input_KeyTriggered(KEY_K)) {
			held_tool->uses = 3;
			held_tool->tool_sprite = CP_Image_Load("Assets/tool_sprites/water_bucket_full.png");
		}
		show_tooltip("Press 'K' to Refill Bucket");
	}
}

//moves pot along with player
void move_pot(float* posx, float* posy) {
	float offset_x = 0.0f;
	float offset_y = 0.0f;

	//Rough offset for now
	switch (player.rotation) {
	case 0: //DOWN
		offset_x += 0.5f;
		offset_y += 4.0f;
		break;
	case 180: //UP
		offset_x += 0.5f;
		offset_y -= 3.0f;
		break;
	case 90: //LEFT
		offset_x -= 9.0f;
		offset_y += 0.0f;
		break;
	case 270: //RIGHT
		offset_x += 9.0f;
		offset_y += 0.0f;
		break;
	default:
		break;
	}

	*posx = player.position.x + offset_x * PLAYER_SCALE;
	*posy = player.position.y + offset_y * PLAYER_SCALE;
}

//only called when player is holding pot
void handle_pot_interaction(Pot* pot) {
	for (int row = 0; row < TILE_ROWS; ++row) {
		for (int col = 0; col < TILE_COLS; ++col) {
			Tile* const tile = &map_tiles[row][col];

			if (IsWithinRect(tile->x, tile->y,
				CP_System_GetWindowWidth() / TILE_COLS, CP_System_GetWindowHeight() / TILE_ROWS,
				player.position.x, player.position.y) &&
				!(tile->obstructed) && //if no pots on tile
				tile->placeable) { //if tile is placeable (set manually)

				//highlights grid that pot would be placed
				CP_Settings_RectMode(CP_POSITION_CENTER);
				CP_Settings_NoStroke();
				CP_Settings_Fill(CP_Color_Create(255, 255, 0, 80));
				CP_Graphics_DrawRect(tile->x, tile->y,
					CP_System_GetWindowWidth() / TILE_COLS, CP_System_GetWindowHeight() / TILE_ROWS);

				//place down pot if tile is placeable and not obstructed by other things
				if (CP_Input_KeyReleased(KEY_J)) {
					tile->obstructed = true;
					held_tool = NULL;
					held_pot = NULL;
					pot->position.x = tile->x;
					pot->position.y = tile->y;
					pot->occupied_tile = tile;
				}
				show_tooltip("Press 'K' to Place Down");
			}
		}
	}
	CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
}

void spawn_box(void) {
	if (boxCount >= MAX_BOXES) return;

	float screenW = (float)CP_System_GetWindowWidth();
	float screenH = (float)CP_System_GetWindowHeight();

	//  Shift all existing boxes left
	for (int i = 0; i < boxCount; i++) {
		boxes[i].targetX -= (boxes[i].width + 10);  // push left
	}

	//  Create new box on the right
	Box newBox;
	newBox.width = 120; // size of req box // old numbers: 250
	newBox.height = 120; // old numbers: 150
	newBox.x = screenW;        // start off-screen right
	newBox.y = 10.0f;          // top position
	newBox.targetX = screenW - (newBox.width + 10);  // rightmost visible spot
	newBox.speed = 150.0f;

	newBox.removing = 0;
	newBox.removeSpeed = 0.0f;
	newBox.lateTimer = REQUEST_TIMER;

	// --- Random plant request setup ---
	newBox.numPlants = CP_Random_RangeInt(0, 1); // gives 1, 2, or 3
	if (newBox.numPlants < 1) newBox.numPlants = 1; // So that it will spawn at least 1
	if (newBox.numPlants > 3) newBox.numPlants = 3;
	for (int i = 0; i < newBox.numPlants; i++) {
		newBox.plantType = CP_Random_RangeInt(0, L1_TOOL_SIZE - 3);
		if (newBox.plantType > 2) newBox.plantType = 2; // safety clamp
		if (newBox.plantType < 0) newBox.plantType = 0;
	}

	// Add new box to array
	boxes[boxCount++] = newBox;
	//CP_Sound_Play(customerNoti);
}

void customer_request() {
	//--------------------- Create Requests ---------------------//
	spawnTimer += dt;

	// --- START DELAY HANDLING ---
	if (startDelay > 0.0f) {
		startDelay -= dt;

		if (startDelay <= 0.0f) {
			// First request appears after the 10s delay
			spawn_box();

			// Reset timers cleanly
			spawnTimer = 0.0f;
			startDelay = 0.0f;
		}

		return; // Do not spawn normally yet
	}

	// --- NORMAL SPAWNING (after delay finishes) ---
	if (spawnTimer >= REQUEST_INTERVAL && boxCount < MAX_BOXES) {
		spawn_box();
		spawnTimer = 0.0f;
	}

	for (int i = 0; i < boxCount; i++) {
		if (!boxes[i].removing)
		{
			boxes[i].lateTimer -= dt;

			if (boxes[i].lateTimer <= 0.0f)
			{
				boxes[i].removing = 1;
				boxes[i].removeSpeed = 300.0f;
			}
		}
	}

	for (int i = 0; i < boxCount; i++) {

		if (boxes[i].removing)
		{
			boxes[i].y -= boxes[i].removeSpeed * dt;

			// If off-screen, delete it
			if (boxes[i].y + boxes[i].height < 0)
			{
				// shift boxes down
				for (int j = 0; j < boxCount - i; ++j) {
					boxes[i + j] = boxes[i + j + 1];
				}
				if (i != 0) {
					for (int j = 0; j < i; ++j) {
						boxes[j].targetX += boxes[j].width + 10;
					}
				}

				boxCount--;
				i--; // loop checks next box correctly
				continue;
			}
		}

		if (boxes[i].x > boxes[i].targetX + 1.5f) {
			boxes[i].x -= boxes[i].speed * dt;
		}
		else if (boxes[i].x < boxes[i].targetX - 1.5f) {
			boxes[i].x += boxes[i].speed * dt;
		}
		else {
			boxes[i].x = boxes[i].targetX;
		}

		CP_Settings_ImageMode(CP_POSITION_CORNER);
		CP_Image_Draw(requestbg, boxes[i].x, boxes[i].y, boxes[i].width, boxes[i].height, 170);
		CP_Image plantImage = NULL;
		float imgSize = 60.0f; // plant icon size
		float spacing = 10.0f; // space between icons


		int num = boxes[i].numPlants;
		if (num > 0) {
			//float totalWidth = num * imgSize + (num - 1) * spacing;
			float startX = boxes[i].x + (boxes[i].width - imgSize) / 2.0f;
			float startY = boxes[i].y + (boxes[i].height - imgSize) / 2.0f;

			CP_Image plantImage = NULL;

			switch (boxes[i].plantType) {
			case 0: plantImage = berrybush; break;
			case 1: plantImage = cactus; break;
			case 2: plantImage = orangebush; break;
			default: plantImage = berrybush; break; // fallback
			}

			CP_Image_Draw(
				plantImage,
				startX,
				startY,
				imgSize, imgSize, 250
			);
		}

		float timeLeft = boxes[i].lateTimer;
		if (timeLeft < 0) timeLeft = 0;

		CP_Font_Set(Font);
		CP_Settings_TextSize(50);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));

		char timerStr[8];
		snprintf(timerStr, sizeof(timerStr), "%.1f", timeLeft);

		CP_Font_DrawText(timerStr, boxes[i].x + boxes[i].width / 2, boxes[i].y + boxes[i].height + 20);
	}

	//--------------------- Fulfill Request ---------------------//
	Tile* dropoff[2] = { &map_tiles[2][5], &map_tiles[2][6] };

	const char* praises[] = { "NICE!", "AWESOME!", "GOOD JOB!" };
	int numPraises = 3;
	static char currentPraise[32] = "";
	static float praiseTimer = 0.0f;

	//apologies for this mess
	for (int i = 0; i < 2; ++i) {
		//if drop-off point is occupied
		if (dropoff[i]->obstructed) {

			for (int j = 0; j < L1_POT_SIZE; ++j) {
				//find pot that is occupying drop-off point
				if (level1_pots[j].occupied_tile == dropoff[i] &&
					level1_pots[j].plant.growth_stage == ADULT &&
					boxCount > 0) {

					for (int k = 0; k < boxCount; ++k) {
						if (level1_pots[j].plant.seed_type == boxes[k].plantType) {
							if (0 == boxes[k].removing) {
								score += 1000.0f * (1.0f + (boxes[k].lateTimer / 60.0f));

								int idx = rand() % numPraises;
								snprintf(currentPraise, sizeof(currentPraise), "%s", praises[idx]);
								praiseTimer = 1.0f;

								boxes[k].removing = 1;
								boxes[k].removeSpeed = 300.0f;

								//remove plant								
								kill_plant(&level1_pots[j]);
								break; //exit early
							}
						}
					}
				}
			}
		}
	}

	if (praiseTimer > 0.0f)
	{
		CP_Settings_TextSize(80);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Settings_Fill(CP_Color_Create(255, 255, 0, 255)); // yellow text

		float screenW = (float)CP_System_GetWindowWidth();
		float screenH = (float)CP_System_GetWindowHeight();
		CP_Font_DrawText(currentPraise, screenW / 2.0f, screenH / 4.0f);

		praiseTimer -= CP_System_GetDt();
	}
}

void simulate_pests() {
	//--------------------- Pests ---------------------//
	static float pestSpawnTimer = 0.0f;
	static float nextSpawnTime = 0.0f;

	if (pestSpawnTimer >= nextSpawnTime) {
		spawn_pest(level1_pots, L1_POT_SIZE);
		pestSpawnTimer = 0.0f;
		nextSpawnTime = PEST_SPAWN_INTERVAL + ((float)rand() / RAND_MAX) * 5.0f;
	}

	// pass player to update_pests
	update_pests(level1_pots, L1_POT_SIZE, &player);
	render_pests();

	pestSpawnTimer += dt;
}

void draw_gameover_panel(int score)
{
	float w = CP_System_GetWindowWidth();
	float h = CP_System_GetWindowHeight();
	int stars = 0;
	if (score > 5000) {
		stars = 1;
	}
	if (score > 6000) {
		stars = 2;
	}
	if (score > 7000) {
		stars = 3;
	}

	if (stars > 0) {
		if (level == 1) {
			level = 2; // unlock level 2
			saveLevel(level);
		}
		else if (level >= 2) {
			saveLevel(level);
		}
		// if level >= 2, no need to change
	}

	// Dim background
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 180));
	CP_Settings_RectMode(CP_POSITION_CENTER);
	CP_Graphics_DrawRect(w / 2, h / 2, w, h);

	// Panel
	CP_Settings_ImageMode(CP_POSITION_CENTER);
	float panelW = 800, panelH = 500;
	CP_Image_Draw(white_background,w / 2, h / 2, panelW, panelH,255);
	CP_Font_Set(myFont);
	CP_Settings_TextSize(70);
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

	if (stars == 0) {
		// Title
		
		CP_Font_DrawText("Level Failed", w / 2, h / 2 - 130);
		CP_Font_DrawText("Score", w / 2, h / 2 - 30);
	}
	else {
		// Title

		CP_Font_DrawText("Level 1", w / 2, h / 2 - 130);
		CP_Font_DrawText("Score", w / 2, h / 2 - 30);
	}

	char buffer[50];
	sprintf_s(buffer, 50, "%d", score);
	CP_Settings_TextSize(100);
	CP_Font_DrawText(buffer, w / 2, h / 2 + 70);
	// Draw stars

	float starW = 150, starH = 150;
	float spacingX = 100;
	float spacingY = 30; // middle star higher
	float centerX = w / 2;
	float baseY = 200.0f;

	for (int i = 1; i <= 3; i++) // start from 1, go to 3
	{
		float x = centerX + (i - 2) * spacingX; // -1,0,1 for left, center, right
		float y = baseY;
		if (i == 2) y -= spacingY; // middle star higher
		// Set size: middle star bigger, side stars smaller
		float starW = (i == 2) ? 150.0f : 100.0f;
		float starH = (i == 2) ? 150.0f : 100.0f;
		// Set rotation for inverted U effect
		float rotation = 0.0f;
		if (i == 1) rotation = -15.0f; // tilt left star slightly counter-clockwise
		if (i == 3) rotation = 15.0f;  // tilt right star slightly clockwise

		CP_Image img = (i <= stars) ? star_achieved : star_default;
		CP_Image_DrawAdvanced(img, x, y, starW, starH, 255, rotation);
	}

	float btnWidth = 150.0f;
	float btnHeight = 150.0f;
	float btnX[3] = { w / 2 - 250,w / 2, w / 2 + 250 }; // spaced around center
	float btnY = h / 2 + 225; // 


	// Draw buttons
	for (int i = 0; i < 3; i++) {


		if (IsCircleHovered(btnX[i], btnY, btnWidth))
			CP_Image_Draw(button_circle_green, btnX[i], btnY, btnWidth, btnHeight, 255);
		else
			CP_Image_Draw(button_circle_white, btnX[i], btnY, btnWidth, btnHeight, 255);

		if (i == 0) {
			CP_Image_Draw(home, btnX[i], btnY, btnWidth - 75.0f, btnHeight - 75.0f, 255);
		}
		if (i == 1) {
			CP_Image_Draw(repeat, btnX[i], btnY, btnWidth - 75.0f, btnHeight - 75.0f, 255);
		}
		if (i == 2) {
			CP_Image_Draw(next, btnX[i], btnY, btnWidth - 75.0f, btnHeight - 75.0f, 255);
		}

	}

	// Play click sound for any button
	for (int i = 0; i < 2; i++) {
		if (IsButtonClicked(btnX[i], btnY, btnWidth - 75.0f, btnHeight - 75.0f)) {
			CP_Sound_Play(BtnSound);
			break;
		}
	}

	// Button actions
	if (IsCircleClicked(btnX[0], btnY, 150, CP_Input_GetMouseX(), CP_Input_GetMouseY())) {
		CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
	}

	if (IsCircleClicked(btnX[1], btnY, 150, CP_Input_GetMouseX(), CP_Input_GetMouseY())) {
		CP_Engine_SetNextGameStateForced(Game_Init, Game_Update, Game_Exit);
	}

	if (IsCircleClicked(btnX[2], btnY, 150, CP_Input_GetMouseX(), CP_Input_GetMouseY()))
	{
		if (level < 2) {
			showText = true;
			flashTimer = 0;
		}
		else {
			CP_Engine_SetNextGameState(Game_Init, Game_Update, Game_Exit);
		}
	}

	if (CP_Input_KeyTriggered(KEY_ESCAPE)) {
		CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
	}
}
