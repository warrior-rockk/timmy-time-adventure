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

//struct of tile
typedef struct
{
    uint8_t tileId;         //id of tile on tileset
    uint8_t tileProperty;   //property of tile    
} tTile;

//tile properties
enum E_TILE_PROPERTY
{
    E_TILE_PROP_NO_SOLID    = 0x01,
    E_TILE_PROP_FRONT_LAYER = 0x02,
};

//loads a map level and creates level entities
void map_load(char *mapFile, char *tileFile, tVector screenSize);
//unload the map and free resources
void map_unload();
//get the dimensions of the map (on tiles)
tVector map_get_dimensions();
//draw the current map
void map_draw(BITMAP *buffer, tScroll *scroll, bool frontLayer);
//check if tile position exists on map
uint16_t map_tile_exists(tVector *checkPosition);
//check the code of tile in map position
uint16_t map_get_tile_code(tVector *checkPosition);

#endif
