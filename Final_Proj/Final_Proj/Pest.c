#include "Pest.h"
#include <stdlib.h>
#include <math.h>
#include "cprocessing.h"
#include "plants.h"


Pest pests[MAX_PESTS];
CP_Image pest_spritesheet; // set in Game_Init or elsewhere

#define PLAYER_DETECT_RADIUS 100.0f
#define WINDOW_WIDTH CP_System_GetWindowWidth() //+ 200.0f
#define WINDOW_HEIGHT CP_System_GetWindowHeight()//20.0f
#define DESCEND_DISTANCE 270.0f
#define POT_AVOID_RADIUS 50.0f

// Sprite animation
#define PEST_WIDTH 19       // width of 1 frame
#define PEST_HEIGHT 19      // height of 1 frame
#define PEST_FRAMES 3       // total frames
#define FRAME_DURATION 0.3f // seconds per frame
#define SCALE 3             // scale factor on screen

// Initialize all pests as inactive
void initialize_pests(void) {
    for (int i = 0; i < MAX_PESTS; i++) {
        pests[i].state = PEST_INACTIVE;
        pests[i].current_frame = 0;
        pests[i].time_between_frame = 0.0f;
    }
}

// Spawn pest at top-right, set descend target
void spawn_pest(Pot* pots, int potCount) {
    // Build list of valid pots as you already do
    int validIndices[MAX_PESTS];
    int validCount = 0;
    for (int i = 0; i < potCount; i++) {
        if (pots[i].filled && pots[i].plant.seed_type != EMPTY && !pots[i].beingCarried) {
            validIndices[validCount++] = i;
        }
    }
    if (validCount == 0) return;

    for (int i = 0; i < MAX_PESTS; i++) {
        if (pests[i].state == PEST_INACTIVE) {
            pests[i].state = PEST_DESCENDING;

            // Pick a random side: 0 = right, 1 = bottom, 2 = left
            int side = rand() % 3;

            switch (side) {
            case 0: // right
                pests[i].position.x = WINDOW_WIDTH + 50; // just off-screen
                pests[i].position.y = rand() % WINDOW_HEIGHT; // random y
                break;
            case 1: // bottom
                pests[i].position.x = rand() % WINDOW_WIDTH; // random x
                pests[i].position.y = WINDOW_HEIGHT + 50; // just off-screen
                break;
            case 2: // left
                pests[i].position.x = -50; // just off-screen
                pests[i].position.y = rand() % WINDOW_HEIGHT; // random y
                break;
            }

            pests[i].speed = 200.0f + (rand() % 50);
            pests[i].rotation = 1;

            // Descend target can be somewhere near the center or random
            pests[i].descendTarget.x = WINDOW_WIDTH / 2;
            pests[i].descendTarget.y = WINDOW_HEIGHT / 2;

            // Pick a random valid pot as target
            int randomIndex = rand() % validCount;
            pests[i].targetPotIndex = validIndices[randomIndex];
            pests[i].target = pots[pests[i].targetPotIndex].position;

            pests[i].current_frame = 0;
            pests[i].time_between_frame = 0.0f;
            break;
        }
    }
}



// Helper: normalize vector
static Vector2 normalize(Vector2 v) {
    float len = sqrtf(v.x * v.x + v.y * v.y);
    if (len != 0) {
        v.x /= len;
        v.y /= len;
    }
    return v;
}

// Helper: distance
static float distance(Vector2 a, Vector2 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

// Returns true if pot is valid for pest to target
static bool is_pot_valid(Pot* pot) {
    return pot->filled && pot->plant.seed_type != EMPTY && !pot->beingCarried;
}

// Update pests
void update_pests(Pot* pots, int potCount, Player* player) {
    for (int i = 0; i < MAX_PESTS; i++) {
        Pest* p = &pests[i];
        if (p->state == PEST_INACTIVE) continue;

        Vector2 dir = { 0, 0 };

        // Detect player
        if (distance(p->position, player->position) < PLAYER_DETECT_RADIUS) {
            p->state = PEST_FLEEING;
        }

        // Movement
        if (p->state == PEST_DESCENDING) {
            dir.x = p->descendTarget.x - p->position.x;
            dir.y = p->descendTarget.y - p->position.y;
            dir = normalize(dir);

            p->position.x += dir.x * p->speed * dt;
            p->position.y += dir.y * p->speed * dt;

            if (distance(p->position, p->descendTarget) < 5.0f)
                p->state = PEST_CHASING;
        }
        else if (p->state == PEST_CHASING) {
            Pot* targetPot = &pots[p->targetPotIndex];

            // Check every frame if target is still valid (not carried, filled, has seed)
            if (!is_pot_valid(targetPot)) {
                int validIndices[MAX_PESTS];
                int validCount = 0;

                // Build list of valid pots
                for (int j = 0; j < potCount; j++) {
                    if (is_pot_valid(&pots[j])) {
                        validIndices[validCount++] = j;
                    }
                }

                if (validCount > 0) {
                    // Pick a new random valid pot
                    int randomIndex = rand() % validCount;
                    p->targetPotIndex = validIndices[randomIndex];
                    targetPot = &pots[p->targetPotIndex];
                }
                else {
                    // No valid pots left, flee
                    p->state = PEST_FLEEING;
                    continue;
                }
            }

            // Move toward the current valid targetPot
            dir.x = targetPot->position.x - p->position.x;
            dir.y = targetPot->position.y - p->position.y;
            dir = normalize(dir);

            p->position.x += dir.x * p->speed * dt;
            p->position.y += dir.y * p->speed * dt;

            // Reached pot
            if (distance(p->position, targetPot->position) < 5.0f) {
                kill_plant(targetPot);
                p->state = PEST_INACTIVE;
            }
        }
        else if (p->state == PEST_FLEEING) {
            dir.x = WINDOW_WIDTH - p->position.x;
            dir.y = WINDOW_HEIGHT - p->position.y;

            // Avoid pots
            for (int j = 0; j < potCount; j++) {
                Vector2 away;
                away.x = p->position.x - pots[j].position.x;
                away.y = p->position.y - pots[j].position.y;
                float dist = sqrtf(away.x * away.x + away.y * away.y);
                if (dist < POT_AVOID_RADIUS && dist != 0) {
                    away.x /= dist;
                    away.y /= dist;
                    dir.x += away.x;
                    dir.y += away.y;
                }
            }

            dir = normalize(dir);

            p->position.x += dir.x * p->speed * dt;
            p->position.y += dir.y * p->speed * dt;

            if (distance(p->position, (Vector2) { WINDOW_WIDTH, WINDOW_HEIGHT }) < 5.0f)
                p->state = PEST_INACTIVE;
        }

        // Flip sprite
        p->rotation = (dir.x < 0) ? -1 : 1;

        // Animate
        p->time_between_frame += dt;
        if (p->time_between_frame > FRAME_DURATION) {
            p->time_between_frame = 0.0f;
            p->current_frame = (p->current_frame + 1) % PEST_FRAMES;
        }
    }
}


// Render pests
void render_pests(void) {
    CP_Settings_ImageMode(CP_POSITION_CORNER);
    for (int i = 0; i < MAX_PESTS; i++) {
        Pest* p = &pests[i];
        if (p->state != PEST_INACTIVE) {
            float srcX = p->current_frame * PEST_WIDTH;

            //makes it so when sprite width is inverted, sprite appears on same location
            float imgX;
            imgX = (p->rotation < 0) ? p->position.x + PEST_WIDTH * SCALE : p->position.x;

            CP_Image_DrawSubImage(
                pest_spritesheet,
                imgX,
                p->position.y,
                PEST_WIDTH * SCALE * p->rotation,
                PEST_HEIGHT * SCALE,
                srcX,
                0.0f,
                PEST_WIDTH + srcX,
                PEST_HEIGHT,
                255
            );
        }
    }
}