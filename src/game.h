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
#define ALLEGRO_USES_TIMER      1
#define ALLEGRO_USES_KEYBOARD   1
#define ALLEGRO_USES_MOUSE      0
#define ALLEGRO_USES_SOUND      1

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

//colors
#define BLACK_COLOR             79
#define WHITE_COLOR             12

//clock iteration duration value
#define GAME_CLOCK_TICK     10      //10ms game tick

//general game definitions
#define GAME_INI_LIVES          2       //initial game lives
#define GAME_INI_LIFE           2       //initial game life
#define GAME_FADE_SPEED         2       //general game fade speed
#define GAME_DEAD_WAIT_TIME     200     //time to wait between player dead and reset/game over

//position of player on entities array
#define PLAYER_ENTITY_ID    0

//HUD defines
#define HUD_POSITION_Y          166
#define HUD_POSITION_X          40

//score points
#define SCORE_POINT_HURT_ENEMY  10
#define SCORE_POINT_EXTRA_LIFE  500
#define SCORE_OVERFLOW          65000

//game states
enum E_GAME_STATE
{
    E_GAME_ST_LOGO,
    E_GAME_ST_INTRO,
    E_GAME_ST_TITLE,
    E_GAME_ST_INIT,
    E_GAME_ST_LOAD_LEVEL,
    E_GAME_ST_INIT_LEVEL,
    E_GAME_ST_PLAY_LEVEL,
    E_GAME_ST_LOSE_LIVE,
    E_GAME_ST_COMPLETE_LEVEL,
    E_GAME_ST_GAME_OVER,
    E_GAME_ST_ENDING,
    E_GAME_ST_DESTROY_LEVEL,
    E_GAME_ST_EXIT,
};

//game levels
enum E_GAME_LEVELS
{    
    E_GAME_LEVEL_JURASSIC,    
    E_GAME_LEVEL_WEST,    
    E_GAME_LEVEL_TEST,    
    E_GAME_NUM_LEVELS,
};

//refresh HUD
enum E_REFRESH_HUD
{
    E_REFRESH_HUD_LIVES     = 0x01,
    E_REFRESH_HUD_LIFE      = 0x02,
    E_REFRESH_HUD_SCORE     = 0x04,   
    E_REFRESH_HUD_TIME      = 0x08,
    E_REFRESH_HUD_ALL       = 0xFF,
};

//fade states
enum E_FADE_STATE
{
    E_FADED_IN,
    E_FADED_OFF,
};

//game sfx voices
enum E_GAME_SFX_VOICES
{
    E_SFX_GAME_VOICE,
    E_SFX_PLAYER_VOICE,
    E_SFX_ENEMY_VOICE,
    E_SFX_NUM_VOICES,
};

//game data
typedef struct game
{
    uint8_t state;
    uint8_t prevState;
    uint8_t actualLevel;
    uint8_t lives;
    uint8_t life;
    uint8_t loseLive;
    uint8_t score;
    uint16_t time;    
    uint8_t fadeIn;
    uint8_t fadeOut;    
    uint8_t fadeState;
    uint8_t viewMap;
    uint8_t scrollMode;
} tGame;

//options for debug mode
#define DEBUG_TRACE_ENTITIES            0
#define DEBUG_TRACE_COLL_POINTS_ARRAY   0
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