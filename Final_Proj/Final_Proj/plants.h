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

void initialize_pots(Pot *pots, int size);
void render_pots(Pot* pots, int size, float player_posx, float player_posy);

void initialize_plant(Plant *plant);
void render_plant(Plant plant, float posx, float posy);
void grow_plant(GROWTH_STAGE *growth_stage);

#endif // !PLANT_H
