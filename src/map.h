/********************************************************************
* Map System
*
* 11/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#ifndef _H_MAP_
#define _H_MAP_

#include "allegro.h"
#include "utils.h"
#include "game.h"
#include "scroll.h"

#define NUM_TILES       30
#define TILES_ROW       6

/*typedef struct tTile
{
    uint8_t tileCode;
    uint16_t tileFrame;    
} tTile;

typedef struct tMap
{
    tTile **tile;
} tMap;

enum E_TILE_CODE
{
    E_SOLID_TILE_CODE =  0x01,
};*/

void map_load();
tVector map_get_dimensions();
void map_draw(BITMAP *buffer, tScroll *scroll, tVector screenSize);

uint16_t map_tile_exists(tVector *checkPosition);
uint16_t map_get_tile_code(tVector *checkPosition);

#endif
