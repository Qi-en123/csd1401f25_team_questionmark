#include "cprocessing.h"
#include "utils.h"
#include "game.h"
#include "mainmenu.h"
//image
CP_Image spriteSheet_level;
CP_Image mainmenu_bg_level;
CP_Image button_white_square;
CP_Image button_green_square;
CP_Image button_exitW_level;
CP_Image button_exitG_level;

//font_level 
CP_Font font_level;

// Number of clouds
#define NUM_CLOUDS 3

// x coordinates of clouds
float cloudX_level[NUM_CLOUDS] = { -350, -700, -1050 };

// speed of clouds
// diff speed so sometimes clouds can overlap and does not move in a pattern of 3
float cloudSpeed_level[NUM_CLOUDS] = { 2.2f, 1.6f, 1.4f };

// x coordinate of bg, If bgX_level were an int, adding 0.5 each frame would round down to 0, and the background would not move smoothly.
float bgX_level = 0;
// speed of bg
float bgSpeed_level = 0.5f;

// Tileable background width (in pixels)
// use tileable if not when the bg is moving, the two bg doesnt align perfectly
// leaving a thin black line in between.
int bgWidth_level;



void Level_Init(void)
{
    
    //load mainmenu background and mainmenu_bg_level_Level for cloud from asset folder
    mainmenu_bg_level = CP_Image_Load("Assets/backgroundColorGrass.png");
    spriteSheet_level = CP_Image_Load("Assets/spriteSheet_level_default.png");
    //load font_level 
    font_level = CP_Font_Load("Assets/Kenney Pixel Square.ttf");
    // load buttons for levels
    button_white_square = CP_Image_Load("Assets/button_square_depth_line.png");
    button_green_square = CP_Image_Load("Assets/button_square_depth_gloss.png");
    // for exit 
    button_exitW_level = CP_Image_Load("Assets/check_square_grey_cross.png");
    button_exitG_level = CP_Image_Load("Assets/check_square_green_cross.png");
    // sound for button click
    BtnSound = CP_Sound_Load("Assets/ADDING-MACHINE-ELECTRONIC_GEN-HDF-00038.wav");
    // x coordinate of background initate to 0 whenever we reopen mainmenu
    bgX_level = 0;
    // Get background width
    bgWidth_level = CP_Image_GetWidth(mainmenu_bg_level);



}

void Level_Update(void)
{
    //screen width and height
    float screenW = CP_System_GetWindowWidth();
    float screenH = CP_System_GetWindowHeight();

    // Update positions of bg
    bgX_level += bgSpeed_level;
    if (bgX_level >= bgWidth_level) bgX_level -= bgWidth_level; // once file bg image is out of screen, restart from left side

    // Draw tileable background
    // Calculate positions for two tiles so they appear seamless
    // cast first drawing to int, so that there wont be pixel offsets which eventually becomes thin line in the ui

    /*When bgX_level is small, first tile covers left side, second tile covers 1024 px to the right.
    Screen width = 1600 , thats why need third tile to cover the rest
    */
    int bgDrawX1 = -(int)bgX_level + bgWidth_level / 2;  // first tile center
    int bgDrawX2 = bgDrawX1 + bgWidth_level;      // second tile immediately to the right
    int bgDrawX3 = bgDrawX2 + bgWidth_level;      // third tile to ensure full coverage 

    // set the rectangle drawing mode to center for background
    CP_Settings_ImageMode(CP_POSITION_CENTER);
    CP_Image_Draw(mainmenu_bg_level, bgDrawX1, screenH / 2, bgWidth_level, screenH, 255);
    CP_Image_Draw(mainmenu_bg_level, bgDrawX2, screenH / 2, bgWidth_level, screenH, 255);
    CP_Image_Draw(mainmenu_bg_level, bgDrawX3, screenH / 2, bgWidth_level, screenH, 255);



    // --- Update clouds ---
    for (int i = 0; i < NUM_CLOUDS; i++) {
        cloudX_level[i] += cloudSpeed_level[i];
        if (cloudX_level[i] >= screenW + 175)
            cloudX_level[i] = -175; // smooth wrap
        CP_Image_DrawSubImage(spriteSheet_level, cloudX_level[i], screenH / 6, 300, 250, // x, y , width, height
            0, 218, 251, 363, 255); // left, top, right, bottom, alpha
    }
    // --- Draw semi-transparent overlay to tint background only ---\
    
    CP_Settings_ImageMode(CP_POSITION_CENTER);
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 100)); // black with alpha 100
    CP_Graphics_DrawRect(screenW / 2, screenH / 2, screenW, screenH);
   

    // --- Buttons setup ---
    int btnY = screenH/2 - 100; // shared x coordinate for all buttons
    float btnWidth = 150.0f;
    float btnHeight = 150.0f;
    float btnX[3] = { 225.0f, 725.0f, 1225.0f };
    const char* btnText[3] = { "1", "2", "3" };

    // set the buttons drawing mode to corner so that its easier to align with the text
    CP_Settings_ImageMode(CP_POSITION_CORNER);

    for (int i = 0; i < 3; i++) {
        // Draw button image based on hover or click
        if (IsButtonHovered(btnX[i], btnY, btnWidth, btnHeight) || IsButtonClicked(btnX[i], btnY, btnWidth, btnHeight)) {
            CP_Image_Draw(button_green_square, btnX[i], btnY, btnWidth, btnHeight, 255);
        }
        else {
            CP_Image_Draw(button_white_square, btnX[i], btnY, btnWidth, btnHeight, 255);
        }
    }

    // set font
    CP_Font_Set(font_level);
    CP_Settings_TextSize(100.0f);
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE); // center text

    // draw texts for buttons
    for (int i = 0; i < 3; i++) {
        float textX = btnX[i] + btnWidth / 2 + 3;
        float textY = btnY + btnHeight / 2 - 3;
        CP_Font_DrawText(btnText[i], textX, textY);
    }
    for (int i = 0; i < 5; i++) {
        if (IsButtonClicked(btnX[i], btnY, btnWidth, btnHeight)) {
            CP_Sound_Play(BtnSound);
            break; // play once even if multiple buttons are clicked
        }
    }


    if (IsButtonClicked(btnX[0], btnY, btnWidth, btnHeight) == 1) {
        CP_Engine_SetNextGameState(Game_Init, Game_Update, Game_Exit);
    }


    //draw exit button
    if (IsButtonHovered(50, 50, 100, 100) || IsButtonClicked(50, 50, 100, 100)) {
        CP_Image_Draw(button_exitG_level, 50, 50, 100, 100, 255);
    }
    else {
        CP_Image_Draw(button_exitW_level, 50, 50, 100, 100, 255);
    }

    // if exit button is clicked
    if(IsButtonClicked(50, 50, 100, 100)){ CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit); }

    






    if (CP_Input_KeyTriggered(KEY_ESCAPE)) {
        CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
    }
}

void Level_Exit(void)
{

    //free image and font_level and sound
    CP_Image_Free(spriteSheet_level);
    CP_Image_Free(mainmenu_bg_level);
    CP_Image_Free(button_white_square);
    CP_Image_Free(button_green_square);
    CP_Image_Free(button_exitW_level);
    CP_Image_Free(button_exitG_level);
    CP_Font_Free(font_level);
    CP_Sound_Free(BtnSound);

}
