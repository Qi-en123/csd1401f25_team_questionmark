#pragma once
#include "tools.h"
#include "plants.h"

void Game_Init(void);

void Game_Update(void);

void Game_Exit(void);	

void start_timer();
void move_tool(float* posx, float* posy, float* rotation);
void handle_tool_interaction(Tool* tool);
void spawn_box(void);
void move_pot(float* posx, float* posy);
void handle_pot_interaction(Pot* pot);
void customer_request();
