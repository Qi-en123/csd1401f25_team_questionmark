#ifndef PLANT_H
#define PLANT_H

#include "utils.h"
#include "cprocessing.h"

typedef enum SEED_TYPE {
	CABBAGE,
	CARROT,
	POTATO,
	EMPTY,
}SEED_TYPE;

typedef enum GROWTH_STAGE {
	SEED,
	SEEDLING,
	YOUTH,
	ADULT,
}GROWTH_STAGE;

typedef struct Plant {
	SEED_TYPE seed_type;
	float water_amount;
	float sunlight_amount;
	CP_Image plant_spritesheet; //plant growth stages
	GROWTH_STAGE growth_stage;
}Plant;

typedef struct Pot {
	Vector2 position;
	bool selected;
	bool filled;
	Plant plant;
}Pot;

void initialize_pots(Pot* pots, int size);
void render_pots(Pot* pots, CP_Image pot_spritesheet, int size, float player_posx, float player_posy, Pot* held_pot);
void render_held_pot(Pot* pot, CP_Image pot_spritesheet);


void initialize_plant(Plant* plant);
void render_plant(Plant plant, float posx, float posy);
void grow_plant(GROWTH_STAGE* growth_stage);

#endif // !PLANT_H
