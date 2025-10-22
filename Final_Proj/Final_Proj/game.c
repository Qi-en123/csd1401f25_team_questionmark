#include "cprocessing.h" // for all CP function
#include "mainmenu.h" // to go back with key Q
#include <math.h>   // calculations for speed and all  
#include "utils.h" //for intersecting function



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
    float speed = 400.0f * dt;
    //if user press key WSAD, it will go up down left right respectively
    if (CP_Input_KeyDown(KEY_D)) { small_circlex += speed; }
    if (CP_Input_KeyDown(KEY_A)) { small_circlex += -speed; }
    if (CP_Input_KeyDown(KEY_S)) { small_circley += speed; }
    if (CP_Input_KeyDown(KEY_W)) { small_circley += -speed; }
    /*-----------------------------------------------------------------------*/

    //MINUS AND ADD HP

    //if small circle is intersecting red circle
    if (AreCirclesIntersecting(small_circlex, small_circley, 120.0f / 2.0f, red_circlex, red_circley, 400.0f / 2.0f)) {
        if (current_hp > 0) //as long as hp is not 0
            current_hp -= 100.0f * dt; //we minus hp 
    }
    //if small circle is intersecting red circle
    if (AreCirclesIntersecting(small_circlex, small_circley, 120.0f / 2.0f, green_circlex, green_circley, 400.0f / 2.0f)) {
        if (current_hp < max_hp) //as long as hp is less than max hp
            current_hp += 100.0f * dt; //we add hp
    }











    /*----------------------------------------------------------------------*/
    //HEALTH BAR

    /*DO NOTE THAT WE WILL CODE THE SMALL HP BAR SUCH THAT ONLY WHEN FULL THEN THE 10TH SMALL HP BAR WILL APPEAR,
    LIKEWISE CURRENT HP IS 89% THEN WILL SHOW 8TH BAR ETC, SO THAT USER ROUGHLY KNOW HP HAVE HOW MANY PERCENTAGE LEFT.
    THIS IS TO LET USER KNOW HP BAR IS FULL
    */

    //Set position to corner so that we can allign the small hp bar later 
    CP_Settings_RectMode(CP_POSITION_CORNER);
    //fill empty health bar dark red colour
    CP_Settings_Fill(empty_health_red);
    //draw empty health bar
    CP_Graphics_DrawRect(150.0f, 40.0f, max_hp, hp_height);

    //fill health bar red colour
    CP_Settings_Fill(health_red);
    //draw rectangle (health red)
    CP_Graphics_DrawRect(150.0f, 40.0f, current_hp, hp_height);
    // Calculate how many small hp bar to show
    int HpToShow = ((current_hp / max_hp) * num_of_hpBar);
    // draw the expected small hp bar with x position of each bar + the gaps and previous hp bars width
    for (int i = 0; i < HpToShow; i++) {
        float x = 150.0f + i * (small_hpWidth + gap);
        CP_Graphics_DrawRect(x, 130.0f, small_hpWidth, hp_height);
    }
    /*-----------------------------------------------------------------------*/






























    // if Q is pressed, go back main menu page
    if (CP_Input_KeyDown(KEY_Q)) { CP_Engine_SetNextGameStateForced(Main_Menu_Init, Main_Menu_Update, Main_Menu_Exit); }


}

void Game_Exit(void)
{

}