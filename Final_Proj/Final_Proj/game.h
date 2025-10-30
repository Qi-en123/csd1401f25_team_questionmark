#pragma once
#include "tools.h"

void Game_Init(void);

void Game_Update(void);

void Game_Exit(void);	

void start_timer();
void move_tool(float* posx, float* posy, float* rotation);
void handle_tool_interaction(Tool* tool);
