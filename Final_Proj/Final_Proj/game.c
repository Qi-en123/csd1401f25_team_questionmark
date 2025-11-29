#include "cprocessing.h" // for all CP function
#include "mainmenu.h" // to go back with key Q
#include <math.h>   // calculations for speed and all  
#include "utils.h" //for intersecting function
#include "game.h"
#include <stdio.h>
#include "level2.h"
#include "player.h"
#include "tools.h"
#include "plants.h"
#include "map.h"
#include <stdlib.h>
#include <time.h>

#include "Pest.h"

extern int level;
extern int flash_timer; // timer for flashing
extern int flash_interval; // frames between flashes
extern bool show_text;
//--------------------- Player ---------------------//
Player player;
float player_speed;
float player_startpos_x, player_startpos_y;
CP_Image player_spritesheet;
//Timer
#define L1_TIMER 300.0f
static Timer timer;

//--------------------- Tools ---------------------//
#define L1_TOOL_SIZE 4
Tool level1_tools[L1_TOOL_SIZE];
/*!!!!Due to time constraint thrs a bug, DO NOT skip seed numbers : !!!!
  !!!!		e.g. do SEED_1,2,3, DO NOT DO SEED_2,3 or SEED_1,3		!!!!
  !!!!Tools must always have WATERING_CAN, SOIL & SEED_1 (Minimum)  !!!!*/
Tool_name level1_tool_names[L1_TOOL_SIZE] = { WATERING_CAN, SOIL, SEED_1, SEED_2 };
Tool* held_tool; // a pointer to the tool held
int watering_can_capacity;

//--------------------- Pots ---------------------//
#define L1_POT_SIZE 5
Pot level1_pots[L1_POT_SIZE];
Pot* held_pot; // a pointer to the pot held

Request_State l1_request;

//Fonts
CP_Font my_font;
CP_Font font;

//Ctrls
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

//Sounds
CP_Sound refill;
CP_Sound watering;
CP_Sound win;
CP_Sound lose;

void Game_Init(void)
{
	level = loadLevel();
	show_text = false;
	flash_timer = 0;
	flash_interval = 30;

	paused = 1;
	show_controls = 1;
	game_over = 0;
	score = 0;
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
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
	l1_request.request_bg = CP_Image_Load("Assets/button_square_gradient-1.png");
	l1_request.customer_noti = CP_Sound_Load("Assets/BellBrass CTE01_28.5.wav");
	l1_request.berrybush = CP_Image_Load("Assets/tool_sprites/seed_berrybush.png");
	l1_request.cactus = CP_Image_Load("Assets/tool_sprites/seed_cactus.png");
	l1_request.orangebush = CP_Image_Load("Assets/tool_sprites/seed_orangebush.png");

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
#pragma region Timer Initialization
	timer.timer_bg_blue = CP_Image_Load("Assets/timer_bg_blue.png");
	timer.timer_bg_red = CP_Image_Load("Assets/timer_bg_red.png");
	timer.width = CP_System_GetWindowWidth() / 8.0f;
	timer.height = 70.0f;
	timer.font_size = 50.0f;
	timer.pos_x = 30.0f + timer.width / 2;
	timer.pos_y = 30.0f + timer.height / 2;
	timer.font_color = CP_Color_Create(255, 255, 255, 255);
#pragma endregion


	level = loadLevel();
	paused = 1;
	show_controls = 1;
	game_over = 0;
	score = 0;
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	timer.stage_timer = L1_TIMER;

	CP_Settings_ImageMode(CP_POSITION_CORNER);


	draw_tile_map(*map_tiles);
	set_tile_placeable(*map_tiles, placeable, sizeof(placeable) / sizeof(placeable[0]));

	//Request
	srand((unsigned int)time(NULL));

	CP_System_SetFrameRate(60);
	l1_request.box_count = 0;
	l1_request.spawn_timer = 0;
	l1_request.start_delay = 10.0f;
	l1_request.boxes = boxes;
	l1_request.request_timer = 80.0f;
	l1_request.request_interval = 20.0f;

	//Initialize player with default values
	player_speed = 400.0f;
	player_startpos_x = CP_System_GetWindowWidth() / 2.0f;
	player_startpos_y = CP_System_GetWindowHeight() / 2.0f;
	initialize_player(&player, player_speed, player_startpos_x, player_startpos_y);

	//Tools
	watering_can_capacity = 3;
	initialize_tools(level1_tools, L1_TOOL_SIZE, level1_tool_names, &map_tiles[4][1], watering_can_capacity);
	held_tool = NULL;


	//Pots
	initialize_pots(level1_pots, L1_POT_SIZE, &map_tiles[5][5]);
	held_pot = NULL;

	//pest
	pest_spawn_interval = 40.0f;
	initialize_pests();

	//Colors
	blue = CP_Color_Create(0, 0, 255, 255);
	red = CP_Color_Create(220, 100, 100, 255);
	green = CP_Color_Create(100, 220, 100, 255);
	gray = CP_Color_Create(210, 210, 210, 255);
}

void Game_Update(void)
{

	// Don't allow pause if game is over
	if (!game_over && CP_Input_KeyReleased(KEY_ESCAPE)) {
		paused = !paused;

		if (paused) {
			show_controls = 0; // Reset controls overlay when opening pause menu
		}
	}
	if (CP_Input_KeyReleased(KEY_2)) {
		game_over = 1;
		score = 100000;
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
					move_pot(&held_pot->position.x, &held_pot->position.y, &player); //intialize held pot pos
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
			move_pot(&held_pot->position.x, &held_pot->position.y, &player);
			handle_pot_interaction(held_pot, &player, *map_tiles, &held_tool, &held_pot);
		}
		else {
			move_tool(&held_tool->position.x, &held_tool->position.y, &held_tool->rotation, &player);
			handle_tool_interaction(held_tool, &player, *map_tiles, level1_pots, &held_tool, L1_POT_SIZE);
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
			float panel_width = 1600;
			float panel_height = 900;
			float panel_x = (float)CP_System_GetWindowWidth() / 2;
			float panel_y = (float)CP_System_GetWindowHeight() / 2;

			if (show_controls == 1) {
				CP_Image_Draw(guide1, panel_x, panel_y, panel_width, panel_height, 255);

				// Draw right button
				float btn_x = 1550, btn_y = 420, btn_size = 75;
				if (IsCircleHovered(btn_x, btn_y, btn_size))
					CP_Image_Draw(button_square_green, btn_x, btn_y, btn_size, btn_size, 255);
				else
					CP_Image_Draw(button_square_white, btn_x, btn_y, btn_size, btn_size, 255);

				CP_Image_Draw(right, btn_x + 3, btn_y - 5, 67, 67, 255);

				// Navigate
				if (IsButtonClicked_Center(btn_x, btn_y, btn_size, btn_size)) {
					CP_Sound_Play(btn_sound);
					show_controls = 2;
				}

			}
			else { // show_controls == 2
				CP_Image_Draw(guide2, panel_x, panel_y, panel_width, panel_height, 255);

				// Draw left button
				float btn_x = 50, btn_y = 420, btn_size = 75;
				if (IsCircleHovered(btn_x, btn_y, btn_size))
					CP_Image_Draw(button_square_green, btn_x, btn_y, btn_size, btn_size, 255);
				else
					CP_Image_Draw(button_square_white, btn_x, btn_y, btn_size, btn_size, 255);

				CP_Image_Draw(left, btn_x - 3, btn_y - 5, 67, 67, 255);

				// Navigate
				if (IsButtonClicked_Center(btn_x, btn_y, btn_size, btn_size)) {
					CP_Sound_Play(btn_sound);
					show_controls = 1;
				}
			}

			// Close overlay with key 1
			if (CP_Input_KeyReleased(KEY_1))
				show_controls = 0;

		}
		else { // show_controls == 0
			// Draw normal pause menu
			int x_coord = 650; // shared x coordinate for all buttons
			float btn_width = 500.0f;
			float btn_height = 90.0f;
			float btn_x = x_coord - 100.0f;
			float btn_y[3] = { 225.0f, 425.0f, 625.0f };
			const char* btn_text[3] = { "Resume","Controls", "Quit" };

			CP_Settings_ImageMode(CP_POSITION_CORNER);

			for (int i = 0; i < 3; i++) {
				if (IsButtonHovered(btn_x, btn_y[i], btn_width, btn_height))
					CP_Image_Draw(button_green, btn_x, btn_y[i], btn_width, btn_height, 255);
				else
					CP_Image_Draw(button_white, btn_x, btn_y[i], btn_width, btn_height, 255);
			}

			for (int i = 0; i < 3; i++) {
				if (IsButtonClicked(btn_x, btn_y[i], btn_width, btn_height)) {
					CP_Sound_Play(btn_sound);
					break;
				}
			}

			if (IsButtonClicked(btn_x, btn_y[0], btn_width, btn_height)) paused = 0;
			if (IsButtonClicked(btn_x, btn_y[1], btn_width, btn_height)) show_controls = 1;
			if (IsButtonClicked(btn_x, btn_y[2], btn_width, btn_height))
				CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);

			// Draw button texts
			CP_Font_Set(font);
			CP_Settings_TextSize(64.0f);
			CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

			for (int i = 0; i < 3; i++) {
				float text_x = btn_x + btn_width / 2;
				float text_y = btn_y[i] + btn_height / 2;
				CP_Font_DrawText(btn_text[i], text_x, text_y);
			}
		}

		return; // skip the rest of the game logic
	}
	//---------------- GAME OVER -----------------------------
	if (game_over)
	{
		draw_game_over_panel(&score, Game_Init, Game_Update, Game_Exit, Level2_Init, Level2_Update, Level2_Exit, 1, &played_win, &played_lose);
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
	customer_request(&l1_request, level1_pots, *map_tiles, &score, L1_TOOL_SIZE, L1_POT_SIZE);
	simulate_pests(pest_spawn_interval, level1_pots, L1_POT_SIZE, &player, pest_spritesheet);

	//--------------------- Timer ---------------------//
	//render timer on top
	start_timer(L1_TIMER, &timer, font);

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


	// Flashing logic
	if (show_text) {
		flash_timer++;
		if (flash_timer % flash_interval < flash_interval / 2) {
			CP_Font_Set(font);
			CP_Settings_TextSize(64.0f);
			CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
			CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
			CP_Font_DrawText("Level not unlocked", CP_System_GetWindowWidth() / 2.0f, 100.0f);
		}

		//  stop flashing after some time
		if (flash_timer > flash_interval * 2.5) { // 
			show_text = false;
		}
	}

	CP_Font_DrawText(buffer_score, timer.pos_x, 130);
}

void Game_Exit(void)
{
	//--------------------- Free Fonts ---------------------//
	CP_Font_Free(my_font);
	CP_Font_Free(font);

	//--------------------- Free Sprites ---------------------//
	//spritesheets
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

	//Guides
	CP_Image_Free(guide1);
	CP_Image_Free(guide2);

	//Timer sprite
	CP_Image_Free(timer.timer_bg_blue);
	CP_Image_Free(timer.timer_bg_red);

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
	CP_Image_Free(l1_request.request_bg);
	CP_Image_Free(l1_request.berrybush);
	CP_Image_Free(l1_request.cactus);
	CP_Image_Free(l1_request.orangebush);

	CP_Image_Free(star_default);
	CP_Image_Free(star_achieved);

	//--------------------- Free Audio ---------------------//
	CP_Sound_Free(l1_request.customer_noti);
	CP_Sound_Free(btn_sound);
	CP_Sound_Free(refill);
	CP_Sound_Free(watering);
	CP_Sound_Free(lose);
	CP_Sound_Free(win);
}

void start_timer(float timer, Timer* tp, CP_Font font) { //lol sry, no time
	//countdown
	if (tp->stage_timer == 0.0f) {
		tp->stage_timer = timer;
	}
	tp->stage_timer -= dt;

	static float timer_red_alpha;

	//--------------------- Timer Code ---------------------//
	//clamp timer
	int minutes = ((int)tp->stage_timer / 60.0f) >= 0 ? ((int)tp->stage_timer / 60) : 0;
	int seconds = ((int)tp->stage_timer % 60) >= 0 ? ((int)tp->stage_timer % 60) : 0;

	//Big enough to store formatted string
	char countdown[50] = { 0 };
	//converts stage_timer into a string/char*
	sprintf_s(countdown, 50, "%d:%02d", minutes, seconds); // seconds formatted to have leading 0 if 1 digit

	//if (tp->stage_timer <= 0.0f) {
	//	draw_game_over_panel(&score, init, update, exit, next_init, next_update, next_exit, curr_lvl, playedWin, playedLose);
	//}

	//blinking effect
	if (tp->stage_timer < 30.0f) {
		if ((int)tp->stage_timer % 2) {
			timer_red_alpha += 255.0f * dt;
			tp->width += tp->width * dt * 0.09f;
			tp->height += tp->height * dt * 0.09f;
			tp->font_size += tp->font_size * dt * 0.25f;
		}
		else {
			timer_red_alpha -= 200.0f * dt;
			tp->width -= tp->width * dt * 0.09f;
			tp->height -= tp->height * dt * 0.09f;
			tp->font_size -= tp->font_size * dt * 0.25f;
		}
		timer_red_alpha = CP_Math_ClampFloat(timer_red_alpha, 0.0f, 255.0f);
		tp->font_color = ((int)(tp->stage_timer * 1.5f) % 2) ? CP_Color_Create(50, 50, 50, 255) : CP_Color_Create(255, 255, 255, 255);
		tp->width = CP_Math_ClampFloat(tp->width, CP_System_GetWindowWidth() / 8.0f * 0.9f, CP_System_GetWindowWidth() / 8.0f * 1.1f);
		tp->height = CP_Math_ClampFloat(tp->height, 70.0f * 0.9f, 70.0f * 1.1f);
	}
	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE); // center text

	//Timer background
	CP_Image_Draw(tp->timer_bg_blue, tp->pos_x, tp->pos_y, tp->width, tp->height, 255);
	CP_Image_Draw(tp->timer_bg_red, tp->pos_x, tp->pos_y, tp->width, tp->height, (int)timer_red_alpha);

	//Timer 
	CP_Font_Set(font);
	CP_Settings_TextSize(tp->font_size);
	CP_Settings_Fill(tp->font_color);
	CP_Font_DrawText(countdown, tp->pos_x, tp->pos_y);
}

//moves the tool along with the player
void move_tool(float* posx, float* posy, float* rotation, Player* player) {
	float offset_x = 0.0f;
	float offset_y = 0.0f;

	//Have to make it look like the player is holding it (right handed)
	switch (player->rotation) {
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
	*posx = player->position.x + offset_x * PLAYER_SCALE;
	*posy = player->position.y + offset_y * PLAYER_SCALE;
	*rotation = player->rotation - 180.0f;
}

//only called when player is holding tool
void handle_tool_interaction(Tool* tool, Player* player, Tile* tiles, Pot* pots, Tool** held_tool, int size) {
	for (int row = 0; row < TILE_ROWS; ++row) {
		for (int col = 0; col < TILE_COLS; ++col) {
			Tile* const tile = tiles + row * TILE_COLS + col;

			if (IsWithinRect(tile->x, tile->y,
				(float)CP_System_GetWindowWidth() / TILE_COLS, (float)CP_System_GetWindowHeight() / TILE_ROWS,
				player->position.x, player->position.y) &&
				!(tile->obstructed) && //if no pots on tile
				tile->placeable) { //if tile is placeable (set manually)

				if (CP_Input_KeyReleased(KEY_J)) {
					tile->obstructed = true;
					tool->position.x = tile->x;
					tool->position.y = tile->y;
					tool->occupied_tile = tile;
					*held_tool = NULL;
					return;
				}

				//show_tooltip("Press 'J' to Place Down");
			}

		}
	}

	// Progress bar logic
	float rect_width = 40.0f;
	float rect_height = 24.0f;
	int pot_bar_segments = 5;
	float spacing = 0.0f;
	float fill_speed = 0.4f;

	static bool is_playing;

	//--------------------- Plant Interaction ---------------------//
	for (int i = 0; i < size; ++i) {
		if (pots[i].selected) {
			switch (tool->tool_name) {
			case WATERING_CAN:
				// If pot isn't empty and plant isn't fully grown
				if (pots[i].plant.seed_type != EMPTY &&
					pots[i].plant.growth_stage != ADULT &&
					!pots[i].plant.watered_once) {

					// Only proceed if tool has uses
					if (tool->uses > 0) {
						float rectX = pots[i].position.x - rect_width * pot_bar_segments / 2.0f;
						float rect_y = pots[i].position.y - POT_HEIGHT * POT_SCALE - rect_height;

						CP_Settings_RectMode(CP_POSITION_CORNER);
						CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
						CP_Settings_StrokeWeight(3.0f);

						// HANDLE WATERING LOGIC
						if (CP_Input_KeyDown(KEY_K)) {
							// Increase fill while holding E
							pots[i].plant.fill_progress += fill_speed * dt;
							if (pots[i].plant.fill_progress > 1.0f)
								pots[i].plant.fill_progress = 1.0f;
							if (!is_playing) {
								CP_Sound_PlayAdvanced(watering, 1, 1, FALSE, CP_SOUND_GROUP_7);
								is_playing = true;
							}
						}

						// DRAW PROGRESS BAR (always draw, even if E is not held)
						float totalBarWidth = pot_bar_segments * rect_width + (pot_bar_segments - 1) * spacing;
						float filled_width = totalBarWidth * pots[i].plant.fill_progress;
						float threshold_width = pots[i].plant.water_needed * rect_width;

						for (int seg = 0; seg < pot_bar_segments; seg++) {
							float x = rectX + seg * (rect_width + spacing);

							// Background color
							if (seg == pots[i].plant.water_needed)
								CP_Settings_Fill(CP_Color_Create(255, 0, 0, 180)); // Red bar
							else
								CP_Settings_Fill(CP_Color_Create(255, 255, 255, 100));

							CP_Graphics_DrawRect(x, rect_y, rect_width, rect_height);

							// Fill segment
							float segment_start = seg * (rect_width + spacing);
							float segment_fill = filled_width - segment_start;
							if (segment_fill > 0.0f) {
								if (segment_fill > rect_width)
									segment_fill = rect_width;

								CP_Color fill_color;
								if (filled_width <= threshold_width)
									fill_color = CP_Color_Create(0, 0, 255, 180); // Blue
								else {
									float red_amount = (filled_width - threshold_width) / (totalBarWidth - threshold_width);
									if (red_amount > 1.0f) red_amount = 1.0f;
									int r = (int)(255.0f * red_amount);
									int g = 0;
									int b = (int)(255.0f * (1.0f - red_amount));
									fill_color = CP_Color_Create(r, g, b, 255);
								}

								CP_Settings_Fill(fill_color);
								CP_Graphics_DrawRect(x, rect_y, segment_fill, rect_height);
							}
						}

						// LOCK POT WHEN E IS RELEASED OR USES RUN OUT
						if (CP_Input_KeyReleased(KEY_K) || tool->uses <= 0) {
							pots[i].plant.watered_once = true;

							// Grow plant only if fill is within the target segment
							int target_segment = pots[i].plant.water_needed;
							float segment_start = target_segment * (rect_width + spacing);
							float segment_end = segment_start + rect_width;
							CP_Sound_StopGroup(CP_SOUND_GROUP_7);
							is_playing = false;
							tool->uses--;

							if (filled_width >= segment_start && filled_width <= segment_end) {
								pots[i].plant.water_amount = 1.0f;
							}
							else {
								kill_plant(&pots[i]);
							}
						}
					}
				}
				//Controls tool tip at btm right
				CP_Font_Set(my_font);
				show_tooltip("Press & Hold 'K' to Water");

				break;


			case SOIL:
				if (pots[i].plant.seed_type == EMPTY) {
					if (CP_Input_KeyReleased(KEY_K)) {
						pots[i].filled = true;
					}
					show_tooltip("Press 'K' to Dig Soil");
				}
				if (pots[i].plant.seed_type != EMPTY) {
					if (CP_Input_KeyReleased(KEY_K)) {
						kill_plant(&pots[i]);
					}
					show_tooltip("Press 'K' to Remove Plant");
				}
				break;
			case SEED_1:
				if (pots[i].filled && pots[i].plant.seed_type == EMPTY) {
					if (CP_Input_KeyReleased(KEY_K)) {
						initialize_plant(&pots[i].plant, BERRY_BUSH);
					}
					show_tooltip("Press 'K' to Plant Berry Bush");
				}
				else { show_tooltip("Pot must be empty before planting seed"); }
				break;
			case SEED_2:
				if (pots[i].filled && pots[i].plant.seed_type == EMPTY) {
					if (CP_Input_KeyReleased(KEY_K)) {
						initialize_plant(&pots[i].plant, CACTUS);
					}
					show_tooltip("Press 'K' to Plant Cactus");
				}
				else { show_tooltip("Pot must be empty before planting seed"); }
				break;
			case SEED_3:
				if (pots[i].filled && pots[i].plant.seed_type == EMPTY) {
					if (CP_Input_KeyReleased(KEY_K)) {
						initialize_plant(&pots[i].plant, ORANGE_BUSH);
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
	if (tool->tool_name == WATERING_CAN &&
		IsWithinRect((tiles + 2 * TILE_COLS + 3)->x, (tiles + 2 * TILE_COLS + 3)->y, // <-- COORDS TO BE CHANGED WITH WELL COORDS
			(float)CP_System_GetWindowWidth() / TILE_COLS, (float)CP_System_GetWindowHeight() / TILE_ROWS,
			player->position.x, player->position.y)) {

		if (CP_Input_KeyReleased(KEY_K)) {
			tool->uses = watering_can_capacity;
			tool->tool_sprite = CP_Image_Load("Assets/tool_sprites/water_bucket_full.png");
			refill = CP_Sound_Load("Assets/Refilling Bucket.wav");
			CP_Sound_PlayAdvanced(refill, 1, 1, FALSE, CP_SOUND_GROUP_8);
		}
		show_tooltip("Press 'K' to Refill Bucket");
	}
}

//moves pot along with player
void move_pot(float* posx, float* posy, Player* player) {
	float offset_x = 0.0f;
	float offset_y = 0.0f;

	//Rough offset for now
	switch (player->rotation) {
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

	*posx = player->position.x + offset_x * PLAYER_SCALE;
	*posy = player->position.y + offset_y * PLAYER_SCALE;
}

//only called when player is holding pot
void handle_pot_interaction(Pot* pot, Player* player, Tile* tiles, Tool** held_tool, Pot** held_pot) {
	for (int row = 0; row < TILE_ROWS; ++row) {
		for (int col = 0; col < TILE_COLS; ++col) {
			Tile* const tile = tiles + row * TILE_COLS + col;

			if (IsWithinRect(tile->x, tile->y,
				(float)CP_System_GetWindowWidth() / TILE_COLS, (float)CP_System_GetWindowHeight() / TILE_ROWS,
				player->position.x, player->position.y) &&
				!(tile->obstructed) && //if no pots on tile
				tile->placeable) { //if tile is placeable (set manually)

				//highlights grid that pot would be placed
				CP_Settings_RectMode(CP_POSITION_CENTER);
				CP_Settings_NoStroke();
				CP_Settings_Fill(CP_Color_Create(255, 255, 0, 80));
				CP_Graphics_DrawRect(tile->x, tile->y,
					(float)CP_System_GetWindowWidth() / TILE_COLS, (float)CP_System_GetWindowHeight() / TILE_ROWS);

				//place down pot if tile is placeable and not obstructed by other things
				if (CP_Input_KeyReleased(KEY_J)) {
					tile->obstructed = true;
					*held_tool = NULL;
					*held_pot = NULL;
					pot->position.x = tile->x;
					pot->position.y = tile->y;
					pot->occupied_tile = tile;
				}
				show_tooltip("Press 'J' to Place Down");
			}
		}
	}
	CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
}

void spawn_box(Request_State* request, int tool_size) {
	if (request->box_count >= MAX_BOXES) return;

	float screen_w = (float)CP_System_GetWindowWidth();
	float screen_h = (float)CP_System_GetWindowHeight();

	//  Shift all existing boxes left
	for (int i = 0; i < request->box_count; i++) {
		request->boxes[i].target_x -= (request->boxes[i].width + 10);  // push left
	}

	//  Create new box on the right
	Box new_box = { 0 };
	new_box.width = 120; // size of req box // old numbers: 250
	new_box.height = 120; // old numbers: 150
	new_box.x = screen_w;        // start off-screen right
	new_box.y = 10.0f;          // top position
	new_box.target_x = screen_w - (new_box.width + 10);  // rightmost visible spot
	new_box.speed = 150.0f;

	new_box.removing = 0;
	new_box.remove_speed = 0.0f;
	new_box.late_timer = request->request_timer;

	// --- Random plant request setup ---
	new_box.num_plants = CP_Random_RangeInt(0, 1); // gives 1, 2, or 3
	if (new_box.num_plants < 1) new_box.num_plants = 1; // So that it will spawn at least 1
	if (new_box.num_plants > 3) new_box.num_plants = 3;
	for (int i = 0; i < new_box.num_plants; i++) {
		new_box.plant_type = CP_Random_RangeInt(0, tool_size - 3);
		if (new_box.plant_type > 2) new_box.plant_type = 2; // safety clamp
		if (new_box.plant_type < 0) new_box.plant_type = 0;
	}

	// Add new box to array
	request->boxes[request->box_count++] = new_box;
	//CP_Sound_Play(request->customer_noti);
}

void customer_request(Request_State* request, Pot* pots, Tile* tiles, int* score, int tool_size, int pot_size) {
	//--------------------- Create Requests ---------------------//
	request->spawn_timer += dt;

	// --- START DELAY HANDLING ---
	if (request->start_delay > 0.0f) {
		request->start_delay -= dt;

		if (request->start_delay <= 0.0f) {
			// First request appears after the 10s delay
			spawn_box(request, tool_size);

			// Reset timers cleanly
			request->spawn_timer = 0.0f;
			request->start_delay = 0.0f;
		}

		return; // Do not spawn normally yet
	}

	// --- NORMAL SPAWNING (after delay finishes) ---
	if (request->spawn_timer >= request->request_interval && request->box_count < MAX_BOXES) {
		spawn_box(request, tool_size);
		request->spawn_timer = 0.0f;
	}

	for (int i = 0; i < request->box_count; i++) {
		if (!request->boxes[i].removing)
		{
			request->boxes[i].late_timer -= dt;

			if (request->boxes[i].late_timer <= 0.0f)
			{
				request->boxes[i].removing = 1;
				request->boxes[i].remove_speed = 300.0f;
			}
		}
	}

	for (int i = 0; i < request->box_count; i++) {

		if (request->boxes[i].removing)
		{
			request->boxes[i].y -= request->boxes[i].remove_speed * dt;

			// If off-screen, delete it
			if (request->boxes[i].y + request->boxes[i].height < 0)
			{
				// shift boxes down
				for (int j = 0; j < request->box_count - i; ++j) {
					request->boxes[i + j] = request->boxes[i + j + 1];
				}
				if (i != 0) {
					for (int j = 0; j < i; ++j) {
						request->boxes[j].target_x += request->boxes[j].width + 10;
					}
				}

				request->box_count--;
				i--; // loop checks next box correctly
				continue;
			}
		}

		if (request->boxes[i].x > request->boxes[i].target_x + 1.5f) {
			request->boxes[i].x -= request->boxes[i].speed * dt;
		}
		else if (request->boxes[i].x < request->boxes[i].target_x - 1.5f) {
			request->boxes[i].x += request->boxes[i].speed * dt;
		}
		else {
			request->boxes[i].x = request->boxes[i].target_x;
		}

		CP_Settings_ImageMode(CP_POSITION_CORNER);
		CP_Image_Draw(request->request_bg, request->boxes[i].x, request->boxes[i].y,
			request->boxes[i].width, request->boxes[i].height, 170);
		CP_Image plant_image = NULL;
		float img_size = 80.0f; // plant icon size
		float spacing = 10.0f; // space between icons


		int num = request->boxes[i].num_plants;
		if (num > 0) {
			//float totalWidth = num * img_size + (num - 1) * spacing;
			float start_x = request->boxes[i].x + (request->boxes[i].width - img_size) / 2.0f;
			float start_y = request->boxes[i].y + (request->boxes[i].height - img_size) / 2.0f;

			CP_Image plant_image = NULL;

			switch (request->boxes[i].plant_type) {
			case 0: plant_image = request->berrybush; break;
			case 1: plant_image = request->cactus; break;
			case 2: plant_image = request->orangebush; break;
			default: plant_image = request->berrybush; break; // fallback
			}

			CP_Image_Draw(
				plant_image,
				start_x,
				start_y,
				img_size, img_size, 250
			);
		}

		float time_left = request->boxes[i].late_timer;
		if (time_left < 0) time_left = 0;

		CP_Font_Set(font);
		CP_Settings_TextSize(50);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));

		char timer_str[8];
		snprintf(timer_str, sizeof(timer_str), "%.1f", time_left);

		CP_Font_DrawText(timer_str, request->boxes[i].x + request->boxes[i].width / 2,
			request->boxes[i].y + request->boxes[i].height + 20);
	}

	//--------------------- Fulfill Request ---------------------//
	Tile* drop_off[2] = { tiles + 2 * TILE_COLS + 5, tiles + 2 * TILE_COLS + 6 };

	const char* praises[] = { "NICE!", "AWESOME!", "GOOD JOB!" };
	int num_praises = 3;
	static char current_praise[32] = "";
	static float praise_timer = 0.0f;

	//apologies for this mess
	for (int i = 0; i < 2; ++i) {
		//if drop-off point is occupied
		if (drop_off[i]->obstructed) {

			for (int j = 0; j < pot_size; ++j) {
				//find pot that is occupying drop-off point
				if (pots[j].occupied_tile == drop_off[i] &&
					pots[j].plant.growth_stage == ADULT &&
					request->box_count > 0) {

					for (int k = 0; k < request->box_count; ++k) {
						if (pots[j].plant.seed_type == request->boxes[k].plant_type) {
							if (0 == request->boxes[k].removing) {
								*score += (int)(1000.0f * (1.0f + (request->boxes[k].late_timer / 60.0f)));

								int idx = rand() % num_praises;
								snprintf(current_praise, sizeof(current_praise), "%s", praises[idx]);
								praise_timer = 1.0f;

								request->boxes[k].removing = 1;
								request->boxes[k].remove_speed = 300.0f;

								//remove plant								
								kill_plant(&pots[j]);
								break; //exit early
							}
						}
					}
				}
			}
		}
	}

	if (praise_timer > 0.0f)
	{
		CP_Settings_TextSize(80);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Settings_Fill(CP_Color_Create(255, 255, 0, 255)); // yellow text

		float screen_w = (float)CP_System_GetWindowWidth();
		float screen_h = (float)CP_System_GetWindowHeight();
		CP_Font_DrawText(current_praise, screen_w / 2.0f, screen_h / 4.0f);

		praise_timer -= CP_System_GetDt();
	}
}

void simulate_pests(float spawn_interval, Pot* pots, int pot_size, Player* player, CP_Image pest_spritesheet) {
	//--------------------- Pests ---------------------//
	static float pestspawn_timer = 0.0f;
	static float nextSpawnTime = 0.0f;

	if (pestspawn_timer >= nextSpawnTime) {
		spawn_pest(pots, pot_size);
		pestspawn_timer = 0.0f;
		nextSpawnTime = spawn_interval + ((float)rand() / RAND_MAX) * 5.0f;
	}

	// pass player to update_pests
	update_pests(pots, pot_size, player);
	render_pests(pest_spritesheet);

	pestspawn_timer += dt;
}

void draw_game_over_panel(int* score, void* init, void* update, void* exit,
	void* next_init, void* next_update, void* next_exit, int curr_lvl, int* playedWin, int* playedLose)
{
	float w = (float)CP_System_GetWindowWidth();
	float h = (float)CP_System_GetWindowHeight();
	int stars = 0;

	if (*score > 5000) {
		stars = 1;
	}
	if (*score > 8000) {
		stars = 2;
	}
	if (*score > 12000) {
		stars = 3;
	}

	//rmb to change lvl2.c & lvl3.c parameters
	if (stars > 0) {
		if (level == 1 && curr_lvl == 1) {
			level = 2; // unlock level 2
			saveLevel(level);
		}
		else if (level >= 2) {
			saveLevel(level);
		}
		if (level == 2 && curr_lvl == 2) {
			level = 3; // unlock level 2
			saveLevel(level);
		}
		else if (level >= 3) {
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
	CP_Image_Draw(white_background, w / 2, h / 2, panelW, panelH, 255);
	CP_Font_Set(my_font);
	CP_Settings_TextSize(70);
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

	if (stars == 0) {
		// Title
		if (!(*playedLose)) {
			CP_Sound_Play(lose);
			*playedLose = 1;
		}

		CP_Font_DrawText("Level Failed", w / 2, h / 2 - 130);
		CP_Font_DrawText("Score", w / 2, h / 2 - 30);
	}
	else {
		// Title
		if (!(*playedWin)) {
			CP_Sound_Play(win);
			*playedWin = 1;
		}
		CP_Font_DrawText("Level Cleared", w / 2, h / 2 - 130);
		CP_Font_DrawText("Score", w / 2, h / 2 - 30);
	}

	char buffer[50];
	sprintf_s(buffer, 50, "%d", *score);
	CP_Settings_TextSize(100);
	CP_Font_DrawText(buffer, w / 2, h / 2 + 70);
	// Draw stars

	float star_w = 150, star_h = 150;
	float spacing_x = 100;
	float spacing_y = 30; // middle star higher
	float center_x = w / 2;
	float base_y = 200.0f;

	for (int i = 1; i <= 3; i++) // start from 1, go to 3
	{
		float x = center_x + (i - 2) * spacing_x; // -1,0,1 for left, center, right
		float y = base_y;
		if (i == 2) y -= spacing_y; // middle star higher
		// Set size: middle star bigger, side stars smaller
		float star_w = (i == 2) ? 150.0f : 100.0f;
		float star_h = (i == 2) ? 150.0f : 100.0f;
		// Set rotation for inverted U effect
		float rotation = 0.0f;
		if (i == 1) rotation = -15.0f; // tilt left star slightly counter-clockwise
		if (i == 3) rotation = 15.0f;  // tilt right star slightly clockwise

		CP_Image img = (i <= stars) ? star_achieved : star_default;
		CP_Image_DrawAdvanced(img, x, y, star_w, star_h, 255, rotation);
	}

	float btn_width = 150.0f;
	float btn_height = 150.0f;
	float btn_x[3] = { w / 2 - 250,w / 2, w / 2 + 250 }; // spaced around center
	float btn_y = h / 2 + 225; // 


	// Draw buttons
	for (int i = 0; i < 3; i++) {
		if (IsCircleHovered(btn_x[i], btn_y, btn_width))
			CP_Image_Draw(button_circle_green, btn_x[i], btn_y, btn_width, btn_height, 255);
		else
			CP_Image_Draw(button_circle_white, btn_x[i], btn_y, btn_width, btn_height, 255);

		if (i == 0) {
			CP_Image_Draw(home, btn_x[i], btn_y, btn_width - 75.0f, btn_height - 75.0f, 255);
		}
		if (i == 1) {
			CP_Image_Draw(repeat, btn_x[i], btn_y, btn_width - 75.0f, btn_height - 75.0f, 255);
		}
		if (i == 2) {
			CP_Image_Draw(next, btn_x[i], btn_y, btn_width - 75.0f, btn_height - 75.0f, 255);
		}

	}

	// Play click sound for any button
	for (int i = 0; i < 2; i++) {
		if (IsButtonClicked(btn_x[i], btn_y, btn_width - 75.0f, btn_height - 75.0f)) {
			CP_Sound_Play(btn_sound);
			break;
		}
	}

	// Button actions
	if (IsCircleClicked(btn_x[0], btn_y, 150, CP_Input_GetMouseX(), CP_Input_GetMouseY())) {
		*playedLose = 0;
		*playedWin = 0;
		CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
	}

	//restart
	if (IsCircleClicked(btn_x[1], btn_y, 150, CP_Input_GetMouseX(), CP_Input_GetMouseY())) {
		*playedLose = 0;
		*playedWin = 0;
		CP_Engine_SetNextGameStateForced(init, update, exit);
	}

	//nxt level
	if (IsCircleClicked(btn_x[2], btn_y, 150, CP_Input_GetMouseX(), CP_Input_GetMouseY()))
	{
		if (level < curr_lvl + 1) {
			show_text = true;
			flash_timer = 0;
		}
		else {
			CP_Engine_SetNextGameState(next_init, next_update, next_exit);
		}
	}

	if (CP_Input_KeyTriggered(KEY_ESCAPE)) {
		CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
	}
}
