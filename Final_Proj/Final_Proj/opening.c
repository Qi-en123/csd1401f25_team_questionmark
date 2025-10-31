#include "cprocessing.h"
#include "opening_gamename.h"

CP_Image logo;
CP_BOOL opening = FALSE;
int alpha = 0;

float startTime = 0;

void Opening_Init(void)
{
    logo = CP_Image_Load("Assets/DigiPen_BLACK.png");
    alpha = 0;
    opening = FALSE;
    startTime = CP_System_GetSeconds();
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
        if (CP_System_GetSeconds() - startTime > 3.0f)
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