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
#include "entity.h"

//struct of tile
typedef struct
{
    uint8_t tileId;             //id of tile on tileset    
    uint8_t tileAnimationId;    //id of array of tile animation 
    uint16_t tileProperty;       //property of tile   
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
    E_TILE_PROP_NO_SOLID        = 0x0001,
    E_TILE_PROP_FRONT_LAYER     = 0x0002,
    E_TILE_PROP_ANIMATION       = 0x0004,
    E_TILE_PROP_SLOPE_45        = 0x0008,
    E_TILE_PROP_SLOPE_135       = 0x0010,
    E_TILE_PROP_NO_SCROLL_Y     = 0x0020,
    E_TILE_PROP_NO_SCROLL_X     = 0x0040,
    E_TILE_PROP_TOP_STAIR       = 0x0080,
    E_TILE_PROP_STAIR           = 0x0100,
    E_TILE_PROP_SOLID_ON_FALL   = 0x0200,
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
uint16_t map_get_tile_property(tVector checkPosition);
//return x position snapped to tile
int16_t map_snap_x_to_tile(tEntity *entity);
//return y position snapped to tile
int16_t map_snap_y_to_tile(tEntity *entity);
#endif
