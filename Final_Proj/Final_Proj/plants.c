#include "plants.h"
#include "cprocessing.h"

//Initialize all the pot values, based on how many in the level
void initialize_pots(Pot *pots, int size) {
	float potX = 350.0f;
	float potY = 400.0f;

	for (int i = 0; i < size; i++) {
		pots[i].position.x = potX + (i * 100.0f);
		pots[i].position.y = potY;
		pots[i].filled = false;
		pots[i].selected = false;
		pots[i].plant.seed_type = EMPTY;
	}
}

//Draws all the pots, checks if player is close-by
void render_pots(Pot* pots, int size, float player_posx, float player_posy) {
#define pot_width 19
#define pot_height 19

	CP_Image pot_spritesheet = CP_Image_Load("Assets/tool_sprites/pot_spritesheet.png");

	//ensures only 1 pot can be selected at all times
	bool pot_selected = false;

	//Resets all pots to unselected every frame
	for (int i = 0; i < size; i++) {
		pots[i].selected = false;
	}

	for (int i = 0; i < size; ++i) {
		if (AreCirclesIntersecting(player_posx, player_posy, 9.0f, //player pos and radius
			pots[i].position.x, pots[i].position.y, 50.0f)) { //pot pos and range
			if (!pot_selected) {
				pots[i].selected = true;
				pot_selected = true;
			}
		}
		else {
			pots[i].selected = false;
		}

		float start_x = pot_width * pots[i].selected; //changes sprite subimg to highlighted version
		float start_y = pot_height - pot_height * pots[i].filled; //changes to filled version (in this version its dug hole)

		CP_Image_DrawSubImage(pot_spritesheet, pots[i].position.x, pots[i].position.y, pot_width * 3.4f, pot_height * 3.4f,
			start_x, start_y, start_x + pot_width, start_y + pot_height, 255);

		if (pots[i].plant.seed_type != EMPTY) {
			render_plant(pots[i].plant, pots[i].position.x, pots[i].position.y);
		}
	}
}

//Initialize the plant for the pot
void initialize_plant(Plant *plant) {
	plant->plant_spritesheet = CP_Image_Load("Assets/plant_sprites/plant_spritesheet_eg.png");
	plant->growth_stage = SEED;
	plant->seed_type = CABBAGE; //give it a random seed type first
}

//Draws the plant on top of the pot
void render_plant(Plant plant, float posx, float posy) {
#define plant_offsety -26
#define plant_offsetx -1
#define plant_width 19
#define plant_height 19

	CP_Image_DrawSubImage(plant.plant_spritesheet, posx + plant_offsetx, posy + plant_offsety,
		pot_width * 3.4f, pot_height * 3.4f,
		19.0f * plant.growth_stage, 0.0f, plant_width + 19.0f * plant.growth_stage, plant_height, 255);
}

//Increments the plant's growth stage to the next
void grow_plant(GROWTH_STAGE *growth_stage) {
	//CONTENTS TO BE REPLACED WITH WATERING AND SUNLIGHT FUNCTIONS
	*growth_stage = *growth_stage >= ADULT ? *growth_stage : *growth_stage + 1;
}

