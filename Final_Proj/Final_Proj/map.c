#include "cprocessing.h" // for all CP function
#include "map.h"
int map[TILE_ROWS][TILE_COLS] = {

{6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
{6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 6, 6},
	{1,1,2,1,2,2,1,2,1,2,1,1,2,1,2,2,1,1,1,2},
	{2,1,1,1,2,1,2,2,1,2,1,1,2,2,1,2,1,1,2,1},
	{1,2,1,1,2,2,1,2,1,2,2,1,1,1,2,1,2,1,1,2},
	{2,2,1,2,1,1,2,1,1,2,1,1,1,2,1,2,2,1,2,1},
	{1,1,2,1,1,2,2,1,1,2,2,1,1,2,1,2,1,2,1,2},
	{2,1,2,1,1,1,1,2,2,1,1,2,2,1,2,1,1,1,2,1},
	{1,2,2,2,1,2,1,2,1,1,1,2,2,1,1,1,2,2,1,2},
	{2,1,1,1,2,2,1,1,2,1,1,2,1,1,2,2,1,2,1,2},
	{1,1,2,2,1,1,1,2,2,2,1,1,2,1,1,2,1,2,2,1},
	{2,1,1,2,2,1,2,1,1,2,2,1,1,2,1,1,2,1,1,2},
	{1,2,1,1,2,1,1,1,2,2,1,2,2,1,2,1,1,1,2,2},
	{2,2,1,1,1,2,1,2,1,1,2,2,1,1,1,2,1,2,2,1},
	{1,1,2,2,1,1,2,1,1,2,2,1,1,2,1,2,1,1,2,1}
};
int modified_map[TILE_ROWS][TILE_COLS] = {

{6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 7, 7, 5, 0, 0},
	{1, 2, 2, 1, 2, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 2, 1, 1, 1, 2},
	{2, 1, 1, 1, 2, 1, 2, 2, 1, 2, 1, 1, 2, 2, 1, 2, 1, 1, 2, 1},
	{1, 2, 1, 1, 2, 2, 1, 2, 1, 2, 2, 1, 1, 1, 2, 1, 2, 1, 1, 2},
	{2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 2, 2, 1, 2, 1},
	{1, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2},
	{2, 1, 2, 1, 1, 1, 1, 2, 2, 1, 1, 2, 2, 1, 2, 1, 1, 1, 2, 1},
	{1, 2, 2, 2, 1, 2, 1, 2, 1, 1, 1, 2, 2, 1, 1, 1, 2, 2, 1, 2},
	{2, 1, 1, 1, 2, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 1, 2, 2, 2},
	{1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 2, 1, 1, 2, 1, 2, 1, 1},
	{2, 1, 1, 2, 2, 1, 2, 1, 1, 2, 2, 1, 1, 2, 1, 1, 2, 1, 2, 2},
	{1, 1, 1, 2, 2, 1, 2, 2, 1, 2, 1, 1, 2, 2, 2, 1, 2, 2, 1, 2},
	{2, 2, 1, 1, 1, 2, 1, 2, 1, 1, 2, 2, 1, 1, 1, 2, 1, 2, 2, 1},
	{1, 1, 2, 2, 1, 1, 2, 1, 1, 2, 2, 1, 1, 2, 1, 2, 1, 1, 2, 1}
};
void draw_tile(int tile_index, float x, float y, float w, float h)
{
	// coordinates to draw each tile in spriteSheetTiles
	int left, top, right, bottom;

	switch (tile_index)
	{//16*16
	case 1: // normal green path
		left = 0; top = 0; right = 16; bottom = 16; break;

	case 2: // grass path
		left = 17; top = 0; right = 33; bottom = 16; break;

	case 3: // normal green path
		left = 169; top = 55; right = 182; bottom = 67; break;



	case 4: // left opening fence
		left = 139; top = 82; right = 152; bottom = 95; break;

	case 5: // right opening fence
		left = 151; top = 82; right = 164, bottom = 95; break;

	case 6: // dirt path
		left = 14; top = 26; right = 30; bottom = 42; break;

	case 7: // bottom dirt path
		left = 17; top = 48; right = 33, bottom = 64; break;


	default:
		// horizontal fence
		left = 145; top = 50; right = 158; bottom = 63; break;
	}

	// Shrink the rectangle by 1 pixel inside to avoid texture bleeding
	left += 1;
	top += 1;
	right -= 1;
	bottom -= 1;

	CP_Image_DrawSubImage(spriteSheetTiles,
		x, y, w, h,
		left, top, right, bottom,
		255);
}
void draw_tile_map(Tile* map_tile)
{
	// Get current window size
	float screen_w = CP_System_GetWindowWidth();
	float screen_h = CP_System_GetWindowHeight();

	// Calculate tile size so map fills the whole screen
	float tile_w = screen_w / TILE_COLS;
	float tile_h = screen_h / TILE_ROWS;

	// Loop through map and draw each tile
	for (int row = 0; row < TILE_ROWS; row++)
	{
		for (int col = 0; col < TILE_COLS; col++)
		{
			Tile* tile = &map_tile[row * TILE_COLS + col];
			tile->tile_index = map[row][col];

			// Use integer positions to avoid gaps
			tile->x = col * tile_w;
			tile->y = row * tile_h;

			draw_tile(tile->tile_index, tile->x, tile->y, tile_w, tile_h);
		}
	}
}
void draw_modified_map(void)
{
	float screen_w = CP_System_GetWindowWidth();
	float screen_h = CP_System_GetWindowHeight();
	float tile_w = screen_w / TILE_COLS;
	float tile_h = screen_h / TILE_ROWS;

	for (int row = 0; row < TILE_ROWS; row++)
	{
		for (int col = 0; col < TILE_COLS; col++)
		{
			int tile = modified_map[row][col];
			float x = col * tile_w;
			float y = row * tile_h;
			draw_tile(tile, x, y, tile_w, tile_h);
		}
	}
}
void set_tile_placeable(Tile* map_tile, int* placeable, int size) {
	for (int row = 0; row < TILE_ROWS; row++)
	{
		for (int col = 0; col < TILE_COLS; col++)
		{
			Tile* tile = &map_tile[row * TILE_COLS + col];
			for (int i = 0; i < size; ++i) {
				if (placeable[i] == tile->tile_index) {
					tile->placeable = true;
					continue;
				}
			}
		}
	}
}