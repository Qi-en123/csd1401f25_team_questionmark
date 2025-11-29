#include "cprocessing.h"
#include "utils.h"
#include "game.h"
#include "levels.h"
#include "credits.h"

//image
CP_Image spriteSheet;
CP_Image mainmenu_bg;
CP_Image button_white;
CP_Image white_background;
CP_Image button_green;
CP_Image guide_1;
CP_Image guide_2;
CP_Image left;
CP_Image right;
//font 
CP_Font font;
CP_Font my_font;
CP_Sound btn_sound;
//sound
extern CP_Sound opening_sound;
CP_Sound mainmenu_sound;
int quitClicked = 0;       // flag if Quit was clicked
float quit_start_time = 0;   // time when quit button was clicked

// Number of clouds
#define NUM_CLOUDS 3

// x coordinates of clouds
float cloud_x[NUM_CLOUDS] = { -350, -700, -1050 };

// speed of clouds
// diff speed so sometimes clouds can overlap and does not move in a pattern of 3
float cloud_speed[NUM_CLOUDS] = { 2.2f, 1.6f, 1.4f };

// x coordinate of bg, If bg_x were an int, adding 0.5 each frame would round down to 0, and the background would not move smoothly.
float bg_x = 0;
// speed of bg
float bg_speed = 0.5f;

// Tileable background width (in pixels)
// use tileable if not when the bg is moving, the two bg doesnt align perfectly
// leaving a thin black line in between.
int bg_width;

int show_controls;
int level = 1;

void Main_Menu_Init(void)
{
	opening_sound = CP_Sound_LoadMusic("Assets/opening.wav");
	mainmenu_sound = CP_Sound_LoadMusic("Assets/mainmenu.wav");
	level = loadLevel();
	//load mainmenu background and spritesheet for cloud from asset folder
	mainmenu_bg = CP_Image_Load("Assets/backgroundColorGrass.png");
	spriteSheet = CP_Image_Load("Assets/spritesheet_default.png");
	button_white = CP_Image_Load("Assets/button_rectangle_line.png");
	white_background = CP_Image_Load("Assets/button_rectangle_line-1.png");
	button_green = CP_Image_Load("Assets/button_rectangle_gloss.png");
	guide_1 = CP_Image_Load("Assets/guide1.png");
	guide_2 = CP_Image_Load("Assets/guide2.png");
	//load font 
	font = CP_Font_Load("Assets/Kenney Pixel Square.ttf");
	my_font = CP_Font_Load("Assets/Kenney Future Narrow.ttf");
	//themeSound = CP_Sound_LoadMusic("Assets/mainmenu.wav");
	btn_sound = CP_Sound_Load("Assets/ADDING-MACHINE-ELECTRONIC_GEN-HDF-00038.wav");
	// x coordinate of background initate to 0 whenever we reopen mainmenu
	bg_x = 0;
	// Get background width
	bg_width = CP_Image_GetWidth(mainmenu_bg);

	//CP_Sound_PlayMusic(themeSound);
	show_controls = 0;
	left = CP_Image_Load("Assets/left.png");
	right = CP_Image_Load("Assets/right.png");
	CP_Sound_PlayMusic(mainmenu_sound);
}

void Main_Menu_Update(void)
{
	float screen_w = (float)CP_System_GetWindowWidth();
	float screen_h = (float)CP_System_GetWindowHeight();

	// ---------------- Background scrolling ----------------
	bg_x += bg_speed;
	if (bg_x >= bg_width) bg_x -= bg_width;

	int bg_drawX1 = -(int)bg_x + bg_width / 2;
	int bg_drawX2 = bg_drawX1 + bg_width;
	int bg_drawX3 = bg_drawX2 + bg_width;

	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Image_Draw(mainmenu_bg, (float)bg_drawX1, screen_h / 2.0f, (float)bg_width, screen_h, 255);
	CP_Image_Draw(mainmenu_bg, (float)bg_drawX2, screen_h / 2.0f, (float)bg_width, screen_h, 255);
	CP_Image_Draw(mainmenu_bg, (float)bg_drawX3, screen_h / 2.0f, (float)bg_width, screen_h, 255);

	// ---------------- Clouds ----------------
	for (int i = 0; i < NUM_CLOUDS; i++) {
		cloud_x[i] += cloud_speed[i];
		if (cloud_x[i] >= screen_w + 175) cloud_x[i] = -175;
		CP_Image_DrawSubImage(spriteSheet, cloud_x[i], screen_h / 6,
			300, 250,
			0, 218, 251, 363,
			255);
	}

	// ---------------- Controls overlay ----------------
	if (show_controls == 1 || show_controls == 2)
	{
		// dark overlay
		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 180));
		CP_Graphics_DrawRect(screen_w / 2, screen_h / 2, screen_w, screen_h);

		// panel
		CP_Settings_ImageMode(CP_POSITION_CENTER);
		float panel_width = 1600;
		float panel_height = 900;
		float panel_x = screen_w / 2;
		float panel_y = screen_h / 2;

		// Draw different guides for each panel
		if (show_controls == 1)
			CP_Image_Draw(guide_1, panel_x, panel_y, panel_width, panel_height, 255);
		else
			CP_Image_Draw(guide_2, panel_x, panel_y, panel_width, panel_height, 255);


		// Right button (panel 1)
		if (show_controls == 1)
		{
			float btn_rx = 1550, btn_ry = 420, btn_rsize = 75;

			// Hover effect
			if (IsButtonHovered_Center(btn_rx, btn_ry, btn_rsize, btn_rsize))
				CP_Image_Draw(button_green, btn_rx, btn_ry, btn_rsize, btn_rsize, 255);
			else
				CP_Image_Draw(button_white, btn_rx, btn_ry, btn_rsize, btn_rsize, 255);

			CP_Image_Draw(right, btn_rx, btn_ry, 67, 67, 255);

			if (IsButtonClicked_Center(btn_rx, btn_ry, btn_rsize, btn_rsize))
				show_controls = 2;

			// Close overlay with key 1
			if (CP_Input_KeyReleased(KEY_1))
				show_controls = 0;

			if (IsButtonClicked_Center(btn_rx, btn_ry, btn_rsize, btn_rsize)) {
				CP_Sound_Play(btn_sound);
				show_controls = 2;
			}
		}




		// Left button (panel 2)
		if (show_controls == 2)
		{
			// Close overlay with key 1
			if (CP_Input_KeyReleased(KEY_1))
				show_controls = 0;

			float btn_lx = 50, btn_ly = 420, btn_lsize = 75;

			// Hover effect
			if (IsButtonHovered_Center(btn_lx, btn_ly, btn_lsize, btn_lsize))
				CP_Image_Draw(button_green, btn_lx, btn_ly, btn_lsize, btn_lsize, 255);
			else
				CP_Image_Draw(button_white, btn_lx, btn_ly, btn_lsize, btn_lsize, 255);

			CP_Image_Draw(left, btn_lx, btn_ly, 67, 67, 255);



			if (IsButtonClicked_Center(btn_lx, btn_ly, btn_lsize, btn_lsize)) {
				CP_Sound_Play(btn_sound);
				show_controls = 1;
			}
		}

		return; // skip main menu buttons while overlay is active
	}
	// normal menu without controls


	int x_coord = 650;
	float btn_width = 500.0f;
	float btn_height = 90.0f;
	float btn_x = x_coord - 100.0f;
	float btn_y[5] = { 225, 325, 425, 525, 625 };
	const char* btn_text[5] = { "Start", "Levels", "Controls", "Credits", "Quit" };

	CP_Settings_ImageMode(CP_POSITION_CORNER);

	// Draw buttons
	for (int i = 0; i < 5; i++) {
		if (IsButtonHovered(btn_x, btn_y[i], btn_width, btn_height))
			CP_Image_Draw(button_green, btn_x, btn_y[i], btn_width, btn_height, 255);
		else
			CP_Image_Draw(button_white, btn_x, btn_y[i], btn_width, btn_height, 255);
	}

	// Play click sound for any button
	for (int i = 0; i < 5; i++) {
		if (IsButtonClicked(btn_x, btn_y[i], btn_width, btn_height)) {
			CP_Sound_Play(btn_sound);
			break;
		}
	}

	// Button actions
	if (IsButtonClicked(btn_x, btn_y[0], btn_width, btn_height))
		CP_Engine_SetNextGameState(Game_Init, Game_Update, Game_Exit);

	if (IsButtonClicked(btn_x, btn_y[1], btn_width, btn_height))
		CP_Engine_SetNextGameState(Level_Init, Level_Update, Level_Exit);

	if (IsButtonClicked(btn_x, btn_y[2], btn_width, btn_height)) {
		show_controls = 1;
		return;
	}

	if (IsButtonClicked(btn_x, btn_y[3], btn_width, btn_height))
		CP_Engine_SetNextGameState(Credits_Init, Credits_Update, Credits_Exit);

	if (!quitClicked && IsButtonClicked(btn_x, btn_y[4], btn_width, btn_height)) {
		quitClicked = 1;
		quit_start_time = CP_System_GetSeconds();
	}

	if (quitClicked && CP_System_GetSeconds() - quit_start_time >= 0.25f)
		CP_Engine_Terminate();

	// Draw button labels
	CP_Font_Set(font);
	CP_Settings_TextSize(80);
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

	for (int i = 0; i < 5; i++) {
		float text_x = btn_x + btn_width / 2;
		float text_y = btn_y[i] + btn_height / 2;
		CP_Font_DrawText(btn_text[i], text_x, text_y);
	}

	if (CP_Input_KeyTriggered(KEY_ESCAPE))
		CP_Engine_Terminate();
}


void Main_Menu_Exit(void)
{
	//free image and font
	CP_Image_Free(spriteSheet);
	CP_Image_Free(mainmenu_bg);
	CP_Font_Free(font);
	CP_Font_Free(my_font);
	CP_Sound_Free(opening_sound);
	CP_Sound_Free(mainmenu_sound);
	CP_Image_Free(button_white);
	CP_Image_Free(white_background);
	CP_Image_Free(button_green);
	CP_Sound_Free(btn_sound);
	CP_Image_Free(guide_1);
	CP_Image_Free(guide_2);
	CP_Image_Free(left);
	CP_Image_Free(right);
}