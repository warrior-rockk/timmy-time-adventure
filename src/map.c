/********************************************************************
* Map System
*
* 11/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/

#include "map.h"

static tVector mapLimit;

static uint8_t map[MAP_TILE_H][MAP_TILE_W] =
{
    {22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22},
    {22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,0,16,17,17,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,26,22,22,22,22,23,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,22},
    {22,0,0,0,0,0,26,22,22,22,22,22,22,23,0,0,0,0,0,0,0,0,0,0,22,22,0,0,0,0,0,22},
    {22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22}
};

void map_load(tVector mapLimits)
{
    //load tiles
    /*tiles[0] = load_bmp("res/tiles/001.bmp", desktop_palette);
    tiles[1] = load_bmp("res/tiles/002.bmp", NULL);
    tiles[2] = load_bmp("res/tiles/003.bmp", NULL);*/
    
    //load map tileSheet
    BITMAP *mapTileSheet;
    mapTileSheet = load_bmp("res/tiles/tsheet.bmp", desktop_palette);
    for (uint8_t i = 0; i < NUM_TILES; i++)
    {
        tiles[i] = create_sub_bitmap(mapTileSheet, (i % TILES_ROW) * TILE_H, (int)(i / TILES_ROW) * TILE_W, TILE_W, TILE_H);
    }

    mapLimit = mapLimits;
}

void map_draw(BITMAP *buffer, tScroll *scroll)
{
    uint8_t tileNum;
    //int16_t lx = (GAME_W / TILE_W) + 1; //limit scroll x
    //int16_t ly = (GAME_H / TILE_H);     //limit scroll y
    int16_t sx = scroll->pos.x % TILE_W; //tile pos x on scroll
    int16_t sy = scroll->pos.y % TILE_H; //tile pos y on scroll
    int16_t tx = scroll->pos.x / TILE_W; //tile num x on scroll
    int16_t ty = scroll->pos.y / TILE_H; //tile num y on scroll   

    clear_to_color(buffer, 1);

    for (int y = 0; y < mapLimit.y; y++)
    {
        for (int x = 0; x < mapLimit.x; x++)        
        {
            tileNum = map[y + ty][x + tx];

            /* blit tile*/
            if (tileNum != 0)            
                draw_sprite(buffer, tiles[tileNum - 1], (x * 16) - sx, (y * 16) - sy);
        }    
    }
}

//checks if position exists on tile maps
uint16_t map_tile_exists(tVector *checkPosition)
{
	return false;
    //return (checkPosition->x / TILE_W) < level.numTilesX && (checkPosition->y / TILE_H) < level.numTilesY && checkPosition->x >= 0 && checkPosition->y >= 0;
}

//gets map tile code
uint16_t map_get_tile_code(tVector *checkPosition)
{
    return map[checkPosition->y / TILE_H][checkPosition->x / TILE_W];
}

//TODO: get pixel color of position's map
/*uint8_t map_get_pixel(tVector *checkPosition)
{
    getpixel(hud.hsImage, mouse_x, mouse_y - HUD_Y);    
}*/