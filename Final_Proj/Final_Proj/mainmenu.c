#include "cprocessing.h"
#include "utils.h"
#include "game.h"
#include "levels.h"
#include "credits.h"
//image
CP_Image spriteSheet;
CP_Image mainmenu_bg;
CP_Image button_white;
CP_Image button_green;
//font 
CP_Font font;

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



void Main_Menu_Init(void)
{
    //load mainmenu background and spritesheet for cloud from asset folder
    mainmenu_bg = CP_Image_Load("Assets/backgroundColorGrass.png");
    spriteSheet = CP_Image_Load("Assets/spritesheet_default.png");
    button_white = CP_Image_Load("Assets/button_rectangle_line.png");
    button_green = CP_Image_Load("Assets/button_rectangle_gloss.png");
    //load font 
    font = CP_Font_Load("Assets/Kenney Pixel Square.ttf");

    // x coordinate of background initate to 0 whenever we reopen mainmenu
    bgX = 0;
    // Get background width
    bgWidth = CP_Image_GetWidth(mainmenu_bg);



}

void Main_Menu_Update(void)
{
    //screen width and height
    float screenW = CP_System_GetWindowWidth();
    float screenH = CP_System_GetWindowHeight();

    // Update positions of bg
    bgX += bgSpeed;
    if (bgX >= bgWidth) bgX -= bgWidth; // once file bg image is out of screen, restart from left side

    // Draw tileable background
    // Calculate positions for two tiles so they appear seamless
    // cast first drawing to int, so that there wont be pixel offsets which eventually becomes thin line in the ui

    /*When bgX is small, first tile covers left side, second tile covers 1024 px to the right.
    Screen width = 1600 , thats why need third tile to cover the rest
    */
    int bgDrawX1 = -(int)bgX + bgWidth / 2;  // first tile center
    int bgDrawX2 = bgDrawX1 + bgWidth;      // second tile immediately to the right
    int bgDrawX3 = bgDrawX2 + bgWidth;      // third tile to ensure full coverage 

    // set the rectangle drawing mode to center for background
    CP_Settings_ImageMode(CP_POSITION_CENTER);
    CP_Image_Draw(mainmenu_bg, bgDrawX1, screenH / 2, bgWidth, screenH, 255);
    CP_Image_Draw(mainmenu_bg, bgDrawX2, screenH / 2, bgWidth, screenH, 255);
    CP_Image_Draw(mainmenu_bg, bgDrawX3, screenH / 2, bgWidth, screenH, 255);

    // --- Update clouds ---
    for (int i = 0; i < NUM_CLOUDS; i++) {
        cloudX[i] += cloudSpeed[i];
        if (cloudX[i] >= screenW + 175)
            cloudX[i] = -175; // smooth wrap
        CP_Image_DrawSubImage(spriteSheet, cloudX[i], screenH / 6, 300, 250, // x, y , width, height
            0, 218, 251, 363, 255); // left, top, right, bottom, alpha
    }

    // --- Buttons setup ---
    int x_coord = 650; // shared x coordinate for all buttons
    float btnWidth = 500.0f;
    float btnHeight = 90.0f;
    float btnX = x_coord - 100;
    float btnY[5] = { 225.0f, 325.0f, 425.0f, 525.0f, 625.0f };
    const char* btnText[5] = { "Start", "Levels", "Controls", "Credits", "Quit" };

    // set the buttons drawing mode to corner so that its easier to align with the text
    CP_Settings_ImageMode(CP_POSITION_CORNER);

    for (int i = 0; i < 5; i++) {
        // Draw button image based on hover or click
        if (IsButtonHovered(btnX, btnY[i], btnWidth, btnHeight) || IsButtonClicked(btnX, btnY[i], btnWidth, btnHeight)) {
            CP_Image_Draw(button_green, btnX, btnY[i], btnWidth, btnHeight, 255);
        }
        else {
            CP_Image_Draw(button_white, btnX, btnY[i], btnWidth, btnHeight, 255);
        }
    }

    // if levels button is clicked, go to levels page
    if (IsButtonClicked(btnX, btnY[0], btnWidth, btnHeight) == 1) {
        CP_Engine_SetNextGameState(Game_Init, Game_Update, Game_Exit);
    }

    // if levels button is clicked, go to levels page
    if (IsButtonClicked(btnX, btnY[1], btnWidth, btnHeight) == 1) {
        CP_Engine_SetNextGameState(Level_Init, Level_Update, Level_Exit);
    }

    // if credits button is clicked, go to credits page
    if (IsButtonClicked(btnX, btnY[3], btnWidth, btnHeight) == 1) {
        CP_Engine_SetNextGameState(Credits_Init, Credits_Update, Credits_Exit);
    }

    // if exit button is clicked, terminate the game
    if (IsButtonClicked(btnX, btnY[4], btnWidth, btnHeight) == 1) {
        CP_Engine_Terminate();
    }

    // set font
    CP_Font_Set(font);
    CP_Settings_TextSize(80.0f);
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE); // center text

    // draw texts for buttons
    for (int i = 0; i < 5; i++) {
        float textX = btnX + btnWidth / 2;
        float textY = btnY[i] + btnHeight / 2;
        CP_Font_DrawText(btnText[i], textX, textY);
    }


    if (CP_Input_KeyTriggered(KEY_ESCAPE)) {
        CP_Engine_Terminate();
    }
}

void Main_Menu_Exit(void)
{

    //free image and font
    CP_Image_Free(spriteSheet);
    CP_Image_Free(mainmenu_bg);
    CP_Image_Free(button_white);
    CP_Image_Free(button_green);
    CP_Font_Free(font);
}
