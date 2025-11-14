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
#define L1_TOOL_SIZE 3
Tool level1_tools[L1_TOOL_SIZE];
Tool_name level1_tool_names[L1_TOOL_SIZE] = { WATERING_CAN, SOIL, SEEDS };
Tool held_tool;

//--------------------- Timer ---------------------//
float stage_timer;
CP_Image timer_bg_blue = NULL, timer_bg_red = NULL;
float timer_pos_x, timer_pos_y;
float timer_width, timer_height;
float timer_font_size;
CP_Color timer_font_color;


CP_Image requestbg = NULL;
CP_Image berrybush = NULL;
CP_Image orangebush = NULL;
CP_Image cactus = NULL;

CP_Sound customerNoti;

typedef struct {
	float x, y;
	float targetX;
	float width, height;
	float speed;
	int removing;
	float removeSpeed;
	float lateTimer;

	// New fields for plant requests
	int numPlants;       // how many plants requested (1–3)
	int plantTypes[3];   // 0 = cactus | 1 = berrybush | 2 = orangebush

} Box;

// Maximum number of box
#define MAX_BOXES 5
Box boxes[MAX_BOXES];
int boxCount;
float spawnTimer;


Tile map_tiles[TILE_ROWS][TILE_COLS];



//--------------------- Pots ---------------------//
#define L1_POT_SIZE 4
Pot level1_pots[L1_POT_SIZE];

void Game_Init(void)
{
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	myFont = CP_Font_Load("Assets/Kenney Pixel Square.ttf");

	CP_Settings_ImageMode(CP_POSITION_CORNER);

	spriteSheetTiles = CP_Image_Load("Assets/tilemap_packed.png");

	//ahmad
	srand((unsigned int)time(NULL));
	requestbg = CP_Image_Load("Assets/button_square_gradient.png");
	customerNoti = CP_Sound_Load("Assets/BellBrass CTE01_28.5.wav");
	berrybush = CP_Image_Load("Assets/seed_berrybush.png");
	cactus = CP_Image_Load("Assets/seed_cactus.png");
	orangebush = CP_Image_Load("Assets/seed_orangebush.png");
	CP_System_SetFrameRate(60);
	boxCount = 0;
	spawnTimer = 0;

	//Initialize player with default values
	player_speed = 300.0f;
	player_startpos_x = player_startpos_y = 100.0f;
	initialize_player(&player, player_speed, player_startpos_x, player_startpos_y);
	player_spritesheet = CP_Image_Load("Assets/farmer_spritesheet.png");

	//Tools
	initialize_tools(level1_tools, L1_TOOL_SIZE, level1_tool_names, &held_tool);


	//Timer
#pragma region Timer Initialization

	stage_timer = 35.0f;
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
	initialize_pots(level1_pots, L1_POT_SIZE);

	// pest
	initialize_pests();

}


void Game_Update(void)
{
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);




	draw_tile_map(map_tiles);
		
	draw_modified_map();
	
	CP_Color blue = CP_Color_Create(0, 0, 255, 255);
	CP_Color red = CP_Color_Create(220, 100, 100, 255);
	CP_Color green = CP_Color_Create(100, 220, 100, 255);
	CP_Color gray = CP_Color_Create(210, 210, 210, 255);

	//--------------------- Pots ---------------------//
	render_pots(level1_pots, L1_POT_SIZE, player.position.x, player.position.y);

	//--------------------- Tools ---------------------//
	render_tools(level1_tools, L1_TOOL_SIZE, player.position.x, player.position.y, held_tool.tool_name);
	//Render tool behind player if player facing Left or Up
	if (held_tool.tool_name != HAND && (90 == player.rotation || 180 == player.rotation)) {
		render_held_tool(&held_tool);
	}

	//--------------------- Hold Tool ---------------------//
	//If player is not holding anything
	if (HAND == held_tool.tool_name) {
		if (CP_Input_KeyReleased(KEY_Q)) {
			//Picks up the first tool that is in range
			for (int i = 0; i < L1_TOOL_SIZE; ++i) {
				if (level1_tools[i].in_range) {
					held_tool = level1_tools[i];
					break;
				}
			}
		}
	}
	//If player is holding something
	else {
		move_tool(&held_tool.position.x, &held_tool.position.y, &held_tool.rotation);
		handle_tool_interaction(&held_tool);
	}

	//--------------------- Player ---------------------//
	//EVERYTH PLAYER RELATED IN THIS FUNCTION
	simulate_player(&player, player_spritesheet);
	//Render tool infront if player facing Right or Down
	if (held_tool.tool_name != HAND && (0 == player.rotation || 270 == player.rotation)) {
		render_held_tool(&held_tool);
	}

	/*-----------------------------------------------------------------------*/
	CP_Settings_Fill(red);
	//initialise the x and y coordinate of red circle
	float red_circlex = 500.0f;
	float red_circley = 600.0f;
	//draw first big circle at the middle height of screen and 1/4 width from the left
	CP_Graphics_DrawCircle(red_circlex, red_circley, 100.0f);
	
	static int inRedZone = 0;
	//if small circle is intersecting red circle
	int touchingRed = AreCirclesIntersecting(player.position.x, player.position.y, 9.0f, red_circlex, red_circley, 100.0f / 2.0f);
	


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


	const char* praises[] = { "NICE!", "AWESOME!", "GOOD JOB!" };
	int numPraises = 3;
	static char currentPraise[32] = "";
	static float praiseTimer = 0.0f;

	if (touchingRed && !inRedZone)
	{

		// Remove ONE box
		if (boxCount > 0 && boxes[0].removing == 0)
		{
			boxes[0].removing = 1;
			boxes[0].removeSpeed = 300.0f;
		}
		int idx = rand() % numPraises;
		snprintf(currentPraise, sizeof(currentPraise), "%s", praises[idx]);
		praiseTimer = 1.0f; // show for 1 second
		inRedZone = 1;
	}
	else if (!touchingRed)
	{
		inRedZone = 0;
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


	//ahmad
	spawnTimer += dt;

	// Spawn every 2 seconds
	if (spawnTimer >= 5.0f) {
		spawn_box();
		spawnTimer = 0.0f;
	}

	for (int i = 0; i < boxCount; i++) {

		if (boxes[i].removing)
		{
			boxes[i].y -= boxes[i].removeSpeed * dt;

			// If off-screen, delete it
			if (boxes[i].y + boxes[i].height < 0)
			{
				// shift boxes down
				for (int j = 1; j < boxCount; j++)
					boxes[j - 1] = boxes[j];

				boxCount--;
				i--; // loop checks next box correctly
				continue;
			}
		}

		if (boxes[i].x > boxes[i].targetX) {
			boxes[i].x -= boxes[i].speed;
			if (boxes[i].x < boxes[i].targetX)
				boxes[i].x = boxes[i].targetX;
		}

		CP_Settings_ImageMode(CP_POSITION_CORNER);
		CP_Image_Draw(requestbg, boxes[i].x, boxes[i].y, boxes[i].width, boxes[i].height, 170);
		CP_Image plantImage = NULL;
		float imgSize = 65.0f; // plant icon size
		float spacing = 10.0f; // space between icons

		
		int num = boxes[i].numPlants;
		if (num > 0) {
			float totalWidth = num * imgSize + (num - 1) * spacing;
			float startX = boxes[i].x + (boxes[i].width - totalWidth) / 2.0f;
			float startY = boxes[i].y + (boxes[i].height - imgSize) / 2.0f;

			for (int p = 0; p < num; p++) {
				CP_Image plantImage = NULL;

				switch (boxes[i].plantTypes[p]) {
				case 0: plantImage = cactus; break;
				case 1: plantImage = berrybush; break;
				case 2: plantImage = orangebush; break;
				default: plantImage = cactus; break; // fallback
				}

				CP_Image_Draw(
					plantImage,
					startX + p * (imgSize + spacing),
					startY,
					imgSize, imgSize, 250
				);
			}
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
//}
	// --------------------- PEST -------------------------------//
	static float pestSpawnTimer = 0.0f;
	static float nextSpawnTime = 0.0f;

	pestSpawnTimer += dt;

	if (pestSpawnTimer >= nextSpawnTime) {
		spawn_pest(level1_pots, L1_POT_SIZE);
		pestSpawnTimer = 0.0f;
		nextSpawnTime = 3.0f + ((float)rand() / RAND_MAX) * 5.0f;
	}

	// pass player to update_pests
	update_pests(level1_pots, L1_POT_SIZE, &player);
	render_pests();


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
	CP_Font_Free(myFont);
	CP_Image_Free(timer_bg_blue);
	CP_Image_Free(timer_bg_red);
	for (int i = 0; i < L1_TOOL_SIZE; i++) {
		CP_Image_Free(level1_tools[i].tool_sprite);
	}
	CP_Image_Free(requestbg);
	CP_Sound_Free(customerNoti);

	//rmb free my imgs
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
	if (AreCirclesIntersecting(player.position.x, player.position.y, 9.0f,
		tool->initial_pos.x, tool->initial_pos.y, 40.0f + tool_range)) {

		tool->in_range = true;

		if (CP_Input_KeyReleased(KEY_Q)) {
			tool->tool_name = HAND;
			tool->position.x = tool->initial_pos.x;
			tool->position.y = tool->initial_pos.y;
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
			switch (held_tool.tool_name) {
			case WATERING_CAN:
				// Only allow watering if pot hasn't been watered before
				if (!level1_pots[i].wateredOnce && level1_pots[i].plant.seed_type != EMPTY) {

					// Progress bar logic
					float rectWidth = 60;
					float rectHeight = 30;
					int potBarSegments = 5;
					float spacing = 0.0f;
					float fillSpeed = 0.4f;

					float rectX = level1_pots[i].position.x - rectWidth / 2.0f;
					float rectY = level1_pots[i].position.y - (120.0f / 2.0f) - rectHeight - 10.0f;

					CP_Settings_RectMode(CP_POSITION_CORNER);

					// Only increase fill if E is held
					if (CP_Input_KeyDown(KEY_E)) {
						level1_pots[i].fillProgress += fillSpeed * dt;
						if (level1_pots[i].fillProgress > 1.0f) level1_pots[i].fillProgress = 1.0f;
					}

					// Draw progress bar
					float totalBarWidth = potBarSegments * rectWidth + (potBarSegments - 1) * spacing;
					float filledWidth = totalBarWidth * level1_pots[i].fillProgress;
					float thresholdWidth = (potBarSegments - 2) * rectWidth;

					for (int seg = 0; seg < potBarSegments; seg++) {
						float x = rectX + seg * (rectWidth + spacing) - 82;

						// Background color for segment
						if (seg == potBarSegments - 2)
							CP_Settings_Fill(CP_Color_Create(255, 150, 150, 255)); // Red bar
						else
							CP_Settings_Fill(CP_Color_Create(120, 120, 120, 255));

						CP_Graphics_DrawRect(x, rectY, rectWidth, rectHeight);

						// Fill segment
						float segmentStart = seg * (rectWidth + spacing);
						float segmentFill = filledWidth - segmentStart;
						if (segmentFill > 0.0f) {
							if (segmentFill > rectWidth) segmentFill = rectWidth;

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

					// Lock watering when E released
					if (CP_Input_KeyReleased(KEY_E)) {
						level1_pots[i].wateredOnce = true;
						level1_pots[i].watered = true; // lock pot

						float totalBarWidth = potBarSegments * rectWidth + (potBarSegments - 1) * spacing;
						float filledWidth = totalBarWidth * level1_pots[i].fillProgress;

						// 2nd last segment boundaries
						int targetSegment = potBarSegments - 2; // index of 2nd last
						float segmentStart = targetSegment * (rectWidth + spacing);
						float segmentEnd = segmentStart + rectWidth;

						// Grow only if blue bar is within 2nd last segment
						if (filledWidth >= segmentStart && filledWidth <= segmentEnd) {
							grow_plant(&level1_pots[i].plant.growth_stage);
						}
					}
				}

				break;


			case SOIL:
				if (CP_Input_KeyTriggered(KEY_E)) {
					level1_pots[i].filled = true;
				}
				break;

			case SEEDS:
				if (level1_pots[i].filled && CP_Input_KeyTriggered(KEY_E)) {
					initialize_plant(&level1_pots[i].plant);
				}
				break;

			default:
				break;
			}
		}
	}
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
	newBox.width = 150; // size of req box // old numbers: 250
	newBox.height = 150; // old numbers: 150
	newBox.x = screenW;        // start off-screen right
	newBox.y = 10.0f;          // top position
	newBox.targetX = screenW - (newBox.width + 10);  // rightmost visible spot
	newBox.speed = 5.0f;

	newBox.removing = 0;
	newBox.removeSpeed = 0.0f;
	newBox.lateTimer = 15.0f;

	// --- Random plant request setup ---
	newBox.numPlants = CP_Random_RangeInt(0, 1); // gives 1, 2, or 3
	if (newBox.numPlants < 1) newBox.numPlants = 1; // So that it will spawn at least 1
	if (newBox.numPlants > 3) newBox.numPlants = 3;
	for (int i = 0; i < newBox.numPlants; i++) {
		newBox.plantTypes[i] = CP_Random_RangeInt(0, 3);
		if (newBox.plantTypes[i] > 2) newBox.plantTypes[i] = 2; // safety clamp
		if (newBox.plantTypes[i] < 0) newBox.plantTypes[i] = 0;
	}

	// Add new box to array
	boxes[boxCount++] = newBox;
	CP_Sound_Play(customerNoti);
}
