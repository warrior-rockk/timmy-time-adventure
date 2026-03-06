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
#include "anim.h"

//struct of tile
typedef struct
{
    uint8_t tileId;             //id of tile on tileset
    uint8_t tileProperty;       //property of tile   
    uint8_t tileAnimationId;    //id of array of tile animation 
} tTile;

//tile animation
typedef struct
{
    uint8_t tileId;
    uint8_t numFrames;
    tAnimation anim;
    tAnimFrame *frames;
} tTileAnimation;

//tile properties
enum E_TILE_PROPERTY
{
    E_TILE_PROP_NO_SOLID    = 0x01,
    E_TILE_PROP_FRONT_LAYER = 0x02,
    E_TILE_PROP_ANIMATION   = 0x04,
    E_TILE_PROP_SLOPE_45    = 0x08,
    E_TILE_PROP_SLOPE_135   = 0x10,
    E_TILE_PROP_NO_SCROLL_Y = 0x20,
    E_TILE_PROP_NO_SCROLL_X = 0x40,
    E_TILE_PROP_TOP_STAIR   = 0x80,
    //E_TILE_PROP_STAIR       = 0xFF,
};

//loads a map level and creates level entities
void map_load(char *mapFile, char *tileFile, tVector screenSize);
//unload the map and free resources
void map_unload();
//get the dimensions of the map (on tiles)
tVector map_get_dimensions();
//draw the current map
void map_draw(BITMAP *buffer, tScroll *scroll, bool frontLayer);
//checks if position exists on tile maps
uint16_t map_tile_exists(tVector checkPosition);
//gets map tile property
uint8_t map_get_tile_property(tVector checkPosition);

#endif
