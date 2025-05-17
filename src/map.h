/********************************************************************
* Map System
*
* 11/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#ifndef _H_MAP_
#define _H_MAP_

#include "allegro.h"
#include "globals.h"
#include "scroll.h"

#define NUM_TILES       3
#define TILE_W          16
#define TILE_H          16

#define MAP_TILE_W      (256 / TILE_W) * 2
#define MAP_TILE_H      (208 / TILE_H) * 1

typedef struct tTile
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
};

BITMAP *tiles[NUM_TILES];

void map_load(tVector mapLimits);
void map_draw(BITMAP *buffer, tScroll *scroll);

uint16_t map_tile_exists(tVector *checkPosition);
uint16_t map_get_tile_code(tVector *checkPosition);

#endif
