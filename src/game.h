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

#define PLAYER_ENTITY_ID    0

//game entity types
enum E_ENTITY_TYPE
{
    E_PLAYER_ENTITY_TYPE,
    E_OBJECT_ENTITY_TYPE,
};

//game states
enum E_GAME_STATE
{
    E_LOGO_GAME_STATE,
    E_INTRO_GAME_STATE,
    E_TITLE_GAME_STATE,
    E_LOAD_LEVEL_GAME_STATE,
    E_INIT_LEVEL_GAME_STATE,
    E_PLAY_LEVEL_GAME_STATE,
    E_DESTROY_LEVEL_GAME_STATE,
    E_EXIT_GAME_STATE,
};

//game structure
struct game
{
    uint8_t state;
    uint8_t prevState;
} game;

void game_init();
void game_draw();
void game_debug_info();
#endif