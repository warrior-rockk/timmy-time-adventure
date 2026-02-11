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
#define GAME_GFX_MODE           GFX_VGA
#define GAME_COLOR_DEPTH        8
#define SCREEN_X                320
#define SCREEN_Y                200

//game screen size (SMS resolution: 256x192 (testing 256x208: extra sms Y tile to center screen))
#define GAME_W                  256
#define GAME_H                  160 // 192
#define GAME_X                  (SCREEN_W>>1) - (GAME_W>>1)
#define GAME_Y                  0 //(SCREEN_H>>1) - (GAME_H>>1)
#define BORDER_COLOR            3

#define GAME_INI_LIVES          3
#define GAME_INI_LIFE           3

//clock iteration duration value
#define GAME_CLOCK_TICK     10      //10ms game tick

//position of player on entities array
#define PLAYER_ENTITY_ID    0

//game states
enum E_GAME_STATE
{
    E_GAME_ST_LOGO,
    E_GAME_ST_INTRO,
    E_GAME_ST_TITLE,
    E_GAME_ST_LOAD_LEVEL,
    E_GAME_ST_INIT,
    E_GAME_ST_PLAY_LEVEL,
    E_GAME_ST_LOSE_LIVE,
    E_GAME_ST_GAME_OVER,
    E_GAME_ST_DESTROY_LEVEL,
    E_GAME_ST_EXIT,
};

//game levels
enum E_GAME_LEVELS
{
    E_GAME_LEVEL_TEST,
    E_GAME_NUM_LEVELS,
};

//game data
typedef struct game
{
    uint8_t state;
    uint8_t prevState;
    uint8_t actualLevel;
    uint8_t lives;
    uint8_t life;
    uint8_t score;  
    uint8_t loseLive;  
} tGame;

//options for debug mode
typedef struct
{
    uint8_t showDebugInfo; 
    uint8_t stepByStep;
} tDebugOptions;

typedef struct
{ 
    char *mapFile;			//binary file of map and entities
    char *tileFile;		    //bitmap file for map tiles
    //char *musicFile;		//music file of level
} tLevelDataFile;

//sequence struct
typedef struct
{
    uint8_t step;               //step of sequence
    uint16_t timeCounter;       //time of sequence    
} tSequence;

extern tGame game;
extern double deltaTime;
extern uint8_t gameExit;
extern tDebugOptions debugOptions;

//function to init game
void game_init();
//function to updated game
void game_update();
//function to draw game
void game_draw();
//funtion to destroy game (unload resources)
void game_destroy();
#endif