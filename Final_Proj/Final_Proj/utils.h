#pragma once
#include "cprocessing.h"

int IsAreaClicked(float area_center_x, float area_center_y, float area_width, float area_height, float click_x, float click_y);
int IsCircleClicked(float circle_center_x, float circle_center_y, float diameter, float click_x, float click_y);
int AreCirclesIntersecting(float c1_x, float c1_y, float r1, float c2_x, float c2_y, float r2);

typedef struct Vector2 {
	float x, y;
}Vector2;

//tint_val should be 1/4, 1/2, or 3/4 etc.
CP_Color tintColor(CP_Color color, float tint_val);

typedef enum { false, true } bool;
