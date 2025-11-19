#include <math.h>
#include "cprocessing.h"
// Returns 1 if mouse clicked inside rectangle
int IsButtonClicked(float rectX, float rectY, float rectW, float rectH)
{
    float mouseX = CP_Input_GetMouseX();
    float mouseY = CP_Input_GetMouseY();

    // Check if mouse is inside rectangle bounds
    if (mouseX >= rectX && mouseX <= rectX + rectW &&
        mouseY >= rectY && mouseY <= rectY + rectH)
    {
        // Check left mouse button click
		if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) {
		
			return 1;
		}
    }

    return 0;
}
int IsCircleHovered(float center_x, float center_y, float diameter)
{
	float mx = CP_Input_GetMouseX();
	float my = CP_Input_GetMouseY();

	float dx = mx - center_x;
	float dy = my - center_y;

	float distance = sqrtf(dx * dx + dy * dy);
	float radius = diameter / 2.0f;

	return (distance <= radius);
}
// returns 1 if mouse is hovered
int IsButtonHovered(float rectX, float rectY, float rectW, float rectH)
{
	float mx = CP_Input_GetMouseX();
	float my = CP_Input_GetMouseY();

	return (mx >= rectX && mx <= rectX + rectW &&
		my >= rectY && my <= rectY + rectH);
}


// if mouse is within left to right of triangle and y is within bottom to top of triangle
int IsCircleClicked(float circle_center_x, float circle_center_y, float diameter, float click_x, float click_y)
{
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) {
		//this calculates the distance of x and y coordinate of click to x and y coordinate of circle center
		float distance_x = click_x - circle_center_x;
		float distance_y = click_y - circle_center_y;
		//use pythagoras theorem to calculate between circle center to point
		float distance = sqrt(distance_x * distance_x + distance_y * distance_y);
		// calculate radius of circle
		float radius = diameter / 2;
		// if distance is less than radius means inside circle, return 1
		if (distance <= radius) {
			return 1;
		}
		//esle return 0 (not inside circle)
		else {
			return 0;
		}
	}
}
int AreCirclesIntersecting(float c1_x, float c1_y, float r1, float c2_x, float c2_y, float r2)
{
	//this calculates the distance of x and y coordinate of click to x and y coordinate of circle center
	float distance_x = c1_x - c2_x;
	float distance_y = c1_y - c2_y;
	//use pythagoras theorem to calculate between circle center to point
	float distance = sqrt(distance_x * distance_x + distance_y * distance_y);
	//if distance between circle 1 centre and circle 2 centre is less than the sum of 2 radius means intersecting
	return distance <= r1 + r2 ? 1 : 0;
}
int IsWithinRect(float rect_x, float rect_y, float rect_w, float rect_h, float posx, float posy) {
	float left = rect_x - rect_w / 2.0f;
	float right = rect_x + rect_w / 2.0f;
	float top = rect_y - rect_h / 2.0f;
	float down = rect_y + rect_h / 2.0f;

	return (posx >= left && posx <= right && posy >= top && posy <= down);
}

void show_tooltip(const char* text) {
	CP_Font font;
	font = CP_Font_Load("Assets/Exo2-Regular.ttf");
	CP_Font_Set(font);
	CP_Settings_TextSize(50.0f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_RIGHT, CP_TEXT_ALIGN_V_TOP);
	CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
	CP_Font_DrawText(text, 1550.0f, 830.0f);
}
