#include "level2.h"
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
#include "pest.h"
#include "level3.h"

#define L2_TIMER 300.0f
static Timer timer;

//--------------------- Tools ---------------------//
#define L2_TOOL_SIZE 5
Tool level2_tools[L2_TOOL_SIZE];
/*!!!!Due to time constraint thrs a bug, DO NOT skip seed numbers : !!!!
  !!!!		e.g. do SEED_1,2,3, DO NOT DO SEED_2,3 or SEED_1,3		!!!!
  !!!!Tools must always have WATERING_CAN, SOIL & SEED_1 (Minimum)  !!!!*/
Tool_name level2_tool_names[L2_TOOL_SIZE] = { WATERING_CAN, SOIL, SEED_1, SEED_2, SEED_3 };
Tool* held_tool; // a pointer to the tool held
int watering_can_capacity;

//--------------------- Pots ---------------------//
#define L2_POT_SIZE 8
Pot level2_pots[L2_POT_SIZE];
Pot* held_pot; // a pointer to the pot held

Request_State l2_request;

CP_Image white_background;
CP_Image repeat;
CP_Image next;
CP_Image home;
CP_Image star_default;
CP_Image star_achieved;
CP_Image button_circle_white;
CP_Image button_circle_green;

//Frequently used colors
CP_Color red, green, blue, gray;

CP_Font my_font;
CP_Font font;

CP_Sound refill;
CP_Sound watering;
CP_Sound win;
CP_Sound lose;

void Level2_Init(void) {
	font = CP_Font_Load("Assets/Kenney Pixel Square.ttf");
	my_font = CP_Font_Load("Assets/Kenney Future Narrow.ttf");
	star_default = CP_Image_Load("Assets/star_outline.png");
	star_achieved = CP_Image_Load("Assets/star.png");
	guide1 = CP_Image_Load("Assets/guide1.png");
	guide2 = CP_Image_Load("Assets/guide2.png");
	left = CP_Image_Load("Assets/left.png");
	right = CP_Image_Load("Assets/right.png");
	spritesheet_tiles = CP_Image_Load("Assets/tilemap_packed.png");

	//Request
	l2_request.request_bg = CP_Image_Load("Assets/button_square_gradient-1.png");
	l2_request.customer_noti = CP_Sound_Load("Assets/BellBrass CTE01_28.5.wav");
	l2_request.berrybush = CP_Image_Load("Assets/tool_sprites/seed_berrybush.png");
	l2_request.cactus = CP_Image_Load("Assets/tool_sprites/seed_cactus.png");
	l2_request.orangebush = CP_Image_Load("Assets/tool_sprites/seed_orangebush.png");

	//player
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
	btn_sound = CP_Sound_Load("Assets/ADDING-MACHINE-ELECTRONIC_GEN-HDF-00038.wav");

	watering = CP_Sound_Load("Assets/Watering Soil.wav");
	lose = CP_Sound_Load("Assets/You Lost.wav");
	win = CP_Sound_Load("Assets/You won.wav");

	//Pots
	pot_spritesheet = CP_Image_Load("Assets/tool_sprites/pot_spritesheet.png");

	//Pests
	pest_spritesheet = CP_Image_Load("Assets/rat_spritesheet.png");

	//Timer
	timer.timer_bg_blue = CP_Image_Load("Assets/timer_bg_blue.png");
	timer.timer_bg_red = CP_Image_Load("Assets/timer_bg_red.png");

	timer.width = CP_System_GetWindowWidth() / 8.0f;
	timer.height = 70.0f;
	timer.font_size = 50.0f;
	timer.pos_x = 30.0f + timer.width / 2;
	timer.pos_y = 30.0f + timer.height / 2;
	timer.font_color = CP_Color_Create(255, 255, 255, 255);

	level = loadLevel();
	paused = 0;
	show_controls = 1;
	game_over = 0;
	score = 0;
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	timer.stage_timer = L2_TIMER;

	CP_Settings_ImageMode(CP_POSITION_CORNER);

	draw_tile_map(*map_tiles);
	set_tile_placeable(*map_tiles, placeable, sizeof(placeable) / sizeof(placeable[0]));

	//Request
	srand((unsigned int)time(NULL));

	CP_System_SetFrameRate(60);
	l2_request.box_count = 0;
	l2_request.spawn_timer = 0;
	l2_request.start_delay = 10.0f;
	l2_request.boxes = boxes;
	l2_request.request_timer = 80.0f;
	l2_request.request_interval = 20.0f;

	//Initialize player with default values
	player_speed = 400.0f;
	player_startpos_x = (float)CP_System_GetWindowWidth() / 2;
	player_startpos_y = (float)CP_System_GetWindowHeight() / 2;
	initialize_player(&player, player_speed, player_startpos_x, player_startpos_y);

	//Tools
	watering_can_capacity = 5;
	initialize_tools(level2_tools, L2_TOOL_SIZE, level2_tool_names, &map_tiles[4][1], watering_can_capacity);
	held_tool = NULL;


	//Pots
	initialize_pots(level2_pots, L2_POT_SIZE, &map_tiles[5][5]);
	held_pot = NULL;

	//pest
	pest_spawn_interval = 30.0f;
	initialize_pests();

	//Colors
	blue = CP_Color_Create(0, 0, 255, 255);
	red = CP_Color_Create(220, 100, 100, 255);
	green = CP_Color_Create(100, 220, 100, 255);
	gray = CP_Color_Create(210, 210, 210, 255);
}

void Level2_Update(void) {
	// Don't allow pause if game is over
	if (!game_over && CP_Input_KeyReleased(KEY_ESCAPE)) {
		paused = !paused;

		if (paused) {
			show_controls = 0; // Reset controls overlay when opening pause menu
		}
	}
	if (CP_Input_KeyReleased(KEY_2)) {
		game_over = 1;
		score = 1000000;

	}

	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_NEAREST);

	draw_tile_map(*map_tiles);
	//set_tile_placeable(map_tiles, &placeable, sizeof(placeable) / sizeof(placeable[0]));

	draw_modified_map(*map_tiles);


	//Player shadow
	CP_Settings_EllipseMode(CP_POSITION_CENTER);
	CP_Settings_NoStroke();
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 120));

	float shadow_x = player.position.x;
	float shadow_y = player.position.y + (PLAYER_HEIGHT - 2.0f) * PLAYER_SCALE / 2.0f;
	CP_Graphics_DrawEllipse(shadow_x, shadow_y, PLAYER_WIDTH * PLAYER_SCALE * 0.65f, 20.0f);

	//--------------------- Pots ---------------------//
	render_pots(level2_pots, pot_spritesheet, L2_POT_SIZE, player.position.x, player.position.y, held_pot, held_tool);

	//--------------------- Tools ---------------------//
	render_tools(level2_tools, L2_TOOL_SIZE, player.position.x, player.position.y, held_tool, held_pot);
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
			for (int i = 0; i < L2_TOOL_SIZE; ++i) {
				if (level2_tools[i].in_range) {
					level2_tools[i].occupied_tile->obstructed = false;
					held_tool = &level2_tools[i];
					break;
				}
			}
			//Picks up first pot that is in range
			for (int i = 0; i < L2_POT_SIZE; ++i) {
				if (level2_pots[i].selected) {
					held_pot = &level2_pots[i];
					move_pot(&held_pot->position.x, &held_pot->position.y, &player); //intialize held pot pos
					//"unobstructs" grid, makes it placeable
					level2_pots[i].occupied_tile->obstructed = false;
					break;
				}
			}
		}
	}
	//If player is holding something
	else {
		if (held_pot) {
			move_pot(&held_pot->position.x, &held_pot->position.y, &player);
			handle_pot_interaction(held_pot, &player, *map_tiles, &held_tool, &held_pot);
		}
		else {
			move_tool(&held_tool->position.x, &held_tool->position.y, &held_tool->rotation, &player);
			handle_tool_interaction(held_tool, &player, *map_tiles, level2_pots, &held_tool, L2_POT_SIZE);
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
		CP_Graphics_DrawRect((float)CP_System_GetWindowWidth() / 2,
			(float)CP_System_GetWindowHeight() / 2,
			(float)CP_System_GetWindowWidth(),
			(float)CP_System_GetWindowHeight());

		if (show_controls == 1 || show_controls == 2) {
			// Draw the controls overlay
			CP_Settings_ImageMode(CP_POSITION_CENTER);
			float panelWidth = 1600;
			float panelHeight = 900;
			float panelX = (float)CP_System_GetWindowWidth() / 2;
			float panelY = (float)CP_System_GetWindowHeight() / 2;

			if (show_controls == 1) {
				CP_Image_Draw(guide1, panelX, panelY, panelWidth, panelHeight, 255);

				// Draw right button
				float btnX = 1550, btnY = 420, btnSize = 75;
				if (IsCircleHovered(btnX, btnY, btnSize))
					CP_Image_Draw(button_square_green, btnX, btnY, btnSize, btnSize, 255);
				else
					CP_Image_Draw(button_square_white, btnX, btnY, btnSize, btnSize, 255);

				CP_Image_Draw(right, btnX + 3, btnY - 5, 67, 67, 255);

				// Navigate
				if (IsButtonClicked_Center(btnX, btnY, btnSize, btnSize))
					show_controls = 2;

			}
			else { // show_controls == 2
				CP_Image_Draw(guide2, panelX, panelY, panelWidth, panelHeight, 255);

				// Draw left button
				float btnX = 50, btnY = 420, btnSize = 75;
				if (IsCircleHovered(btnX, btnY, btnSize))
					CP_Image_Draw(button_square_green, btnX, btnY, btnSize, btnSize, 255);
				else
					CP_Image_Draw(button_square_white, btnX, btnY, btnSize, btnSize, 255);

				CP_Image_Draw(left, btnX - 3, btnY - 5, 67, 67, 255);

				// Navigate
				if (IsButtonClicked_Center(btnX, btnY, btnSize, btnSize))
					show_controls = 1;
			}

			// Close overlay with key 1
			if (CP_Input_KeyReleased(KEY_1))
				show_controls = 0;

		}
		else { // show_controls == 0
			// Draw normal pause menu
			int x_coord = 650; // shared x coordinate for all buttons
			float btnWidth = 500.0f;
			float btnHeight = 90.0f;
			float btnX = x_coord - 100.0f;
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
					CP_Sound_Play(btn_sound);
					break;
				}
			}

			if (IsButtonClicked(btnX, btnY[0], btnWidth, btnHeight)) paused = 0;
			if (IsButtonClicked(btnX, btnY[1], btnWidth, btnHeight)) show_controls = 1;
			if (IsButtonClicked(btnX, btnY[2], btnWidth, btnHeight))
				CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);

			// Draw button texts
			CP_Font_Set(font);
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
	if (game_over)
	{
		draw_game_over_panel(&score, Level2_Init, Level2_Update, Level2_Exit, Level3_Init, Level3_Update, Level3_Exit, 2, &played_win, &played_lose);
		return;   // Stop all other game updates
	}

	//--------------------- Player ---------------------//
	handle_player_input(&player);
	detect_collision_player(&player.position.x, &player.position.y, *map_tiles, player.speed);

	//Render tool infront if player facing Right or Down
	if (0 == player.rotation || 270 == player.rotation) {
		if (held_tool) { render_held_tool(held_tool); }
		if (held_pot) { render_held_pot(held_pot, pot_spritesheet); }
	}

	/*-----------------------------------------------------------------------*/
	customer_request(&l2_request, level2_pots, *map_tiles, &score, L2_TOOL_SIZE, L2_POT_SIZE);
	simulate_pests(pest_spawn_interval, level2_pots, L2_POT_SIZE, &player, pest_spritesheet);

	//--------------------- Timer ---------------------//
	//render timer on top
	start_timer(L2_TIMER, &timer, font);

	if (timer.stage_timer <= 0 && !game_over)
	{
		game_over = 1;
	}

	//--------------------- Score ---------------------//
	char buffer_score[50];
	sprintf_s(buffer_score, 50, "%d", score);
	CP_Image_Draw(button_score, timer.pos_x, 135, 200, 60, 255);
	CP_Font_Set(my_font);
	CP_Settings_TextSize(40);
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText(buffer_score, timer.pos_x, 130);


	//--------------------- Show Level Changes ---------------------//
	CP_Settings_Fill(red);
	CP_Settings_TextSize(50.0f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

	if (timer.stage_timer >= L2_TIMER - 6.0f) {
		if (0 == (int)(timer.stage_timer * 2) % 2) {
			//Watering can
			CP_Font_DrawText("Watering can capacity increased to 5!",
				(float)CP_System_GetWindowWidth() / 2, (float)CP_System_GetWindowHeight() / 2 - 50.0f);

			//Pests
			CP_Font_DrawText("Pest spawn at higher frequencies!",
				(float)CP_System_GetWindowWidth() / 2, (float)CP_System_GetWindowHeight() / 2 + 50.0f);
		}
	}
}

void Level2_Exit(void) {
	//--------------------- Free Fonts ---------------------//
	CP_Font_Free(my_font);
	CP_Font_Free(font);

	//--------------------- Free Sprites ---------------------//
	//Spritesheets
	CP_Image_Free(spritesheet_tiles);
	CP_Image_Free(player_spritesheet);
	CP_Image_Free(pest_spritesheet);

	//Button Sprites
	CP_Image_Free(button_white);
	CP_Image_Free(button_green);
	CP_Image_Free(button_score);
	CP_Image_Free(button_square_white);
	CP_Image_Free(button_square_green);
	CP_Image_Free(button_circle_white);
	CP_Image_Free(button_circle_green);
	CP_Image_Free(white_background);
	CP_Image_Free(repeat);
	CP_Image_Free(next);
	CP_Image_Free(home);
	CP_Image_Free(left);
	CP_Image_Free(right);

	CP_Sound_Free(refill);
	CP_Sound_Free(watering);
	CP_Sound_Free(lose);
	CP_Sound_Free(win);

	//Guides
	CP_Image_Free(guide1);
	CP_Image_Free(guide2);

	//Timer sprite
	CP_Image_Free(timer.timer_bg_blue);
	CP_Image_Free(timer.timer_bg_red);

	//Tool sprites
	for (int i = 0; i < L2_TOOL_SIZE; ++i) {
		CP_Image_Free(level2_tools[i].tool_sprite);
	}
	if (held_tool) { CP_Image_Free(held_tool->tool_sprite); }

	//Pot spritesheet
	CP_Image_Free(pot_spritesheet);
	
	//Plant sprites
	for (int i = 0; i < L2_POT_SIZE; ++i) {
		CP_Image_Free(level2_pots[i].plant.plant_spritesheet);
	}

	//Request sprites
	CP_Image_Free(l2_request.request_bg);
	CP_Image_Free(l2_request.berrybush);
	CP_Image_Free(l2_request.cactus);
	CP_Image_Free(l2_request.orangebush);

	CP_Image_Free(star_default);
	CP_Image_Free(star_achieved);

	//--------------------- Free Audio ---------------------//
	CP_Sound_Free(l2_request.customer_noti);
	CP_Sound_Free(btn_sound);
}
