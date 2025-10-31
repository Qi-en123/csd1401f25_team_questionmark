#include "cprocessing.h"
#include "utils.h"
#include "game.h"
//font
CP_Font myFont;


void Main_Menu_Init(void)
{
	//load font from asset folder
	myFont = CP_Font_Load("Assets/Exo2-Regular.ttf");

}

void Main_Menu_Update(void)
{
	//create red colour for background of rectangle
	CP_Color red = CP_Color_Create(255, 125, 125, 255);
	//create gray colour for background
	CP_Color gray = CP_Color_Create(100, 100, 100, 255);
	//create black colour for text
	CP_Color black = CP_Color_Create(0, 0, 0, 255);
	//set background to gray colour
	CP_Graphics_ClearBackground(gray);


	// set the rectangle drawing mode to CENTER
	CP_Settings_RectMode(CP_POSITION_CENTER);
	//fill subsequent rectangle with red colour
	CP_Settings_Fill(red);

	//x y coordinate for both rectangle
	float x_coordinate = 800.0f;
	float play_y = 250.0f;
	float exit_y = 650.0f;
	//height and weight of rectangle
	float height = 200.0f;
	float length = 300.0f;
	// draw the play rectangle at the center of the screen, 
	CP_Graphics_DrawRect(x_coordinate, play_y, length, height);
	// draw the exit rectangle at the center of the screen
	CP_Graphics_DrawRect(x_coordinate, exit_y, length, height);

	//set font
	CP_Font_Set(myFont);
	CP_Settings_TextSize(72.0f);
	//set colour to black now
	CP_Settings_Fill(black);
	//write play in the middle of the top rect
	CP_Font_DrawText("Play", x_coordinate - 60.0f, play_y + 25.0f);
	//write play in the middle of the bottom rect
	CP_Font_DrawText("Exit", x_coordinate - 50.0f, exit_y + 25.0f);
	// if play button is clicked, go game page
	if (CP_Input_MouseClicked() && IsAreaClicked(x_coordinate, play_y, length, height, CP_Input_GetMouseX(), CP_Input_GetMouseY()) == 1)
	{
		// go game page
		CP_Engine_SetNextGameState(Game_Init, Game_Update, Game_Exit);
		//CP_Engine_SetNextGameState(ui_init, ui_update, ui_exit);
	}
	// if exit button is clicked, terminate program
	else if (CP_Input_MouseClicked() && IsAreaClicked(x_coordinate, exit_y, length, height, CP_Input_GetMouseX(), CP_Input_GetMouseY()) == 1)
	{
		// terminate program
		CP_Engine_Terminate();
	}
	else {
		return 0;
	}
}

void Main_Menu_Exit(void)
{

	//free font
	CP_Font_Free(myFont);
}