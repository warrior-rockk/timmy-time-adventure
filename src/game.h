/********************************************************************
* Game defines header
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#ifndef _H_GAME_
#define _H_GAME_

//game title and versioning
#define GAME_TITLE              "Dos platform game"
#define MAJOR_VERSION           0
#define MINOR_VERSION           1

//allegro engine
#define ALLEGRO_USES_KEYBOARD   1
#define ALLEGRO_USES_MOUSE      0
#define ALLEGRO_USE_SOUND       0

//screen video dimensions
#define SCREEN_X        320
#define SCREEN_Y        200

//game screen size (SMS resolution: 256x192 (testing 256x208: extra sms Y tile to center screen))
#define GAME_W          256
#define GAME_H          208
#define GAME_X          (SCREEN_W>>1) - (GAME_W>>1)
#define GAME_Y          (SCREEN_H>>1) - (GAME_H>>1)

//clock iteration duration value
#define GAME_CLOCK_TICK     10      //10ms game tick

//position of player on entities array
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

//level structure
typedef struct tLevel
{
    uint16_t numTilesX;
    uint16_t numTilesY;
} tLevel;

extern double deltaTime;

void game_init();
void game_draw();
void game_debug_info();
#endif