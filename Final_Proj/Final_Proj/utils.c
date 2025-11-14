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

//Returns the tinted version of color.
//Note: tint_val must be float
CP_Color tintColor(CP_Color color, float tint_val) {
	float r, g, b;
	r = color.r + (255 - color.r) * tint_val;
	g = color.g + (255 - color.g) * tint_val;
	b = color.b + (255 - color.b) * tint_val;

	return CP_Color_Create(r,g,b,255);
}
