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

#define NUM_TILES       30  //TODO: load from tilesheet file
#define TILES_ROW       6   //TODO: fixed??

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

//loads a map level and creates level entities
void map_load(char *mapFile, char *tileFile);
//unload the map and free resources
void map_unload();
//get the dimensions of the map (on tiles)
tVector map_get_dimensions();
//draw the current map
void map_draw(BITMAP *buffer, tScroll *scroll, tVector screenSize);
//check if tile position exists on map
uint16_t map_tile_exists(tVector *checkPosition);
//check the code of tile in map position
uint16_t map_get_tile_code(tVector *checkPosition);

#endif
