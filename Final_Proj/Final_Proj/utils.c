#include <math.h>
#include "cprocessing.h"

int IsAreaClicked(float area_center_x, float area_center_y, float area_width, float area_height, float click_x, float click_y)
{
	// if mouse is within left to right of triangle and y is within bottom to top of triangle
	if ((click_y >= area_center_y - 0.5 * area_height && click_y <= area_center_y + 0.5 * area_height) && (click_x >= area_center_x - 0.5 * area_width && click_x <= area_center_x + 0.5 * area_width))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
// if mouse is within left to right of triangle and y is within bottom to top of triangle
int IsCircleClicked(float circle_center_x, float circle_center_y, float diameter, float click_x, float click_y)
{
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

//tint: color + (255-color) * 1/4 or 1/2 or 3/4 (greater the val, lighter the tint)
CP_Color tintColor(CP_Color color, float tint_val) {
	float r, g, b;
	r = color.r + (255 - color.r) * tint_val;
	g = color.g + (255 - color.g) * tint_val;
	b = color.b + (255 - color.b) * tint_val;

	return CP_Color_Create(r,g,b,255);
}