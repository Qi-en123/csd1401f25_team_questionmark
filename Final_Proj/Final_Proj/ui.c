/*
#include "cprocessing.h"
#include "mainmenu.h"
#include <stdio.h>
#include <stdlib.h>

void spawn_box(void);

CP_Image timerbackground = NULL;
CP_Image requestbg = NULL;
CP_Font myFont;


typedef struct {
    float x, y;
    float targetX;
    float width, height;
    float speed;
} Box;

#define MAX_BOXES 5
Box boxes[MAX_BOXES];
int boxCount = 0;
float spawnTimer = 0.0f;

void ui_init(void) {
    timerbackground = CP_Image_Load("Assets/button_rectangle_depth_border.png");
    requestbg = CP_Image_Load("Assets/button_square_gradient.png");
    CP_Font_Load("Assets/Exo2-Regular.ttf");
    CP_Settings_ImageMode(CP_POSITION_CORNER);
    CP_System_SetWindowSize(CP_System_GetDisplayWidth(), CP_System_GetDisplayHeight());
    
    CP_System_SetFrameRate(60);
    boxCount = 0;
    spawnTimer = 0;

    // Set large text size
    CP_Settings_TextSize(150);

    // Center align text horizontally and vertically
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);

    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));

}

void ui_update(void) {
    // place the box top left
    CP_Graphics_ClearBackground(CP_Color_Create(0, 0, 0, 255));
    CP_Image_Draw(timerbackground, 0.0f, 0.0f, CP_Image_GetWidth(timerbackground) + 150.0f, CP_Image_GetHeight(timerbackground) + 100.0f, 255);

    float dt = CP_System_GetDt();
    spawnTimer += dt;

    // Spawn every 5 seconds
    if (spawnTimer >= 5.0f) {
        spawn_box();
        spawnTimer = 0.0f;
    }

    // Update positions
    for (int i = 0; i < boxCount; i++) {
        if (boxes[i].x > boxes[i].targetX) {
            boxes[i].x -= boxes[i].speed;
            if (boxes[i].x < boxes[i].targetX)
                boxes[i].x = boxes[i].targetX;
        }


        CP_Image_Draw(requestbg, boxes[i].x, boxes[i].y, boxes[i].width, boxes[i].height, 255);
    }



    //q to exit
    if (CP_Input_KeyDown(KEY_Q)) {
        CP_Engine_SetNextGameStateForced(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit);
    }
}

void ui_exit(void) {
 
    CP_Image_Free(timerbackground);
    CP_Image_Free(requestbg);
}

void spawn_box(void) {
    if (boxCount >= MAX_BOXES) return;

    float screenW = (float)CP_System_GetWindowWidth();

    //  Shift all existing boxes left
    for (int i = 0; i < boxCount; i++) {
        boxes[i].targetX -= (boxes[i].width + 10);  // push left
    }

    //  Create new box on the right
    Box newBox;
    newBox.width = 300; // size of req box
    newBox.height = 250;
    newBox.x = screenW;        // start off-screen right
    newBox.y = 10.0f;          // top position
    newBox.targetX = screenW - (newBox.width + 10);  // rightmost visible spot
    newBox.speed = 5.0f;

    // Add new box to array
    boxes[boxCount++] = newBox;
}*/