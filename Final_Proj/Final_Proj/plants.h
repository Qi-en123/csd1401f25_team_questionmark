#pragma once

#include "utils.h"
#include "cprocessing.h"
#include "map.h"

#define POT_WIDTH 17.0f
#define POT_HEIGHT 17.0f
#define POT_SCALE 4.0f
#define PLANT_WIDTH 19.0f
#define PLANT_HEIGHT 19.0f

typedef enum SEED_TYPE {
	BERRY_BUSH,
	CACTUS,
	ORANGE_BUSH,
	EMPTY,
}SEED_TYPE;

typedef enum GROWTH_STAGE {
	SEED, //0
	SEEDLING,
	YOUTH,
	ADULT, //3
}GROWTH_STAGE;

typedef struct Plant {
	SEED_TYPE seed_type;
	float water_amount;
	int water_needed; //0-4 (0 is the first bar)
	float sunlight_amount, sunlight_needed;
	CP_Image plant_spritesheet; //plant growth stages
	GROWTH_STAGE growth_stage;
	bool watered_once;
	float fill_progress;
}Plant;

typedef struct Pot {
	Vector2 position;
	bool selected;
	bool filled;
	Plant plant;
	Tile* occupied_tile; //pointer to tile that its occupying, used to de-occupy when picked up
	bool beingCarried;
}Pot;

//Initialize "size" number of pots for the level
//Assign pots with spawn position, tile occupied, plants, got soil or not
void initialize_pots(Pot* pots, int size, Tile* tile);

//Draws every pot except the one player is holding
void render_pots(Pot* pots, CP_Image pot_spritesheet, int size,
	float player_posx, float player_posy, Pot* held_pot, void* held_tool);

//Draws pot that player is holding
void render_held_pot(Pot* pot, CP_Image pot_spritesheet);

//Initializes plant for the pot (called after using SEED tool on empty pot)
void initialize_plant(Plant* plant, SEED_TYPE seed_type);

//Draws plant on top of pot (called in render_held_pot if pot's plant seed_type != EMPTY)
//Plant subimg based on plant growth stage
void render_plant(Plant* plant, float posx, float posy, bool selected);

//Increments plant growth stage up till 3
void grow_plant(Plant* plant);

//Draws sunlight bar/meter on top of plant
void render_sunlight_bar(Plant* plant, float posx, float posy);

//Destroys plant, reset plant values to default, seed_type to EMPTY
void kill_plant(Pot* pot);