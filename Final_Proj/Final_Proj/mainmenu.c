    #include "cprocessing.h"
    #include "utils.h"
    #include "game.h"
    #include "levels.h"
    #include "credits.h"
    //image
    CP_Image spriteSheet;
    CP_Image mainmenu_bg;
    CP_Image button_white;
    CP_Image white_background;
    CP_Image button_green;
    //font 
    CP_Font font;
    CP_Font myFont;
    CP_Sound BtnSound;
   //sound
    CP_Sound themeSound;
    int quitClicked = 0;       // flag if Quit was clicked
    float quitStartTime = 0;   // time when quit button was clicked

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

    int showControls;


    void Main_Menu_Init(void)
    {
        //load mainmenu background and spritesheet for cloud from asset folder
        mainmenu_bg = CP_Image_Load("Assets/backgroundColorGrass.png");
        spriteSheet = CP_Image_Load("Assets/spritesheet_default.png");
        button_white = CP_Image_Load("Assets/button_rectangle_line.png");
        white_background = CP_Image_Load("Assets/button_rectangle_line-1.png");
        button_green = CP_Image_Load("Assets/button_rectangle_gloss.png");
        //load font 
        font = CP_Font_Load("Assets/Kenney Pixel Square.ttf");
        myFont = CP_Font_Load("Assets/Kenney Future Narrow.ttf");
        themeSound = CP_Sound_LoadMusic("Assets/ENCOURAGEMENT-CorporateMSCCRP1_58.wav");
        BtnSound = CP_Sound_Load("Assets/ADDING-MACHINE-ELECTRONIC_GEN-HDF-00038.wav");
        // x coordinate of background initate to 0 whenever we reopen mainmenu
        bgX = 0;
        // Get background width
        bgWidth = CP_Image_GetWidth(mainmenu_bg);
        
        CP_Sound_PlayMusic(themeSound);
        showControls = 0;
        

    }

    void Main_Menu_Update(void)
    {
        float screenW = CP_System_GetWindowWidth();
        float screenH = CP_System_GetWindowHeight();

        // ---------------- Background scrolling ----------------
        bgX += bgSpeed;
        if (bgX >= bgWidth) bgX -= bgWidth;

        int bgDrawX1 = -(int)bgX + bgWidth / 2;
        int bgDrawX2 = bgDrawX1 + bgWidth;
        int bgDrawX3 = bgDrawX2 + bgWidth;

        CP_Settings_ImageMode(CP_POSITION_CENTER);
        CP_Image_Draw(mainmenu_bg, bgDrawX1, screenH / 2, bgWidth, screenH, 255);
        CP_Image_Draw(mainmenu_bg, bgDrawX2, screenH / 2, bgWidth, screenH, 255);
        CP_Image_Draw(mainmenu_bg, bgDrawX3, screenH / 2, bgWidth, screenH, 255);

        // ---------------- Clouds ----------------
        for (int i = 0; i < NUM_CLOUDS; i++) {
            cloudX[i] += cloudSpeed[i];
            if (cloudX[i] >= screenW + 175) cloudX[i] = -175;
            CP_Image_DrawSubImage(spriteSheet, cloudX[i], screenH / 6,
                300, 250,
                0, 218, 251, 363,
                255);
        }

        // 
        // control window on 
        if (showControls == 1)
        {
            // dark overlay
            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 180));
            CP_Graphics_DrawRect(screenW / 2, screenH / 2, screenW, screenH);

            // panel
            CP_Settings_ImageMode(CP_POSITION_CENTER);
            float panelW = 800, panelH = 500;
            float panelX = screenW / 2;
            float panelY = screenH / 2;

            CP_Image_Draw(white_background, panelX, panelY, panelW, panelH, 255);

            // text
            CP_Font_Set(myFont);
            CP_Settings_TextSize(40);
            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
            CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

            float lineH = 55;
            float startY = panelY - 150;

            CP_Font_DrawText("Controls:", panelX, startY);
            CP_Font_DrawText("Move: [W,A,S,D]", panelX, startY + lineH);
            CP_Font_DrawText("Interact: [E]", panelX, startY + 2 * lineH);
            CP_Font_DrawText("Pick up/Drop Tool: [P]", panelX, startY + 3 * lineH);
            CP_Font_DrawText("Pause/Resume: [ESCAPE]", panelX, startY + 4 * lineH);
            CP_Font_DrawText("Back: [1]", panelX, startY + 5 * lineH);

            // return to menu
            if (CP_Input_KeyReleased(KEY_1))
                showControls = 0;

            return; 
        }

        // normal menu without controls
        

        int x_coord = 650;
        float btnWidth = 500.0f;
        float btnHeight = 90.0f;
        float btnX = x_coord - 100;
        float btnY[5] = { 225, 325, 425, 525, 625 };
        const char* btnText[5] = { "Start", "Levels", "Controls", "Credits", "Quit" };

        CP_Settings_ImageMode(CP_POSITION_CORNER);

        // Draw buttons
        for (int i = 0; i < 5; i++) {
            if (IsButtonHovered(btnX, btnY[i], btnWidth, btnHeight))
                CP_Image_Draw(button_green, btnX, btnY[i], btnWidth, btnHeight, 255);
            else
                CP_Image_Draw(button_white, btnX, btnY[i], btnWidth, btnHeight, 255);
        }

        // Play click sound for any button
        for (int i = 0; i < 5; i++) {
            if (IsButtonClicked(btnX, btnY[i], btnWidth, btnHeight)) {
                CP_Sound_Play(BtnSound);
                break;
            }
        }

        // Button actions
        if (IsButtonClicked(btnX, btnY[0], btnWidth, btnHeight))
            CP_Engine_SetNextGameState(Game_Init, Game_Update, Game_Exit);

        if (IsButtonClicked(btnX, btnY[1], btnWidth, btnHeight))
            CP_Engine_SetNextGameState(Level_Init, Level_Update, Level_Exit);

        if (IsButtonClicked(btnX, btnY[2], btnWidth, btnHeight)) {
            showControls = 1;
            return;
        }

        if (IsButtonClicked(btnX, btnY[3], btnWidth, btnHeight))
            CP_Engine_SetNextGameState(Credits_Init, Credits_Update, Credits_Exit);

        if (!quitClicked && IsButtonClicked(btnX, btnY[4], btnWidth, btnHeight)) {
            quitClicked = 1;
            quitStartTime = CP_System_GetSeconds();
        }

        if (quitClicked && CP_System_GetSeconds() - quitStartTime >= 0.25f)
            CP_Engine_Terminate();

        // Draw button labels
        CP_Font_Set(font);
        CP_Settings_TextSize(80);
        CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

        for (int i = 0; i < 5; i++) {
            float textX = btnX + btnWidth / 2;
            float textY = btnY[i] + btnHeight / 2;
            CP_Font_DrawText(btnText[i], textX, textY);
        }

        if (CP_Input_KeyTriggered(KEY_ESCAPE))
            CP_Engine_Terminate();
    }


    void Main_Menu_Exit(void)
    {

        //free image and font
        CP_Image_Free(spriteSheet);
        CP_Image_Free(mainmenu_bg);
        CP_Font_Free(font);
        CP_Font_Free(myFont);
        CP_Sound_Free(themeSound);
        CP_Image_Free(button_white);
        CP_Image_Free(white_background);
        CP_Image_Free(button_green);
        CP_Sound_Free(BtnSound);
    }




