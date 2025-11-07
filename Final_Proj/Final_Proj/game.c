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

CP_Font myFont;

//--------------------- Player ---------------------//
Player player;
float player_speed;
float player_startpos_x, player_startpos_y;
CP_Image player_spritesheet;

//--------------------- Tools ---------------------//
#define L1_TOOL_SIZE 5
Tool level1_tools[L1_TOOL_SIZE];
Tool_name level1_tool_names[L1_TOOL_SIZE] = { WATERING_CAN, SOIL, SEED_1, SEED_2, SEED_3 };
Tool held_tool; // a copy of the tool to be held

//--------------------- Pots ---------------------//
#define L1_POT_SIZE 40
Pot level1_pots[L1_POT_SIZE];
Pot* held_pot; // a pointer to the pot held (realised i could do it this way)
CP_Image pot_spritesheet = NULL;


//--------------------- Timer ---------------------//
float stage_timer;
CP_Image timer_bg_blue = NULL, timer_bg_red = NULL;
float timer_pos_x, timer_pos_y;
float timer_width, timer_height;
float timer_font_size;
CP_Color timer_font_color;


CP_Image requestbg = NULL;
CP_Sound customerNoti;

typedef struct {
	float x, y;
	float targetX;
	float width, height;
	float speed;
	int removing;
	float removeSpeed;
	float lateTimer;
} Box;

// Maximum number of box
#define MAX_BOXES 5
Box boxes[MAX_BOXES];
int boxCount;
float spawnTimer;


Tile map_tiles[TILE_ROWS][TILE_COLS];
int placeable[] = { 1,2 };

void Game_Init(void)
{
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	myFont = CP_Font_Load("Assets/Kenney Pixel Square.ttf");

	CP_Settings_ImageMode(CP_POSITION_CORNER);

	spriteSheetTiles = CP_Image_Load("Assets/tilemap_packed.png");
	draw_tile_map(map_tiles);

	//ahmad
	srand((unsigned int)time(NULL));
	requestbg = CP_Image_Load("Assets/button_square_gradient.png");
	customerNoti = CP_Sound_Load("Assets/BellBrass CTE01_28.5.wav");
	CP_System_SetFrameRate(60);
	boxCount = 0;
	spawnTimer = 0;

	//Initialize player with default values
	player_speed = 300.0f;
	player_startpos_x = player_startpos_y = 100.0f;
	initialize_player(&player, player_speed, player_startpos_x, player_startpos_y);
	player_spritesheet = CP_Image_Load("Assets/farmer_spritesheet.png");

	//Tools
	initialize_tools(level1_tools, L1_TOOL_SIZE, level1_tool_names, &held_tool, &map_tiles[3][1]);

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
	initialize_pots(level1_pots, L1_POT_SIZE, &map_tiles[5][5]);
	pot_spritesheet = CP_Image_Load("Assets/tool_sprites/pot_spritesheet.png");
	held_pot = NULL;
}


void Game_Update(void)
{
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);

	draw_tile_map(map_tiles);
	set_tile_placeable(map_tiles, &placeable, sizeof(placeable)/sizeof(placeable[0]));
		
	draw_modified_map();
	
	CP_Color blue = CP_Color_Create(0, 0, 255, 255);
	CP_Color red = CP_Color_Create(220, 100, 100, 255);
	CP_Color green = CP_Color_Create(100, 220, 100, 255);
	CP_Color gray = CP_Color_Create(210, 210, 210, 255);

	//--------------------- Pots ---------------------//
	render_pots(level1_pots, pot_spritesheet, L1_POT_SIZE, player.position.x, player.position.y, held_pot);

	//--------------------- Tools ---------------------//
	render_tools(level1_tools, L1_TOOL_SIZE, player.position.x, player.position.y, held_tool.tool_name);
	//Render tool behind player if player facing Left or Up
	if (held_tool.tool_name != HAND && (90 == player.rotation || 180 == player.rotation)) {
		render_held_tool(&held_tool);
		if (held_tool.tool_name == POT) { render_held_pot(held_pot, pot_spritesheet); }
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
			//Picks up first pot that is in range
			for (int i = 0; i < L1_POT_SIZE; ++i) {
				if (level1_pots[i].selected) {
					held_tool.tool_name = POT;
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
			move_tool(&held_tool.position.x, &held_tool.position.y, &held_tool.rotation);
			handle_tool_interaction(&held_tool);
		}
	}

	//--------------------- Player ---------------------//
	//EVERYTH PLAYER RELATED IN THIS FUNCTION
	simulate_player(&player, player_spritesheet);
	//Render tool infront if player facing Right or Down
	if (held_tool.tool_name != HAND && (0 == player.rotation || 270 == player.rotation)) {
		render_held_tool(&held_tool);
		if (held_tool.tool_name == POT) { render_held_pot(held_pot, pot_spritesheet); } 
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
	if (spawnTimer >= 20.0f) {
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
		CP_Image_Draw(requestbg, boxes[i].x, boxes[i].y, boxes[i].width, boxes[i].height, 255);
		float timeLeft = boxes[i].lateTimer;
		if (timeLeft < 0) timeLeft = 0;

		CP_Settings_TextSize(50);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));

		char timerStr[8];
		snprintf(timerStr, sizeof(timerStr), "%.1f", timeLeft);

		CP_Font_DrawText(timerStr, boxes[i].x + boxes[i].width / 2, boxes[i].y + boxes[i].height + 20);
	}
	//ahmad


	CP_Settings_Fill(CP_Color_Create(164, 147, 147, 255));
	float potX = CP_System_GetWindowWidth() / 2.0f;
	float potY = CP_System_GetWindowHeight() / 2.0f;
	CP_Graphics_DrawCircle(potX, potY, 120.0f);

	float rectWidth = 60;
	float rectHeight = 30;
	float potRadius = 120.0f;
	int count = 5; // how many rectangles to draw
	float spacing = 0.0f;

	float totalWidth = count * rectWidth + (count - 1) * spacing;

	static float fillProgress = 0.0f; // 0 = empty, 1 = full
	const int potBarSegments = 5; // number of segments
	float fillSpeed = 0.4f; // fill per second

	float rectX = potX - rectWidth / 2.0f;   // center horizontally on pot
	float rectY = potY - (120.0f / 2.0f) - rectHeight - 10.0f;

	if (AreCirclesIntersecting(player.position.x, player.position.y, 120.0f / 2.0f, potX, potY, 120.0f / 2.0f))
	{
		// Fill while holding L
		if (CP_Input_KeyDown(KEY_L))
		{
			CP_Settings_RectMode(CP_POSITION_CORNER);
			fillProgress += fillSpeed * dt;
			if (fillProgress > 1.0f) fillProgress = 1.0f;

		}

		float totalBarWidth = potBarSegments * rectWidth + (potBarSegments - 1) * spacing;
		float filledWidth = totalBarWidth * fillProgress;

		float thresholdWidth = (potBarSegments - 2) * rectWidth; // start red after 2nd last segment

		for (int i = 0; i < potBarSegments; i++)
		{
			float x = rectX + i * (rectWidth + spacing) - 82;

			// Grey background
			CP_Settings_Fill(CP_Color_Create(120, 120, 120, 255));
			CP_Graphics_DrawRect(x, rectY, rectWidth, rectHeight);

			// Fill calculation
			float segmentStart = i * (rectWidth + spacing);
			float segmentFill = filledWidth - segmentStart;
			if (segmentFill > 0.0f)
			{
				if (segmentFill > rectWidth) segmentFill = rectWidth;

				// Smooth transition: calculate red intensity
				CP_Color fillColor;
				if (filledWidth <= thresholdWidth)
				{
					fillColor = CP_Color_Create(0, 0, 255, 255); // blue
				}
				else
				{
					// progress past threshold (0..1)
					float redAmount = (filledWidth - thresholdWidth) / (totalBarWidth - thresholdWidth);
					if (redAmount > 1.0f) redAmount = 1.0f;
					int r = (int)(255.0f * redAmount);
					int g = (int)(0.0f);
					int b = (int)(255.0f * (1.0f - redAmount));
					fillColor = CP_Color_Create(r, g, b, 255);
				}

				CP_Settings_Fill(fillColor);
				CP_Graphics_DrawRect(x, rectY, segmentFill, rectHeight);
			}

			// Yellow bar above (sun box)
			CP_Settings_Fill(CP_Color_Create(255, 255, 0, 255));
			CP_Graphics_DrawRect(x, rectY - rectHeight - 8, rectWidth, rectHeight);
		}
	}

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
	for (int i = 0; i < L1_TOOL_SIZE; ++i) {
		CP_Image_Free(level1_tools[i].tool_sprite);
	}
	CP_Image_Free(held_tool.tool_sprite);
	CP_Image_Free(pot_spritesheet);
	for (int i = 0; i < L1_POT_SIZE; ++i) {
		CP_Image_Free(level1_pots[i].plant.plant_spritesheet);
	}
	CP_Image_Free(requestbg);
	CP_Sound_Free(customerNoti);
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
		tool->initial_pos.x, tool->initial_pos.y - TOOL_RANGE_Y_OFFSET, 40.0f + TOOL_RANGE)) {

		tool->in_range = true;
		//highlights initial grid
		CP_Settings_RectMode(CP_POSITION_CORNER);
		//CP_Settings_Stroke(CP_Color_Create(225, 173, 1, 255));
		CP_Settings_NoStroke();
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, 80));
		CP_Graphics_DrawRect(tool->initial_pos.x, tool->initial_pos.y,
			CP_System_GetWindowWidth() / TILE_COLS, CP_System_GetWindowHeight() / TILE_ROWS);


		if (CP_Input_KeyReleased(KEY_Q)) {
			tool->tool_name = HAND;
			tool->position.x = tool->initial_pos.x;
			tool->position.y = tool->initial_pos.y;
		}
		CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
	}
	else tool->in_range = false;

	//--------------------- Plant Interaction ---------------------//
	for (int i = 0; i < L1_POT_SIZE; ++i) {
		if (level1_pots[i].selected && CP_Input_KeyReleased(KEY_E)) {
			switch (held_tool.tool_name) {
			case WATERING_CAN:
				if (level1_pots[i].plant.seed_type != EMPTY) { grow_plant(&level1_pots[i].plant.growth_stage); }
				break;
			case SOIL:
				level1_pots[i].filled = true;
				break;
			case SEED_1: //this is easier, :p
				if (level1_pots[i].filled) { initialize_plant(&level1_pots[i].plant, BERRY_BUSH); }
				break;
			case SEED_2:
				if (level1_pots[i].filled) { initialize_plant(&level1_pots[i].plant, CACTUS); }
				break;
			case SEED_3:
				if (level1_pots[i].filled) { initialize_plant(&level1_pots[i].plant, ORANGE_BUSH); }
				break;
			default:
				break;
			}
		}
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
				//CP_Settings_Stroke(CP_Color_Create(225, 173, 1, 255));
				CP_Settings_NoStroke();
				CP_Settings_Fill(CP_Color_Create(255, 255, 255, 80));
				CP_Graphics_DrawRect(tile->x, tile->y, 
					CP_System_GetWindowWidth() / TILE_COLS, CP_System_GetWindowHeight() / TILE_ROWS);

				//place down pot if tile is placeable and not obstructed by other things
				if (CP_Input_KeyReleased(KEY_Q) && tile->placeable && !(tile->obstructed)) {
					tile->obstructed = true;
					held_tool.tool_name = HAND;
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

	//  Shift all existing boxes left
	for (int i = 0; i < boxCount; i++) {
		boxes[i].targetX -= (boxes[i].width + 10);  // push left
	}

	//  Create new box on the right
	Box newBox;
	newBox.width = 300; // size of req box
	newBox.height = 250;
	newBox.x = screenW;        // start off-screen right
	newBox.y = 10.0f;          // top position
	newBox.targetX = screenW - (newBox.width + 10);  // rightmost visible spot
	newBox.speed = 5.0f;

	newBox.removing = 0;
	newBox.removeSpeed = 0.0f;
	newBox.lateTimer = 8.0f;
	// Add new box to array
	boxes[boxCount++] = newBox;
	CP_Sound_Play(customerNoti);
}
