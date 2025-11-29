#pragma once

#include "cprocessing.h"

// Sound for button clicked
CP_Sound btn_sound;

//shortcut for deltatime
#define dt CP_System_GetDt()

typedef struct Vector2 {
	float x, y;
}Vector2;

typedef enum { false, true } bool;

//Check if rectangle button is clicked
int IsButtonClicked(float rectX, float rectY, float rectW, float rectH);

//Check if mouse is hovering over rectangle button
int IsButtonHovered(float rectX, float rectY, float rectW, float rectH);

//Check if circle is clicked
int IsCircleClicked(float circle_center_x, float circle_center_y, float diameter, float click_x, float click_y);

//Check if mouse is hovering over circle
int IsCircleHovered(float center_x, float center_y, float diameter);

//qien
int IsButtonClicked_Center(float centerX, float centerY, float rectW, float rectH);

//qien
int IsButtonHovered_Center(float centerX, float centerY, float width, float height);

//Check is 2 circles are intersecting
int AreCirclesIntersecting(float c1_x, float c1_y, float r1, float c2_x, float c2_y, float r2);

//Check if 2 rects are intersecting
int AreRectsIntersecting(float rect1_x, float rect1_y, float width1, float height1,
	float rect2_x, float rect2_y, float width2, float height2);

//Check if given position is within rect
int IsWithinRect(float rect_x, float rect_y, float rect_w, float rect_h, float posx, float posy);

//Draws tooltip text on bottom right of the screen
void show_tooltip(const char* text);

//qien
int loadLevel();

//qien
void saveLevel(int level);
