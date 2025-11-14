#pragma once
#include "tools.h"
#include "cprocessing.h"

void Game_Init(void);

void Game_Update(void);

void Game_Exit(void);	

void start_timer();
void move_tool(float* posx, float* posy, float* rotation);
void handle_tool_interaction(Tool* tool);
void spawn_box(void);


void draw_gameover_panel(int stars, int score);