#include "Pest.h"
#include <stdlib.h>
#include <math.h>
#include "cprocessing.h"

Pest pests[MAX_PESTS];
CP_Image pest_spritesheet; // set in Game_Init or elsewhere

#define PLAYER_DETECT_RADIUS 100.0f
#define TOP_RIGHT_X 1200.0f
#define TOP_RIGHT_Y 20.0f
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
    // Build a list of valid pots with plants
    int validIndices[MAX_PESTS];
    int validCount = 0;
    for (int i = 0; i < potCount; i++) {
        if (pots[i].filled && pots[i].plant.seed_type != EMPTY) {
            validIndices[validCount++] = i;
        }
    }

    if (validCount == 0) return; // No pots with plants, don't spawn

    for (int i = 0; i < MAX_PESTS; i++) {
        if (pests[i].state == PEST_INACTIVE) {
            pests[i].state = PEST_DESCENDING;
            pests[i].position.x = TOP_RIGHT_X + 100;
            pests[i].position.y = TOP_RIGHT_Y;
            pests[i].speed = 500.0f + (rand() % 50);

            pests[i].descendTarget.x = TOP_RIGHT_X;
            pests[i].descendTarget.y = TOP_RIGHT_Y + DESCEND_DISTANCE;

            // Pick a random valid pot
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
            dir.x = p->target.x - p->position.x;
            dir.y = p->target.y - p->position.y;
            dir = normalize(dir);

            p->position.x += dir.x * p->speed * dt;
            p->position.y += dir.y * p->speed * dt;

            // Check if reached target
            if (distance(p->position, p->target) < 5.0f) {
                // Only affect pots with plants
                Pot* targetPot = &pots[p->targetPotIndex];
                if (targetPot->filled && targetPot->plant.seed_type != EMPTY) {
                    kill_plant(targetPot); // reset the pot
                }
                p->state = PEST_INACTIVE;
            }
        }
        else if (p->state == PEST_FLEEING) {
            dir.x = TOP_RIGHT_X - p->position.x;
            dir.y = TOP_RIGHT_Y - p->position.y;

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

            if (distance(p->position, (Vector2) { TOP_RIGHT_X, TOP_RIGHT_Y }) < 5.0f)
                p->state = PEST_INACTIVE;
        }

        // Animation
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
            CP_Image_DrawSubImage(
                pest_spritesheet,
                p->position.x,
                p->position.y,
                PEST_WIDTH * SCALE,
                PEST_HEIGHT * SCALE,
                srcX,
                0.0f,
                PEST_WIDTH +srcX,
                PEST_HEIGHT,
                255
            );
        }
    }
}
