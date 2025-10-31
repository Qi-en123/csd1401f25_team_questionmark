#include "cprocessing.h" // for all CP function
#include "mainmenu.h" // to go back with key Q
#include <math.h>   // calculations for speed and all  
#include "utils.h" //for intersecting function
//ahmad
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
void spawn_box(void);

CP_Image timerbackground = NULL;
CP_Image requestbg = NULL;
CP_Font myFont;
CP_Sound customerNoti;

typedef struct {
    float x, y;
    float targetX;
    float width, height;
    float speed;
    int removing;
    float removeSpeed;
    float lateTimer;
} Box;

// Maximum number of box
#define MAX_BOXES 5
Box boxes[MAX_BOXES];
int boxCount = 0;
float spawnTimer = 0.0f;
//ahmad


float small_circlex;
float small_circley;
float max_hp;
float current_hp;
float small_hpWidth;
float gap;
float hp_height;
int num_of_hpBar;

void Game_Init(void)
{
    // initalising all the default settings so that everytime user exit and come back, it restarts instead of continue from last checkpoint
    small_circlex = CP_System_GetWindowWidth() / 2.0f;
    small_circley = CP_System_GetWindowHeight() / 2.0f;
    max_hp = 1300.0f;
    hp_height = 45.0f;
    current_hp = max_hp;
    num_of_hpBar = 10;
    small_hpWidth = 75.0f;
    gap = (max_hp - (small_hpWidth * 10)) / 9;

    //ahmad
    srand((unsigned int)time(NULL));
    //useless alrdy : timerbackground = CP_Image_Load("Assets/button_rectangle_depth_border.png");
    requestbg = CP_Image_Load("Assets/button_square_gradient.png");
    CP_Font_Load("Assets/Exo2-Regular.ttf");
    customerNoti = CP_Sound_Load("Assets/BellBrass CTE01_28.5.wav");
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
    //ahmad
}


void Game_Update(void)
{
 
    //create blue colour for small circle
    CP_Color blue = CP_Color_Create(0, 0, 255, 255);
    //create red colour for health bar
    CP_Color health_red = CP_Color_Create(255, 0, 0, 255);
    //create dark red for empty health bar
    CP_Color empty_health_red = CP_Color_Create(128, 0, 0, 255);
    //create green colour for big circle 1
    CP_Color red = CP_Color_Create(220, 100, 100, 255);
    //create green colour for big circle 2
    CP_Color green = CP_Color_Create(100, 220, 100, 255);
    //create gray colour for background
    CP_Color gray = CP_Color_Create(210, 210, 210, 255);
    //set background colour to gray
    CP_Graphics_ClearBackground(gray);

    /*-----------------------------------------------------------------------*/
    // draw two big circle (circle of effect)
    //fill first big circle red
    CP_Settings_Fill(red);
    //initialise the x and y coordinate of red circle
    float red_circlex = CP_System_GetWindowWidth() / 4.0f;
    float red_circley = CP_System_GetWindowHeight() / 2.0f;
    //draw first big circle at the middle height of screen and 1/4 width from the left
    CP_Graphics_DrawCircle(red_circlex, red_circley, 400.0f);
    //fill second big circle green
    CP_Settings_Fill(green);
    //initialise the x and y coordinate of green circle
    float green_circlex = CP_System_GetWindowWidth() / 4.0f * 3.0f;
    float green_circley = CP_System_GetWindowHeight() / 2.0f;
    //draw second big circle at the middle height of screen and 3/4 width from the left
    CP_Graphics_DrawCircle(green_circlex, green_circley, 400.0f);

    /*-----------------------------------------------------------------------*/
    //SMALL CIRCLE AND THE WSAD MOVEMENT
    //fill small circle blue
    CP_Settings_Fill(blue);
    //draw small circle
    CP_Graphics_DrawCircle(small_circlex, small_circley, 120.0f);
    //speed to move using key "WSAD"
    float dt = CP_System_GetDt();
    float speed = 800.0f * dt;
    //if user press key WSAD, it will go up down left right respectively
    if (CP_Input_KeyDown(KEY_D)) { small_circlex += speed; }
    if (CP_Input_KeyDown(KEY_A)) { small_circlex += -speed; }
    if (CP_Input_KeyDown(KEY_S)) { small_circley += speed; }
    if (CP_Input_KeyDown(KEY_W)) { small_circley += -speed; }
    /*-----------------------------------------------------------------------*/

    //MINUS AND ADD HP
    static int inRedZone = 0;
    //if small circle is intersecting red circle
    int touchingRed = AreCirclesIntersecting(small_circlex, small_circley, 120.0f / 2.0f,red_circlex, red_circley, 400.0f / 2.0f);
    for (int i = 0; i < boxCount; i++) {
        if (!boxes[i].removing)
        {
            boxes[i].lateTimer -= dt;


            if (boxes[i].lateTimer <= 0.0f)
            {
                boxes[i].removing = 1;
                boxes[i].removeSpeed = 300.0f;
            }
        }
    }
    const char* praises[] = { "NICE!", "AWESOME!", "GOOD JOB!" };
    int numPraises = 3;
    static char currentPraise[32] = "";
    static float praiseTimer = 0.0f;

    if (touchingRed && !inRedZone)
    {

        // Remove ONE box
        if (boxCount > 0 && boxes[0].removing == 0)
        {
            boxes[0].removing = 1;
            boxes[0].removeSpeed = 300.0f;
        }
        int idx = rand() % numPraises;
        snprintf(currentPraise, sizeof(currentPraise), "%s", praises[idx]);
        praiseTimer = 1.0f; // show for 1 second
        inRedZone = 1;
    }
    else if (!touchingRed)
    {
        inRedZone = 0;
    }

    if (praiseTimer > 0.0f)
    {
        CP_Settings_TextSize(80);
        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
        CP_Settings_Fill(CP_Color_Create(255, 255, 0, 255)); // yellow text

        float screenW = (float)CP_System_GetWindowWidth();
        float screenH = (float)CP_System_GetWindowHeight();
        CP_Font_DrawText(currentPraise, screenW / 2.0f, screenH / 4.0f);

        praiseTimer -= CP_System_GetDt();
    }

    







    //ahmad
    spawnTimer += dt;
    
    // Spawn every 2 seconds
    if (spawnTimer >= 20.0f) {
        spawn_box();
        spawnTimer = 0.0f;
    }

    
    for (int i = 0; i < boxCount; i++) {
        
        if (boxes[i].removing)
        {
            boxes[i].y -= boxes[i].removeSpeed * dt;

            // If off-screen, delete it
            if (boxes[i].y + boxes[i].height < 0)
            {
                // shift boxes down
                for (int j = 1; j < boxCount; j++)
                    boxes[j - 1] = boxes[j];

                boxCount--;
                i--; // loop checks next box correctly
                continue;
            }
        }

        if (boxes[i].x > boxes[i].targetX) {
            boxes[i].x -= boxes[i].speed;
            if (boxes[i].x < boxes[i].targetX)
                boxes[i].x = boxes[i].targetX;
        }


        CP_Image_Draw(requestbg, boxes[i].x, boxes[i].y, boxes[i].width, boxes[i].height, 255);
        float timeLeft = boxes[i].lateTimer;
        if (timeLeft < 0) timeLeft = 0;

        CP_Settings_TextSize(50);
        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
        CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));

        char timerStr[8];
        snprintf(timerStr, sizeof(timerStr), "%.1f", timeLeft);

        CP_Font_DrawText(timerStr,boxes[i].x + boxes[i].width / 2,boxes[i].y + boxes[i].height + 20);
    }
    //ahmad





    CP_Settings_Fill(CP_Color_Create(164,147,147, 255));
    float potX = CP_System_GetWindowWidth() / 2.0f;
    float potY = CP_System_GetWindowHeight() / 2.0f;
    CP_Graphics_DrawCircle(potX, potY, 120.0f);

    float rectWidth = 60;
    float rectHeight = 30;
    float potRadius = 120.0f;
    int count = 5; // how many rectangles to draw

    float spacing = 0.0f;
   
    float totalWidth = count * rectWidth + (count - 1) * spacing;

   
    float rectX = potX - rectWidth / 2.0f;   // center horizontally on pot
    float rectY = potY - (120.0f / 2.0f) - rectHeight - 10.0f;

    if (AreCirclesIntersecting(small_circlex, small_circley, 120.0f / 2.0f, potX, potY, 120.0f / 2.0f)) {

        for (int i = 0; i < count; i++)
        {   // water
            float x = (rectX + i * (rectWidth + spacing)) -82;
            CP_Settings_Fill(CP_Color_Create(0, 0, 155, 255));
            CP_Graphics_DrawRect(x, rectY, rectWidth, rectHeight);
            // sun box
            CP_Settings_Fill(CP_Color_Create(255, 255, 0, 255));
            CP_Graphics_DrawRect(x, rectY-rectHeight-8, rectWidth, rectHeight);
        }
        
    }











    // if Q is pressed, go back main menu page
    if (CP_Input_KeyDown(KEY_Q)) { CP_Engine_SetNextGameStateForced(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit); }


}

void Game_Exit(void)
{
    //CP_Image_Free(timerbackground);
    CP_Image_Free(requestbg);
    CP_Sound_Free(customerNoti);
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

    newBox.removing = 0;        
    newBox.removeSpeed = 0.0f;  
    newBox.lateTimer = 8.0f;
    // Add new box to array
    boxes[boxCount++] = newBox;
    CP_Sound_Play(customerNoti);
}