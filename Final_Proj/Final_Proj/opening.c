#include "cprocessing.h"
#include "opening_gamename.h"

CP_Image logo;
CP_BOOL opening = FALSE;
CP_Sound opening_sound;
int alpha = 0;

float start_time = 0;

void Opening_Init(void)
{
    opening_sound = CP_Sound_LoadMusic("Assets/opening.wav");
    logo = CP_Image_Load("Assets/DigiPen_BLACK.png");
    alpha = 0;
    opening = FALSE;
    start_time = CP_System_GetSeconds();
    CP_Sound_PlayMusic(opening_sound);
}

void Opening_Update(void)
{

    CP_Graphics_ClearBackground(CP_Color_Create(0, 0, 0,50));
    if (!opening)
    {
        if (alpha < 255) alpha += 2;  // fade in
      
        
        CP_Settings_ImageMode(CP_POSITION_CENTER);
        CP_Image_Draw(logo,
            CP_System_GetWindowWidth() / 2.0f,
            CP_System_GetWindowHeight() / 2.0f,
            1200, 700, alpha);

        // After 4 seconds, go to next state
        if (CP_System_GetSeconds() - start_time > 3.0f)
            opening = TRUE;
    }
    else
    {
        CP_Engine_SetNextGameState(Opening_GameName_Init, Opening_GameName_Update, Opening_GameName_Exit);
    }
}

void Opening_Exit(void)
{
    CP_Image_Free(logo);
}