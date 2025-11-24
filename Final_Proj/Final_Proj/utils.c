#include <math.h>
#include "cprocessing.h"
#include <stdio.h>
#include "utils.h"
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
int IsButtonHovered_Center(float centerX, float centerY, float width, float height)
{
	float mouseX = CP_Input_GetMouseX();
	float mouseY = CP_Input_GetMouseY();

	float left = centerX - width / 2;
	float right = centerX + width / 2;
	float top = centerY - height / 2;
	float bottom = centerY + height / 2;

	return (mouseX >= left && mouseX <= right && mouseY >= top && mouseY <= bottom);
}
int IsButtonClicked_Center(float centerX, float centerY, float rectW, float rectH)
{
	float mouseX = CP_Input_GetMouseX();
	float mouseY = CP_Input_GetMouseY();

	float left = centerX - rectW / 2;
	float right = centerX + rectW / 2;
	float top = centerY - rectH / 2;
	float bottom = centerY + rectH / 2;

	if (mouseX >= left && mouseX <= right &&
		mouseY >= top && mouseY <= bottom)
	{
		if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT))
			return 1;
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
	}
	return 0;
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

int AreRectsIntersecting(float rect1_x, float rect1_y, float width1, float height1,
	float rect2_x, float rect2_y, float width2, float height2) {

	float w1 = width1 / 2.0f;
	float w2 = width2 / 2.0f;
	float h1 = height1 / 2.0f;
	float h2 = height2 / 2.0f;

	int right_check = (rect1_x + w1 < rect2_x + w2) && (rect1_x + w1 > rect2_x - w2);
	int left_check = (rect1_x - w1 > rect2_x - w2) && (rect1_x - w1 < rect2_x + w2);
	int top_check = (rect1_y - h1 > rect2_y - h2) && (rect1_y - h1 < rect2_y + h2);
	int btm_check = (rect1_y + h1 < rect2_y + h2) && (rect1_y + h1 > rect2_y - h2);

	return ((right_check || left_check) && (top_check || btm_check));
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
int loadLevel() {
	FILE* file = fopen("savelevel.txt", "r");
	int level = 1;

	if (file == NULL) {
		return level;
	}

	fscanf_s(file, "%d", &level);
	fclose(file);

	// Print inside game window later, not here.
	return level;
}
void saveLevel(int level) {
	FILE* file = fopen("savelevel.txt", "w");  // open file for writing
	if (file == NULL) {
		printf("Failed to save level.\n");
		return;
	}

	fprintf(file, "%d\n", level);  // write the level number
	fclose(file);

	printf("Level saved: %d\n", level);
}
