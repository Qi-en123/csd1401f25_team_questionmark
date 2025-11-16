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


CP_Font myFont;

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
#define REQUEST_TIMER 13.0f
#define REQUEST_INTERVAL 2.0f

//--------------------- Score ---------------------//
//int three_star_val = 1200;
//int two_star_val = 800;
//int three_star_val = 500;
//

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

//Frequently used colors
CP_Color red, green, blue, gray;

void Game_Init(void)
{
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	myFont = CP_Font_Load("Assets/Kenney Pixel Square.ttf");

	CP_Settings_ImageMode(CP_POSITION_CORNER);

	spriteSheetTiles = CP_Image_Load("Assets/tilemap_packed.png");
	draw_tile_map(map_tiles);

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
	player_startpos_x = player_startpos_y = 100.0f;
	initialize_player(&player, player_speed, player_startpos_x, player_startpos_y);
	player_spritesheet = CP_Image_Load("Assets/farmer_spritesheet.png");

	//Tools
	initialize_tools(level1_tools, L1_TOOL_SIZE, level1_tool_names, &map_tiles[3][1]);
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
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);

	draw_tile_map(map_tiles);
	set_tile_placeable(map_tiles, &placeable, sizeof(placeable) / sizeof(placeable[0]));

	draw_modified_map();

	//--------------------- Pots ---------------------//
	render_pots(level1_pots, pot_spritesheet, L1_POT_SIZE, player.position.x, player.position.y, held_pot);

	//--------------------- Tools ---------------------//
	render_tools(level1_tools, L1_TOOL_SIZE, player.position.x, player.position.y, held_tool);
	//Render tool behind player if player facing Left or Up
	if (90 == player.rotation || 180 == player.rotation) {
		if (held_tool) { render_held_tool(held_tool); }
		if (held_pot) { render_held_pot(held_pot, pot_spritesheet); }
	}

	//--------------------- Hold Tool ---------------------//
	//If player is not holding anything
	if (!held_tool && !held_pot) {
		if (CP_Input_KeyReleased(KEY_Q)) {
			//Picks up the first tool that is in range
			for (int i = 0; i < L1_TOOL_SIZE; ++i) {
				if (level1_tools[i].in_range) {
					held_tool = &level1_tools[i];
					break;
				}
			}
			//Picks up first pot that is in range
			for (int i = 0; i < L1_POT_SIZE; ++i) {
				if (level1_pots[i].selected) {
					held_pot = &level1_pots[i];
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

	//--------------------- Player ---------------------//
	//EVERYTH PLAYER RELATED IN THIS FUNCTION
	simulate_player(&player, player_spritesheet);
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

	// if ESC is pressed, go back main menu page
	if (CP_Input_KeyDown(KEY_ESCAPE)) { CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit); }
}

void Game_Exit(void)
{
	CP_Image_Free(spriteSheetTiles);
	CP_Image_Free(player_spritesheet);
	CP_Image_Free(pest_spritesheet);
	CP_Font_Free(myFont);
	CP_Image_Free(timer_bg_blue);
	CP_Image_Free(timer_bg_red);
	for (int i = 0; i < L1_TOOL_SIZE; ++i) {
		CP_Image_Free(level1_tools[i].tool_sprite);
	}
	if (held_tool) { CP_Image_Free(held_tool->tool_sprite); }
	CP_Image_Free(pot_spritesheet);
	for (int i = 0; i < L1_POT_SIZE; ++i) {
		CP_Image_Free(level1_pots[i].plant.plant_spritesheet);
	}
	CP_Image_Free(requestbg);
	CP_Sound_Free(customerNoti);
	CP_Image_Free(berrybush);
	CP_Image_Free(cactus);
	CP_Image_Free(orangebush);
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
		offset_x = 0.0f;
		offset_y = 44.0f;
		break;
	case 90: //LEFT
		offset_x = 0.0f;
		offset_y = 30.0f;
		break;
	case 180: //UP
		offset_x = 35.0f;
		offset_y = 15.0f;
		break;
	case 270: //RIGHT
		offset_x = 25.0f;
		offset_y = 38.0f;
		break;
	default:
		break;
	}
	*posx = player.position.x + offset_x;
	*posy = player.position.y + offset_y;
	*rotation = player.rotation - 180.0f;
}

//only called when player is holding tool
void handle_tool_interaction(Tool* tool) {
	//if player is back at where he picked up the tool
	if (AreCirclesIntersecting(player.position.x, player.position.y + 8.0f, 9.0f, //abit of offset on y-axis for accuracy
		tool->initial_pos.x, tool->initial_pos.y - TOOL_RANGE_Y_OFFSET, 40.0f + TOOL_RANGE)) {

		tool->in_range = true;
		//highlights initial grid
		CP_Settings_RectMode(CP_POSITION_CORNER);
		CP_Settings_NoStroke();
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, 80));
		CP_Graphics_DrawRect(tool->initial_pos.x, tool->initial_pos.y,
			CP_System_GetWindowWidth() / TILE_COLS, CP_System_GetWindowHeight() / TILE_ROWS);

		//reset stroke color
		CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
		if (CP_Input_KeyReleased(KEY_Q)) {
			tool->position.x = tool->initial_pos.x;
			tool->position.y = tool->initial_pos.y;
			held_tool = NULL;
			return;
		}
	}
	else tool->in_range = false;

	// Progress bar logic
	float rectWidth = 60;
	float rectHeight = 30;
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
						float rectX = level1_pots[i].position.x - rectWidth / 2.0f;
						float rectY = level1_pots[i].position.y - (120.0f / 2.0f) - rectHeight - 10.0f;

						CP_Settings_RectMode(CP_POSITION_CORNER);

						// HANDLE WATERING LOGIC
						if (CP_Input_KeyDown(KEY_E)) {
							// Increase fill while holding E
							level1_pots[i].plant.fillProgress += fillSpeed * dt;
							if (level1_pots[i].plant.fillProgress > 1.0f)
								level1_pots[i].plant.fillProgress = 1.0f;

							// Gradually decrease uses while holding E
							//float useDrainRate = 1.0f; // adjust to change how fast uses drain
							//held_tool->uses -= dt * useDrainRate;
							//if (held_tool->uses < 0)
							//	held_tool->uses = 0;
						}

						// DRAW PROGRESS BAR (always draw, even if E is not held)
						float totalBarWidth = potBarSegments * rectWidth + (potBarSegments - 1) * spacing;
						float filledWidth = totalBarWidth * level1_pots[i].plant.fillProgress;
						float thresholdWidth = level1_pots[i].plant.water_needed * rectWidth;

						for (int seg = 0; seg < potBarSegments; seg++) {
							float x = rectX + seg * (rectWidth + spacing) - 82;

							// Background color
							if (seg == level1_pots[i].plant.water_needed)
								CP_Settings_Fill(CP_Color_Create(255, 150, 150, 255)); // Red bar
							else
								CP_Settings_Fill(CP_Color_Create(120, 120, 120, 255));

							CP_Graphics_DrawRect(x, rectY, rectWidth, rectHeight);

							// Fill segment
							float segmentStart = seg * (rectWidth + spacing);
							float segmentFill = filledWidth - segmentStart;
							if (segmentFill > 0.0f) {
								if (segmentFill > rectWidth)
									segmentFill = rectWidth;

								CP_Color fillColor;
								if (filledWidth <= thresholdWidth)
									fillColor = CP_Color_Create(0, 0, 255, 255); // Blue
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
						if (CP_Input_KeyReleased(KEY_E) || held_tool->uses <= 0) {
							level1_pots[i].plant.wateredOnce = true;

							// Grow plant only if fill is within the target segment
							int targetSegment = level1_pots[i].plant.water_needed; // 2nd last segment
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
				break;

			case SOIL:
				if (level1_pots[i].plant.seed_type == EMPTY && CP_Input_KeyReleased(KEY_E)) {
					level1_pots[i].filled = true;
				}
				if (level1_pots[i].plant.seed_type != EMPTY && CP_Input_KeyReleased(KEY_E)) {
					kill_plant(&level1_pots[i]);
				}
				break;
			case SEED_1:
				if (level1_pots[i].filled && level1_pots[i].plant.seed_type == EMPTY && CP_Input_KeyReleased(KEY_E)) {
					initialize_plant(&level1_pots[i].plant, BERRY_BUSH); }
				break;
			case SEED_2:
				if (level1_pots[i].filled && level1_pots[i].plant.seed_type == EMPTY && CP_Input_KeyReleased(KEY_E)) {
					initialize_plant(&level1_pots[i].plant, CACTUS); }
				break;
			case SEED_3:
				if (level1_pots[i].filled && level1_pots[i].plant.seed_type == EMPTY && CP_Input_KeyReleased(KEY_E)) {
					initialize_plant(&level1_pots[i].plant, ORANGE_BUSH); }
				break;

			default:
				break;
			}
		}
	}

	//--------------------- Refill Water ---------------------//
	if (CP_Input_KeyTriggered(KEY_E) && held_tool->tool_name == WATERING_CAN &&
		IsWithinRect(map_tiles[2][2].x, map_tiles[2][2].y, // <-- COORDS TO BE CHANGED WITH WELL COORDS
			CP_System_GetWindowWidth() / TILE_COLS, CP_System_GetWindowHeight() / TILE_ROWS,
			player.position.x, player.position.y)) {

		held_tool->uses = 3;
		held_tool->tool_sprite = CP_Image_Load("Assets/tool_sprites/water_bucket_full.png");
	}
}

//moves pot along with player
void move_pot(float* posx, float* posy) {
	float offset_x = 0.0f;
	float offset_y = 0.0f;

	//Rough offset for now
	switch (player.rotation) {
	case 0: //DOWN
		offset_x += 5.0f;
		offset_y += 10.0f;
		break;
	case 180: //UP
		offset_x += 5.0f;
		offset_y -= 12.0f;
		break;
	case 90: //LEFT
		offset_x -= 40.0f;
		offset_y += 6.0f;
		break;
	case 270: //RIGHT
		offset_x += 40.0f;
		offset_y += 6.0f;
		break;
	default:
		break;
	}


	*posx = player.position.x + offset_x;
	*posy = player.position.y + offset_y;
}

//only called when player is holding pot
void handle_pot_interaction(Pot* pot) {
	for (int row = 0; row < TILE_ROWS; ++row) {
		for (int col = 0; col < TILE_COLS; ++col) {
			Tile* const tile = &map_tiles[row][col];

			if (IsWithinRect(tile->x, tile->y,
				CP_System_GetWindowWidth() / TILE_COLS, CP_System_GetWindowHeight() / TILE_ROWS,
				player.position.x, player.position.y)) {

				//highlights grid that pot would be placed
				CP_Settings_RectMode(CP_POSITION_CORNER);
				CP_Settings_NoStroke();
				CP_Settings_Fill(CP_Color_Create(255, 255, 255, 80));
				CP_Graphics_DrawRect(tile->x, tile->y,
					CP_System_GetWindowWidth() / TILE_COLS, CP_System_GetWindowHeight() / TILE_ROWS);

				//place down pot if tile is placeable and not obstructed by other things
				if (CP_Input_KeyReleased(KEY_Q) && tile->placeable && !(tile->obstructed)) {
					tile->obstructed = true;
					held_tool = NULL;
					held_pot = NULL;
					pot->position.x = tile->x;
					pot->position.y = tile->y;
					pot->occupied_tile = tile;
				}
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
		newBox.plantType = CP_Random_RangeInt(0, L1_TOOL_SIZE-3);
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

	// Spawn every few seconds
	if (startDelay > 0.0f) {
		startDelay -= dt;
		if (startDelay <= 0.0f) {
			spawn_box();
			spawnTimer = 0.0f;
		}
	}
	else {
		if (spawnTimer >= REQUEST_INTERVAL) {
			spawn_box();
			spawnTimer = 0.0f;
		}
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
				//for (int j = 1; j < boxCount; j++)
				//	boxes[j - 1] = boxes[j];
				for (int j = 0; j < boxCount; ++j) {
					boxes[i+j] = boxes[i+j+1];
				}
				if (i != 0) {
					for (int j = 0; j < boxCount - (boxCount - i); ++j) {
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
				imgSize , imgSize, 250
			);
		}

		float timeLeft = boxes[i].lateTimer;
		if (timeLeft < 0) timeLeft = 0;

		CP_Settings_TextSize(50);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));

		char timerStr[8];
		snprintf(timerStr, sizeof(timerStr), "%.1f", timeLeft);

		CP_Font_DrawText(timerStr, boxes[i].x + boxes[i].width / 2, boxes[i].y + boxes[i].height + 20);
	}

	//--------------------- Fulfill Request ---------------------//
	Tile* dropoff[2] = { &map_tiles[2][15], &map_tiles[2][16] };

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

int add_score(Box* box) {
	int score = 100;

	//60secs -- 30secs Fulfil - 2x score
	//		 -- 0secs Fulfil - 1x score
	//
	//stars 0: 

	//when fulfil request - get score based on time left
	//x2 score if u fufill within half the time limit??
	//score += (int)box->lateTimer ;

	return score;
}
