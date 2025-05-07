/********************************************************************
* Game defines header
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#ifndef _H_GAME_
#define _H_GAME_

//SMS resolution: 256x192 (testing 256x208: extra sms Y tile to center screen)
#define SCREEN_X        320
#define SCREEN_Y        240

#define GAME_W          256
#define GAME_H          208
#define GAME_X          (SCREEN_W>>1) - (GAME_W>>1)
#define GAME_Y          (SCREEN_H>>1) - (GAME_H>>1)

#define NUM_TILES       3
#define TILE_W          16
#define TILE_H          16

#define MAP_TILE_W      (GAME_W / TILE_W) * 2
#define MAP_TILE_H      (GAME_H / TILE_H) * 1


#define PLAYER_ENTITY_ID    0

#endif