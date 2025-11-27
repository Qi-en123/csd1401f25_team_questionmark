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
CP_Image guide1;
CP_Image guide2;
CP_Image left;
CP_Image right;
//font 
CP_Font font;
CP_Font myFont;
CP_Sound BtnSound;
//sound
extern CP_Sound openingSound;
CP_Sound mainmenuSound;
int quitClicked = 0;       // flag if Quit was clicked
float quitStartTime = 0;   // time when quit button was clicked

// Number of clouds
#define NUM_CLOUDS 3

// x coordinates of clouds
float cloudX[NUM_CLOUDS] = { -350, -700, -1050 };

// speed of clouds
// diff speed so sometimes clouds can overlap and does not move in a pattern of 3
float cloudSpeed[NUM_CLOUDS] = { 2.2f, 1.6f, 1.4f };

// x coordinate of bg, If bgX were an int, adding 0.5 each frame would round down to 0, and the background would not move smoothly.
float bgX = 0;
// speed of bg
float bgSpeed = 0.5f;

// Tileable background width (in pixels)
// use tileable if not when the bg is moving, the two bg doesnt align perfectly
// leaving a thin black line in between.
int bgWidth;

int showControls;
int level = 1;

void Main_Menu_Init(void)
{
	openingSound = CP_Sound_LoadMusic("Assets/opening.wav");
	mainmenuSound = CP_Sound_LoadMusic("Assets/mainmenu.wav");
	level = loadLevel();
	//load mainmenu background and spritesheet for cloud from asset folder
	mainmenu_bg = CP_Image_Load("Assets/backgroundColorGrass.png");
	spriteSheet = CP_Image_Load("Assets/spritesheet_default.png");
	button_white = CP_Image_Load("Assets/button_rectangle_line.png");
	white_background = CP_Image_Load("Assets/button_rectangle_line-1.png");
	button_green = CP_Image_Load("Assets/button_rectangle_gloss.png");
	guide1 = CP_Image_Load("Assets/guide1.png");
	guide2 = CP_Image_Load("Assets/guide2.png");
	//load font 
	font = CP_Font_Load("Assets/Kenney Pixel Square.ttf");
	myFont = CP_Font_Load("Assets/Kenney Future Narrow.ttf");
	//themeSound = CP_Sound_LoadMusic("Assets/mainmenu.wav");
	BtnSound = CP_Sound_Load("Assets/ADDING-MACHINE-ELECTRONIC_GEN-HDF-00038.wav");
	// x coordinate of background initate to 0 whenever we reopen mainmenu
	bgX = 0;
	// Get background width
	bgWidth = CP_Image_GetWidth(mainmenu_bg);

	//CP_Sound_PlayMusic(themeSound);
	showControls = 0;
	left = CP_Image_Load("Assets/left.png");
	right = CP_Image_Load("Assets/right.png");
	CP_Sound_PlayMusic(mainmenuSound);
}

void Main_Menu_Update(void)
{
	float screenW = (float)CP_System_GetWindowWidth();
	float screenH = (float)CP_System_GetWindowHeight();

	// ---------------- Background scrolling ----------------
	bgX += bgSpeed;
	if (bgX >= bgWidth) bgX -= bgWidth;

	int bgDrawX1 = -(int)bgX + bgWidth / 2;
	int bgDrawX2 = bgDrawX1 + bgWidth;
	int bgDrawX3 = bgDrawX2 + bgWidth;

	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Image_Draw(mainmenu_bg, (float)bgDrawX1, screenH / 2.0f, (float)bgWidth, screenH, 255);
	CP_Image_Draw(mainmenu_bg, (float)bgDrawX2, screenH / 2.0f, (float)bgWidth, screenH, 255);
	CP_Image_Draw(mainmenu_bg, (float)bgDrawX3, screenH / 2.0f, (float)bgWidth, screenH, 255);
																
	// ---------------- Clouds ----------------
	for (int i = 0; i < NUM_CLOUDS; i++) {
		cloudX[i] += cloudSpeed[i];
		if (cloudX[i] >= screenW + 175) cloudX[i] = -175;
		CP_Image_DrawSubImage(spriteSheet, cloudX[i], screenH / 6,
			300, 250,
			0, 218, 251, 363,
			255);
	}

	// ---------------- Controls overlay ----------------
	if (showControls == 1 || showControls == 2)
	{
		// dark overlay
		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 180));
		CP_Graphics_DrawRect(screenW / 2, screenH / 2, screenW, screenH);

		// panel
		CP_Settings_ImageMode(CP_POSITION_CENTER);
		float panelWidth = 1600;
		float panelHeight = 900;
		float panelX = screenW / 2;
		float panelY = screenH / 2;

		// Draw different guides for each panel
		if (showControls == 1)
			CP_Image_Draw(guide1, panelX, panelY, panelWidth, panelHeight, 255);
		else
			CP_Image_Draw(guide2, panelX, panelY, panelWidth, panelHeight, 255);

	
		// Right button (panel 1)
		if (showControls == 1)
		{
			float btnRX = 1550, btnRY = 420, btnRSize = 75;

			// Hover effect
			if (IsButtonHovered_Center(btnRX, btnRY, btnRSize, btnRSize))
				CP_Image_Draw(button_green, btnRX, btnRY, btnRSize, btnRSize, 255);
			else
				CP_Image_Draw(button_white, btnRX, btnRY, btnRSize, btnRSize, 255);

			CP_Image_Draw(right, btnRX, btnRY, 67, 67, 255);

			if (IsButtonClicked_Center(btnRX, btnRY, btnRSize, btnRSize))
				showControls = 2;

			// Close overlay with key 1
			if (CP_Input_KeyReleased(KEY_1))
				showControls = 0;

			if (IsButtonClicked_Center(btnRX, btnRY, btnRSize, btnRSize)) {
				CP_Sound_Play(BtnSound);
				showControls = 2;
			}
		}
		

              
            
		// Left button (panel 2)
		if (showControls == 2)
		{
			float btnLX = 50, btnLY = 420, btnLSize = 75;

			// Hover effect
			if (IsButtonHovered_Center(btnLX, btnLY, btnLSize, btnLSize))
				CP_Image_Draw(button_green, btnLX, btnLY, btnLSize, btnLSize, 255);
			else
				CP_Image_Draw(button_white, btnLX, btnLY, btnLSize, btnLSize, 255);

			CP_Image_Draw(left, btnLX, btnLY, 67, 67, 255);

		

			if (IsButtonClicked_Center(btnLX, btnLY, btnLSize, btnLSize)) {
				CP_Sound_Play(BtnSound);
				showControls = 1;
			}
		}

		return; // skip main menu buttons while overlay is active
	}
	// normal menu without controls


	int x_coord = 650;
	float btnWidth = 500.0f;
	float btnHeight = 90.0f;
	float btnX = x_coord - 100.0f;
	float btnY[5] = { 225, 325, 425, 525, 625 };
	const char* btnText[5] = { "Start", "Levels", "Controls", "Credits", "Quit" };

	CP_Settings_ImageMode(CP_POSITION_CORNER);

	// Draw buttons
	for (int i = 0; i < 5; i++) {
		if (IsButtonHovered(btnX, btnY[i], btnWidth, btnHeight))
			CP_Image_Draw(button_green, btnX, btnY[i], btnWidth, btnHeight, 255);
		else
			CP_Image_Draw(button_white, btnX, btnY[i], btnWidth, btnHeight, 255);
	}

	// Play click sound for any button
	for (int i = 0; i < 5; i++) {
		if (IsButtonClicked(btnX, btnY[i], btnWidth, btnHeight)) {
			CP_Sound_Play(BtnSound);
			break;
		}
	}

	// Button actions
	if (IsButtonClicked(btnX, btnY[0], btnWidth, btnHeight))
		CP_Engine_SetNextGameState(Game_Init, Game_Update, Game_Exit);

	if (IsButtonClicked(btnX, btnY[1], btnWidth, btnHeight))
		CP_Engine_SetNextGameState(Level_Init, Level_Update, Level_Exit);

	if (IsButtonClicked(btnX, btnY[2], btnWidth, btnHeight)) {
		showControls = 1;
		return;
	}

	if (IsButtonClicked(btnX, btnY[3], btnWidth, btnHeight))
		CP_Engine_SetNextGameState(Credits_Init, Credits_Update, Credits_Exit);

	if (!quitClicked && IsButtonClicked(btnX, btnY[4], btnWidth, btnHeight)) {
		quitClicked = 1;
		quitStartTime = CP_System_GetSeconds();
	}

	if (quitClicked && CP_System_GetSeconds() - quitStartTime >= 0.25f)
		CP_Engine_Terminate();

	// Draw button labels
	CP_Font_Set(font);
	CP_Settings_TextSize(80);
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

	for (int i = 0; i < 5; i++) {
		float textX = btnX + btnWidth / 2;
		float textY = btnY[i] + btnHeight / 2;
		CP_Font_DrawText(btnText[i], textX, textY);
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
	CP_Font_Free(myFont);
	CP_Sound_Free(openingSound);
	CP_Sound_Free(mainmenuSound);
	CP_Image_Free(button_white);
	CP_Image_Free(white_background);
	CP_Image_Free(button_green);
	CP_Sound_Free(BtnSound);
	CP_Image_Free(guide1);
	CP_Image_Free(guide2);
	CP_Image_Free(left);
	CP_Image_Free(right);
}




