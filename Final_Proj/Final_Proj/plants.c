#include "plants.h"
#include "cprocessing.h"
#include "player.h"
#include "tools.h"
#include "utils.h"

//Initialize all the pot values, based on how many in the level
void initialize_pots(Pot *pots, int size, Tile* tile) {

	for (int i = 0; i < size; i++) {
		int row_offset = i / 8;
		int col_offset = i % 8;
		int tile_index = col_offset + TILE_COLS * row_offset;
		pots[i].position.x = tile[tile_index].x;
		pots[i].position.y = tile[tile_index].y;
		pots[i].filled = false;
		pots[i].selected = false;
		pots[i].plant.seed_type = EMPTY;
		pots[i].plant.fillProgress = 0.0f;
		pots[i].plant.wateredOnce = false;
		tile[tile_index].obstructed = true;
		pots[i].occupied_tile = &tile[tile_index];
	}
}

//Draws all the pots, checks if player is close-by
void render_pots(Pot* pots, CP_Image pot_spritesheet, int size,
	float player_posx, float player_posy, Pot* pot, void* held_tool) {

	//ensures only 1 pot can be selected at all times
	bool pot_selected = false;

	//Resets all pots to unselected every frame
	for (int i = 0; i < size; i++) {
		pots[i].selected = false;
	}

	float range = -2.5f;
	//highlights nearby pot
	for (int i = 0; i < size; ++i) {
		if (&pots[i] == pot) { continue; } //skips rendering held pot

		float pot_x = pots[i].position.x;
		float pot_y = pots[i].position.y;

		if (AreRectsIntersecting(pot_x, pot_y, POT_WIDTH * POT_SCALE, POT_HEIGHT * POT_SCALE,
			player_posx, player_posy + (1.0f * PLAYER_SCALE),
			(PLAYER_WIDTH + range) * PLAYER_SCALE, (PLAYER_HEIGHT + range) * PLAYER_SCALE) &&
			pot == NULL) { //if player isnt holding a pot

			if (!pot_selected) {
				pots[i].selected = true;
				pot_selected = true;
			}
			if (!held_tool) { show_tooltip("Press 'J' to Pick Up"); }
		}
		else {
			pots[i].selected = false;
		}

		float start_x = POT_WIDTH * pots[i].selected; //changes sprite subimg to highlighted version
		float start_y = POT_HEIGHT - POT_HEIGHT * pots[i].filled; //changes to filled version (in this version its dug hole)

		//Pot shadow
		CP_Settings_EllipseMode(CP_POSITION_CENTER);
		CP_Settings_NoStroke();
		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 120));

		float shadow_x = pots[i].position.x - 1.0f;
		float shadow_y = pots[i].position.y + POT_HEIGHT * POT_SCALE / 2.0f - 6.0f;
		CP_Graphics_DrawEllipse(shadow_x, shadow_y, POT_WIDTH * POT_SCALE * 0.78f, 22.0f);

		//Pot
		CP_Settings_ImageMode(CP_POSITION_CENTER);
		CP_Image_DrawSubImage(pot_spritesheet, pots[i].position.x, pots[i].position.y, POT_WIDTH * POT_SCALE, POT_HEIGHT * POT_SCALE,
			start_x, start_y, start_x + POT_WIDTH, start_y + POT_HEIGHT, 255);

		//if thrs a plant on the pot
		if (pots[i].plant.seed_type != EMPTY) {
			render_plant(&pots[i].plant, pots[i].position.x, pots[i].position.y, pots[i].selected);
		}
	}
}

void render_held_pot(Pot* pot, CP_Image pot_spritesheet) {

	float start_x = POT_WIDTH * pot->selected; //changes sprite subimg to highlighted version
	float start_y = POT_HEIGHT - POT_HEIGHT * pot->filled; //changes to filled version (in this version its dug hole)

	//Pot shadow
	CP_Settings_EllipseMode(CP_POSITION_CENTER);
	CP_Settings_NoStroke();
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 120));

	CP_Graphics_DrawEllipse(pot->position.x, pot->position.y + POT_HEIGHT * POT_SCALE / 2 + 2.0f,
		POT_WIDTH * POT_SCALE * 0.65f, 15.0f);

	//Pot
	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Image_DrawSubImage(pot_spritesheet, pot->position.x, pot->position.y, POT_WIDTH * POT_SCALE, POT_HEIGHT * POT_SCALE,
		start_x, start_y, start_x + POT_WIDTH, start_y + POT_HEIGHT, 255);

	if (pot->plant.seed_type != EMPTY) {
		render_plant(&pot->plant, pot->position.x, pot->position.y, pot->selected);
	}
}

//Initialize the plant for the pot
void initialize_plant(Plant *plant, SEED_TYPE seed_type) {
	switch (seed_type) {
	default:
	case BERRY_BUSH:
		plant->plant_spritesheet = CP_Image_Load("Assets/plant_sprites/berrybush_spritesheet.png");
		plant->sunlight_needed = 1.0f; //default
		plant->water_needed = 4;
		break;
	case ORANGE_BUSH:
		plant->plant_spritesheet = CP_Image_Load("Assets/plant_sprites/orangebush_spritesheet.png");
		plant->sunlight_needed = 1.5f;
		plant->water_needed = 2;
		break;
	case CACTUS:
		plant->plant_spritesheet = CP_Image_Load("Assets/plant_sprites/cactus_spritesheet.png");
		plant->sunlight_needed = 3.0f;
		plant->water_needed = 1;
		break;
	}
	plant->growth_stage = SEED;
	plant->seed_type = seed_type;
	plant->sunlight_amount = 0;
	plant->wateredOnce = false;
	plant->fillProgress = 0.0f;
}

//Draws the plant on top of the pot
void render_plant(Plant* plant, float posx, float posy, bool selected) {
	//sprites are in order (seed, seedling, youth, adult): growth_stage 0 == seed; x=0 etc.
	//y=0, non-highlighted, y=1, highlighted
	float plant_x = posx + POT_WIDTH * POT_SCALE * 0.01f;
	float plant_y = posy - POT_HEIGHT * POT_SCALE * 0.48f;
	CP_Image_DrawSubImage(plant->plant_spritesheet, plant_x, plant_y,
		POT_WIDTH * POT_SCALE, POT_HEIGHT * POT_SCALE,
		PLANT_WIDTH * plant->growth_stage, PLANT_HEIGHT * selected,
		PLANT_WIDTH + PLANT_WIDTH * plant->growth_stage, PLANT_HEIGHT + PLANT_HEIGHT * selected, 255);

	//Increase sunlight bar
	if (plant->growth_stage != ADULT) {
		plant->sunlight_amount += dt / plant->sunlight_needed / 4.0f;
		plant->sunlight_amount = CP_Math_ClampFloat(plant->sunlight_amount, 0.0f, 1.0f);

		if (selected) {
			//SUNLIGHT BAR HERE:
			render_sunlight_bar(plant, posx, posy);
		}
	}

	if (plant->water_amount >= 1.0f && plant->sunlight_amount >= 1.0f) {
		grow_plant(plant);
	}
}

//Increments the plant's growth stage to the next
void grow_plant(Plant* plant) {
	//CONTENTS TO BE REPLACED WITH WATERING AND SUNLIGHT FUNCTIONS
	GROWTH_STAGE* growth_stage = &plant->growth_stage;
	*growth_stage = *growth_stage >= ADULT ? *growth_stage : *growth_stage + 1;
	
	if (growth_stage != ADULT) {
		plant->sunlight_amount = 0; //reset sunlight bar
		plant->sunlight_needed *= 1.25f; //increase sunlight needed per growth stage
		plant->water_amount = 0;
		plant->fillProgress = 0;
		plant->wateredOnce = false;
	}
}


//Renders sunlight bar on top of selected/nearest plant
void render_sunlight_bar(Plant* plant, float posx, float posy) {
	CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_StrokeWeight(3.0f);
	//Bar background
	float bar_width = 200.0f;
	float bar_height = 24.0f;
	float bar_x = posx - bar_width / 2.0f;
	float bar_y = posy - POT_HEIGHT * POT_SCALE;

	CP_Settings_RectMode(CP_POSITION_CORNER);
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 100));
	//CP_Graphics_DrawRect(posx - (bar_width / 2) + (POT_WIDTH * POT_SCALE / 2),
	//	posy - 30.0f, bar_width, bar_height);
	CP_Graphics_DrawRect(bar_x, bar_y, bar_width, bar_height);

	//Sunlight bar
	CP_Settings_Fill(CP_Color_Create(255, 255, 0, 180));
	CP_Graphics_DrawRect(bar_x, bar_y, bar_width * plant->sunlight_amount, bar_height);
	
}

void kill_plant(Pot* pot) {
	Plant* plant = &pot->plant;
	plant->seed_type = EMPTY;
	plant->growth_stage = SEED;
	plant->plant_spritesheet = NULL;
	plant->sunlight_amount = 0;
	plant->sunlight_needed = 0;
	plant->water_amount = 0;
}
