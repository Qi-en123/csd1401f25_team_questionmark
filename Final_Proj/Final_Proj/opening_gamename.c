#include "cprocessing.h"
#include "mainmenu.h"

CP_Image gameName;

CP_BOOL opening_gameName = FALSE;
int alpha_gameName = 0;
extern CP_Sound openingSound;
float startTime_gameName = 0;

void Opening_GameName_Init(void)
{
    openingSound = CP_Sound_LoadMusic("Assets/opening.wav");
    gameName = CP_Image_Load("Assets/gamename.png");
    alpha_gameName = 0;
    opening_gameName = FALSE;
    startTime_gameName = CP_System_GetSeconds();
}

void Opening_GameName_Update(void)
{

    CP_Graphics_ClearBackground(CP_Color_Create(0, 0, 0, 50));
    if (!opening_gameName)
    {
        if (alpha_gameName < 255) alpha_gameName += 2;  // fade in


        CP_Settings_ImageMode(CP_POSITION_CENTER);
        CP_Image_Draw(gameName,
            CP_System_GetWindowWidth() / 2.0f,
            CP_System_GetWindowHeight() / 2.0f,
            (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight(), alpha_gameName);









        // After 4 seconds, go to next state
        if (CP_System_GetSeconds() - startTime_gameName > 3.0f)
            opening_gameName = TRUE;
    }
    else
    {
        CP_Engine_SetNextGameState(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
    }
}

void Opening_GameName_Exit(void)
{
    CP_Image_Free(gameName);
  
}