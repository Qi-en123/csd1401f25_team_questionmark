#ifndef PEST_H
#define PEST_H

#include "cprocessing.h"
#include "plants.h"
#include "player.h" // needed for Player struct

typedef enum Pest_State {
    PEST_INACTIVE,
    PEST_DESCENDING, // new state: moves downward first
    PEST_CHASING,
    PEST_FLEEING
} Pest_State;

typedef struct Pest {
    Vector2 position;
    Vector2 target;       // target pot
    Vector2 descendTarget; // target for initial descent
    float speed;
    Pest_State state;
    int targetPotIndex;
} Pest;

#define MAX_PESTS 5
extern Pest pests[MAX_PESTS];

void initialize_pests(void);
void spawn_pest(Pot* pots, int potCount);
void update_pests(Pot* pots, int potCount, Player* player);
void render_pests(void);

#endif // PEST_H
