#pragma once
#include "cprocessing.h" // for all CP function
#include "utils.h"
#define TILE_COLS 20
#define TILE_ROWS 15
//image
CP_Image spritesheet_tiles;
typedef struct Tile {
    float x;
    float y;
    int tile_index;
    bool placeable;
    bool obstructed;
    bool walkable;
}Tile;
void draw_tile(int tile_index, float x, float y, float w, float h);
void draw_tile_map(Tile* map_tile);
void draw_modified_map(Tile* map_tile);
void set_tile_placeable(Tile* map_tile, int* placeable, int size);