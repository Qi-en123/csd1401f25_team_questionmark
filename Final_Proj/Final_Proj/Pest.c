#include "Pest.h"
#include <stdlib.h>
#include <math.h>

Pest pests[MAX_PESTS];

#define PLAYER_DETECT_RADIUS 100.0f // shorter detection
#define TOP_RIGHT_X 1080.0f
#define TOP_RIGHT_Y 20.0f
#define DESCEND_DISTANCE 100.0f    // how far down pests move initially
#define POT_AVOID_RADIUS 50.0f

// Initialize all pests as inactive
void initialize_pests(void) {
    for (int i = 0; i < MAX_PESTS; i++) {
        pests[i].state = PEST_INACTIVE;
    }
}

// Spawn pest at top-right, set descend target
void spawn_pest(Pot* pots, int potCount) {
    for (int i = 0; i < MAX_PESTS; i++) {
        if (pests[i].state == PEST_INACTIVE) {
            pests[i].state = PEST_DESCENDING;
            pests[i].position.x = TOP_RIGHT_X + 100;
            pests[i].position.y = TOP_RIGHT_Y;
            pests[i].speed = 500.0f + (rand() % 50);

            pests[i].descendTarget.x = TOP_RIGHT_X;
            pests[i].descendTarget.y = TOP_RIGHT_Y + DESCEND_DISTANCE;

            pests[i].targetPotIndex = rand() % potCount;
            pests[i].target = pots[pests[i].targetPotIndex].position;
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

void update_pests(Pot* pots, int potCount, Player* player) {
    for (int i = 0; i < MAX_PESTS; i++) {
        Pest* p = &pests[i];
        if (p->state == PEST_INACTIVE) continue;

        Vector2 dir = { 0, 0 };

        // Detect player
        if (distance(p->position, player->position) < PLAYER_DETECT_RADIUS) {
            p->state = PEST_FLEEING;
        }

        if (p->state == PEST_DESCENDING) {
            // Move downward first
            dir.x = p->descendTarget.x - p->position.x;
            dir.y = p->descendTarget.y - p->position.y;
            dir = normalize(dir);

            p->position.x += dir.x * p->speed * CP_System_GetDt();
            p->position.y += dir.y * p->speed * CP_System_GetDt();

            if (distance(p->position, p->descendTarget) < 5.0f) {
                p->state = PEST_CHASING;
            }
        }
        else if (p->state == PEST_CHASING) {
            dir.x = p->target.x - p->position.x;
            dir.y = p->target.y - p->position.y;
            dir = normalize(dir);

            p->position.x += dir.x * p->speed * CP_System_GetDt();
            p->position.y += dir.y * p->speed * CP_System_GetDt();

            if (distance(p->position, p->target) < 5.0f) {
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
            p->position.x += dir.x * p->speed * CP_System_GetDt();
            p->position.y += dir.y * p->speed * CP_System_GetDt();

            if (distance(p->position, (Vector2) { TOP_RIGHT_X, TOP_RIGHT_Y }) < 5.0f) {
                p->state = PEST_INACTIVE;
            }
        }
    }
}

// Draw pests
void render_pests(void) {
    CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
    for (int i = 0; i < MAX_PESTS; i++) {
        if (pests[i].state != PEST_INACTIVE) {
            CP_Graphics_DrawCircle(pests[i].position.x, pests[i].position.y, 50.0f);
        }
    }
}
