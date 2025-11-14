#pragma once
#include "cprocessing.h"
// Sound for button clicked
CP_Sound BtnSound;
int IsButtonClicked(float rectX, float rectY, float rectW, float rectH);
int IsCircleClicked(float circle_center_x, float circle_center_y, float diameter, float click_x, float click_y);
int AreCirclesIntersecting(float c1_x, float c1_y, float r1, float c2_x, float c2_y, float r2);
int IsButtonHovered(float rectX, float rectY, float rectW, float rectH);
int IsWithinRect(float rect_x, float rect_y, float rect_w, float rect_h, float posx, float posy);


#define dt CP_System_GetDt()
typedef struct Vector2 {
	float x, y;
}Vector2;

//Returns the tinted version of color.
//Note: tint_val must be float (e.g 1.0f/4.0f)
CP_Color tintColor(CP_Color color, float tint_val);

typedef enum { false, true } bool;
