#pragma once
#include "cprocessing.h" // for all CP function
#include "utils.h"
#define TILE_COLS 20
#define TILE_ROWS 15
//image
CP_Image spriteSheetTiles;
typedef struct Tile {
    float x;
    float y;
    int tile_index;
    bool placeable;
}Tile;
void draw_tile(int tile_index, float x, float y, float w, float h);
void draw_tile_map(Tile* map_tile);
void draw_modified_map(void);