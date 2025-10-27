#pragma once

int IsButtonClicked(float rectX, float rectY, float rectW, float rectH);
int IsCircleClicked(float circle_center_x, float circle_center_y, float diameter, float click_x, float click_y);
int AreCirclesIntersecting(float c1_x, float c1_y, float r1, float c2_x, float c2_y, float r2);
int IsButtonHovered(float rectX, float rectY, float rectW, float rectH);
