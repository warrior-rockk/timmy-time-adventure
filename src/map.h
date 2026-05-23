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
    E_TILE_PROP_SOLID           = 0x0000,       //Solid tile (collidable)
    E_TILE_PROP_NO_SOLID        = 0x0001,       //No collision with this tile
    E_TILE_PROP_FRONT_LAYER     = 0x0002,       //Tile draws on front layer
    E_TILE_PROP_ANIMATION       = 0x0004,       //Tile has animation
    E_TILE_PROP_SLOPE_45        = 0x0008,       //Tile is slope 45º
    E_TILE_PROP_SLOPE_135       = 0x0010,       //Tile is slope 135º
    E_TILE_PROP_SLOPE_25        = 0x0020,       //Tile is slope 25º
    E_TILE_PROP_SLOPE_152       = 0x0040,       //Tile is slope 152º
    E_TILE_PROP_TOP_STAIR       = 0x0080,       //Tile is top of stairs
    E_TILE_PROP_STAIR           = 0x0100,       //Tile is stairs
    E_TILE_PROP_SOLID_ON_FALL   = 0x0200,       //Tile only solid falling into
    E_TILE_PROP_HURT            = 0x0400,       //Tile hurts player
    E_TILE_PROP_SLOPE_25_2      = 0x0800,       //Tile is slope 25º
    E_TILE_PROP_SLOPE_152_2     = 0x1000,       //Tile is slope 152º
};

//loads a map level and creates level entities
void map_load(char *mapFile, BITMAP *tileset, tVector screenSize);
//unload the map and free resources
void map_unload();

//get the dimensions of the map (on tiles)
tVector map_get_dimensions();
//get level time
uint8_t map_get_level_time();

//draw the current map
void map_draw(BITMAP *buffer, bool frontLayer);

//checks if position exists on tile maps
uint16_t map_tile_exists(tVector checkPosition);
//gets map tile property
uint16_t map_get_tile_property(tVector checkPosition);
//return x position snapped to tile
int16_t map_snap_x_to_tile(tEntity *entity);
//return y position snapped to tile
int16_t map_snap_y_to_tile(tEntity *entity);
//change map background color
void map_change_background_color(uint8_t color);
#endif
