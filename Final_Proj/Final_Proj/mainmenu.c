#include "cprocessing.h"
#include "utils.h"
#include "game.h"

//image
CP_Image spriteSheet;
CP_Image mainmenu_bg;


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
    //load font from asset folder
    mainmenu_bg = CP_Image_Load("Assets/backgroundColorGrass.png");
    spriteSheet = CP_Image_Load("Assets/spritesheet_default.png");


    bgX = 0;
    // Get background width
    bgWidth = CP_Image_GetWidth(mainmenu_bg);
}

void Main_Menu_Update(void)
{
    //screen width and height
    float screenW = CP_System_GetWindowWidth();
    float screenH = CP_System_GetWindowHeight();

    // Update positions
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

    if (CP_Input_KeyTriggered(KEY_ESCAPE)) { CP_Engine_Terminate(); }

}

void Main_Menu_Exit(void)
{

    //free image
    CP_Image_Free(spriteSheet);
    CP_Image_Free(mainmenu_bg);
}
