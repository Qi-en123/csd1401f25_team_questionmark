#include "plants.h"
#include "cprocessing.h"

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
		pots[i].fillProgress = 0.0f;
		pots[i].watered = false;
		pots[i].wateredOnce = false;
		tile[tile_index].obstructed = true;
		pots[i].occupied_tile = &tile[tile_index];
	}
}

#define POT_WIDTH 19
#define POT_HEIGHT 19
//Draws all the pots, checks if player is close-by
void render_pots(Pot* pots, CP_Image pot_spritesheet, int size, float player_posx, float player_posy, Pot* pot) {

	//ensures only 1 pot can be selected at all times
	bool pot_selected = false;

	//Resets all pots to unselected every frame
	for (int i = 0; i < size; i++) {
		pots[i].selected = false;
	}

	//highlights nearby pot
	for (int i = 0; i < size; ++i) {
		if (&pots[i] == pot) { continue; } //skips rendering held pot
		if (IsWithinRect(pots[i].position.x, pots[i].position.y, //pot pos
			CP_System_GetWindowWidth() / TILE_COLS, CP_System_GetWindowHeight() / TILE_ROWS,  //pot height & width
			player_posx, player_posy + 8.0f) &&  //player pos, abit of offset on y-axis for accuracy
			pot == NULL) { //if player isnt holding a pot
			if (!pot_selected) {
				pots[i].selected = true;
				pot_selected = true;
			}
		}
		else {
			pots[i].selected = false;
		}

		float start_x = POT_WIDTH * pots[i].selected; //changes sprite subimg to highlighted version
		float start_y = POT_HEIGHT - POT_HEIGHT * pots[i].filled; //changes to filled version (in this version its dug hole)

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

	CP_Image_DrawSubImage(pot_spritesheet, pot->position.x, pot->position.y, POT_WIDTH * POT_SCALE, POT_HEIGHT * POT_SCALE,
		start_x, start_y, start_x + POT_WIDTH, start_y + POT_HEIGHT, 255);

	if (pot->plant.seed_type != EMPTY) {
		render_plant(&pot->plant, pot->position.x, pot->position.y, pot->selected);
	}
}

//Initialize the plant for the pot
void initialize_plant(Plant *plant, SEED_TYPE seed_type) {
	switch (seed_type) {
	case BERRY_BUSH:
		plant->plant_spritesheet = CP_Image_Load("Assets/plant_sprites/berrybush_spritesheet.png");
		plant->sunlight_needed = 1.0f; //default
		break;
	case ORANGE_BUSH:
		plant->plant_spritesheet = CP_Image_Load("Assets/plant_sprites/orangebush_spritesheet.png");
		plant->sunlight_needed = 1.5f;
		break;
	case CACTUS:
		plant->plant_spritesheet = CP_Image_Load("Assets/plant_sprites/cactus_spritesheet.png");
		plant->sunlight_needed = 3.0f;
		break;
	default:
		plant->plant_spritesheet = CP_Image_Load("Assets/plant_sprites/berrybush_spritesheet.png");
		plant->sunlight_needed = 1.0f; //default
		break;
	}
	plant->growth_stage = SEED;
	plant->seed_type = seed_type;
	plant->sunlight_amount = 0;
}

//Draws the plant on top of the pot
void render_plant(Plant* plant, float posx, float posy, bool selected) {
#define PLANT_OFFSETY -26 //makes plant appear on top of pot
#define PLANT_OFFSETX 0
#define PLANT_WIDTH 19
#define PLANT_HEIGHT 19

	//sprites are in order (seed, seedling, youth, adult): growth_stage 0 == seed; x=0 etc.
	//y=0, non-highlighted, y=1, highlighted
	CP_Image_DrawSubImage(plant->plant_spritesheet, posx + PLANT_OFFSETX, posy + PLANT_OFFSETY,
		POT_WIDTH * POT_SCALE, POT_HEIGHT * POT_SCALE,
		PLANT_WIDTH * plant->growth_stage, PLANT_HEIGHT * selected,
		PLANT_WIDTH + PLANT_WIDTH * plant->growth_stage, PLANT_HEIGHT + PLANT_HEIGHT * selected, 255);

	//Increase sunlight bar
	if (plant->growth_stage != ADULT) {
		plant->sunlight_amount += dt / plant->sunlight_needed / 2.0f;
		plant->sunlight_amount = CP_Math_ClampFloat(plant->sunlight_amount, 0.0f, 1.0f);

		if (selected) {
			//SUNLIGHT AND WATER BAR HERE:
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
		plant->sunlight_needed *= 1.2f; //increase sunlight needed per growth stage
		plant->water_amount = 0;
	}
}


//Renders sunlight bar on top of selected/nearest plant
void render_sunlight_bar(Plant* plant, float posx, float posy) {
	//Bar background
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 120));
	CP_Graphics_DrawRect(posx, posy - 30.0f, 80.0f, 20.0f);

	//Sunlight bar
	CP_Settings_Fill(CP_Color_Create(255, 255, 0, 180));
	CP_Graphics_DrawRect(posx, posy - 30.0f, 80.0f * plant->sunlight_amount, 20.0f);
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
