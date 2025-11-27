#include "cprocessing.h"
#include "utils.h"
#include "mainmenu.h"

//image
CP_Image spriteSheet_credits;
CP_Image mainmenu_bg_credits;
CP_Image button_exitW_credits;
CP_Image button_exitG_credits;

//font_credits 
CP_Font font_credits;

// Number of clouds
#define NUM_CLOUDS 3

// x coordinates of clouds
float cloudX_credits[NUM_CLOUDS] = { -350, -700, -1050 };

// speed of clouds
// diff speed so sometimes clouds can overlap and does not move in a pattern of 3
float cloudSpeed_credits[NUM_CLOUDS] = { 2.2f, 1.6f, 1.4f };

// x coordinate of bg, If bgX_credits were an int, adding 0.5 each frame would round down to 0, and the background would not move smoothly.
float bgX_credits = 0;
// speed of bg
float bgSpeed_credits = 0.5f;

// Tileable background width (in pixels)
// use tileable if not when the bg is moving, the two bg doesnt align perfectly
// leaving a thin black line in between.
float bgWidth_credits;

// Rolling credits
float yPos_credits; // vertical position of credits
const char* credits[] = {
    "Team Questionmark",
    "Team Members:",
    "DIRECTOR and PROGRAMMER",
    "Ahmad Qusyairi",
    "PRODUCER, DESIGN LEAD and PROGRAMMER",
    "Goh Qi En",
    "AUDIO LEAD, COMPOSER, and PROGRAMMER",
    "Jerod Lim",
    "   ",
    "Faculty and Advisors",
    "Lab management and Instructor",
    "MR Gerald Wong",
    "MR Tommy Tan",
    "DR Soroor",
    "   ",
    "Created at ",
    "DigiPen Institute of Technology Singapore",
    "PRESIDENT",
    "Claude COMAIR",
    "EXECUTIVES",
    "Claude COMAIR   CHU Jason Yeu Tat   Michael GATS",
    "TAN Chek Ming   Prasanna Kumar GHALI",   
    "Mandy WONG   Johnny DEEK",
    "   ",
    "Copyrights for software, tools and libraries",
    "(C) Kenney Assets",
    "(C) MetaDigger",
    NULL // marks end of array
};

void Credits_Init(void)
{
    //load mainmenu background and mainmenu_bg_credits_credits for cloud from asset folder
    mainmenu_bg_credits = CP_Image_Load("Assets/backgroundColorGrass.png");
    spriteSheet_credits = CP_Image_Load("Assets/spriteSheet_credits_default.png");
    //load font_credits 
    font_credits = CP_Font_Load("Assets/Kenney Pixel Square.ttf");

    // for exit 
    button_exitW_credits = CP_Image_Load("Assets/check_square_grey_cross.png");
    button_exitG_credits = CP_Image_Load("Assets/check_square_green_cross.png");
    // for button click sound
    BtnSound = CP_Sound_Load("Assets/ADDING-MACHINE-ELECTRONIC_GEN-HDF-00038.wav");
    // x coordinate of background initate to 0 whenever we reopen mainmenu
    bgX_credits = 0;
    // Get background width
    bgWidth_credits = (float)CP_Image_GetWidth(mainmenu_bg_credits);

    // initialize credits starting position at bottom of screen
    yPos_credits = (float)CP_System_GetWindowHeight();
}

void Credits_Update(void)
{
    //screen width and height
    float screenW = (float)CP_System_GetWindowWidth();
    float screenH = (float)CP_System_GetWindowHeight();

    // Update positions of bg
    bgX_credits += bgSpeed_credits;
    if (bgX_credits >= bgWidth_credits) bgX_credits -= bgWidth_credits; // once bg image is out of screen, restart from left side

    // Draw tileable background
    // Calculate positions for two tiles so they appear seamless
    // cast first drawing to int, so that there wont be pixel offsets which eventually becomes thin line in the ui
    int bgDrawX1 = (int)(- bgX_credits + bgWidth_credits / 2);  // first tile center
    int bgDrawX2 = (int)(bgDrawX1 + bgWidth_credits);               // second tile immediately to the right
    int bgDrawX3 = (int)(bgDrawX2 + bgWidth_credits);               // third tile to ensure full coverage 

    // set the rectangle drawing mode to center for background
    CP_Settings_ImageMode(CP_POSITION_CENTER);
    CP_Image_Draw(mainmenu_bg_credits, (float)bgDrawX1, screenH / 2, bgWidth_credits, screenH, 255);
    CP_Image_Draw(mainmenu_bg_credits, (float)bgDrawX2, screenH / 2, bgWidth_credits, screenH, 255);
    CP_Image_Draw(mainmenu_bg_credits, (float)bgDrawX3, screenH / 2, bgWidth_credits, screenH, 255);

    // --- Update clouds ---
    for (int i = 0; i < NUM_CLOUDS; i++) {
        cloudX_credits[i] += cloudSpeed_credits[i];
        if (cloudX_credits[i] >= screenW + 175)
            cloudX_credits[i] = -175; // smooth wrap
        CP_Image_DrawSubImage(spriteSheet_credits, cloudX_credits[i], screenH / 6, 300, 250, // x, y , width, height
            0, 218, 251, 363, 255); // left, top, right, bottom, alpha
    }

    // --- Draw semi-transparent overlay to tint background only ---
    CP_Settings_ImageMode(CP_POSITION_CENTER);
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 100)); // black with alpha 100
    CP_Graphics_DrawRect(screenW / 2, screenH / 2, screenW, screenH);

    // --- Draw rolling credits ---
    CP_Font_Set(font_credits);
    CP_Settings_TextSize(50.0f); // size of the credits
    CP_Settings_Fill(CP_Color_Create(255, 255, 0, 255)); // yellow text

    // scroll speed
    yPos_credits -= 1.0f;

    // draw each line of credits
    for (int i = 0; credits[i] != NULL; i++) {
        float y = yPos_credits + i * 60; // increase Y per line for spacing
        CP_Font_DrawText(credits[i], screenW / 2, y); // center X
    }

    // reset when all lines scroll off screen
    int totalLines = 0;
    while (credits[totalLines] != NULL) totalLines++;
    if (yPos_credits + totalLines * 60 < 0) {
        yPos_credits = screenH;
    }

    // --- Draw exit button ---
    CP_Settings_ImageMode(CP_POSITION_CORNER);
    if (IsButtonHovered(50, 50, 100, 100) || IsButtonClicked(50, 50, 100, 100)) {
        CP_Image_Draw(button_exitG_credits, 50, 50, 100, 100, 255);
    }
    else {
        CP_Image_Draw(button_exitW_credits, 50, 50, 100, 100, 255);
    }
    // play button click sound when exit button is clicked
    if (IsButtonClicked(50, 50, 100, 100)) {
        CP_Sound_Play(BtnSound);
    }

    // if exit button is clicked
    if (IsButtonClicked(50, 50, 100, 100)) {
        CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
    }

    // handle ESC key
    if (CP_Input_KeyTriggered(KEY_ESCAPE)) {
        CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
    }
}


void Credits_Exit(void)
{
    //free image and font_credits and sound
    CP_Image_Free(spriteSheet_credits);
    CP_Image_Free(mainmenu_bg_credits);
    CP_Image_Free(button_exitW_credits);
    CP_Image_Free(button_exitG_credits);

    CP_Font_Free(font_credits);

}