/********************************************************************
* Game defines header
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#ifndef _H_GAME_
#define _H_GAME_

//game title and versioning
#define GAME_TITLE              "Timmy Time Adventure"
#define MAJOR_VERSION           0
#define MINOR_VERSION           1
#define CONFIG_FILE             "GAME.CFG"

//allegro engine
#define ALLEGRO_USES_TIMER      1   //disable to profile with uclock
#define ALLEGRO_USES_KEYBOARD   1
#define ALLEGRO_USES_MOUSE      0
#define ALLEGRO_USES_SOUND      1   //disable to profile with uclock

//screen video configuration
#define GAME_GFX_MODE           GFX_VGA
#define GAME_GFX_DOBLE_BUFFER   1
#define GAME_COLOR_DEPTH        8
#define SCREEN_X                320
#define SCREEN_Y                200

//game screen size (SMS resolution: 256x192 (testing 256x208: extra sms Y tile to center screen))
#define GAME_W                  256     //16 tiles width
#define GAME_H                  160     //10 tiles height
#define GAME_X                  (SCREEN_W>>1) - (GAME_W>>1)
#define GAME_Y                  0 //(SCREEN_H>>1) - (GAME_H>>1)

//colors
#define BLACK_COLOR             1
#define WHITE_COLOR             63
#define RED_COLOR               21
#define DIALOG_TEXT_COLOR       WHITE_COLOR
#define DIALOG_SEL_TEXT_COLOR   56

//clock iteration duration value
#define GAME_CLOCK_TICK         10      //10ms game tick

//general game definitions
#define GAME_INI_LIVES          6       //initial game lives
#define GAME_INI_LIFE           3       //initial game life
#ifdef DEBUGMODE
    #define GAME_FADE_SPEED         3       //general game fade speed (1: slowest 64: instantaneous)
#else
    #define GAME_FADE_SPEED         2       //general game fade speed (1: slowest 64: instantaneous)
#endif
#define GAME_DEFAULT_SCROLL_MODE    E_SCROLL_MODE_BY_WINDOW_Y_ONLY
#define GAME_INIT_LEVEL_DELAY   100     //time delay on start level
#define GAME_HURRY_TIME_LEFT    10      //time left to hurry up (time on red, speed up music...)
#define GAME_HURRY_MUSIC_TEMPO  160     //tempo of music when time running out

//HUD defines
#define HUD_POSITION_Y          166
#define HUD_POSITION_X          40

//tutorial defines
#define TUTORIAL_TXT_POSITION_X     SCREEN_W>>1
#define TUTORIAL_TXT_POSITION_Y     166
#define TUTORIAL_TXT_MAX_LENGTH     20
#define TUTORIAL_MSG_FRAME_OFFSET   5
#define TUTORIAL_INI_FRAME          0
#define TUTORIAL_MSG_1_FRAME        100
#define TUTORIAL_MSG_2_FRAME        TUTORIAL_MSG_1_FRAME + 200
#define TUTORIAL_MSG_3_FRAME        TUTORIAL_MSG_2_FRAME + 400
#define TUTORIAL_MSG_4_FRAME        TUTORIAL_MSG_3_FRAME + 300
#define TUTORIAL_MSG_5_FRAME        TUTORIAL_MSG_4_FRAME + 400
#define TUTORIAL_MSG_6_FRAME        TUTORIAL_MSG_5_FRAME + 400
#define TUTORIAL_MSG_7_FRAME        TUTORIAL_MSG_6_FRAME + 400
#define TUTORIAL_MSG_8_FRAME        TUTORIAL_MSG_7_FRAME + 400

//score points
#define SCORE_POINT_TIME_LEFT   1       //score for each time level second left
#define SCORE_POINT_HURT_ENEMY  10      //score when hurt enemy
#define SCORE_POINT_FULL_LIFE   50      //score when pick full life item but the life is full
#define SCORE_POINT_EXTRA_LIFE  500     //score needed to get extra life
#define SCORE_OVERFLOW          65000   //overflow score

//intro/ending
#define INTRO_SCENES            6       //number of intro scenes
#define ENDING_SCENES           3       //number of ending scenes
#define SCENE_IMAGE_DELAY       20      //time delay between text and image on scene
#define SCENE_CHAR_DELAY        8       //delay time by text character        

//dialog defines
#define FIRSTRUN_MENU_SIZE_X    80
#define FIRSTRUN_MENU_SIZE_Y    0
#define FIRSTRUN_MENU_POS_X     (SCREEN_W >> 1) - (FIRSTRUN_MENU_SIZE_X >> 1)
#define FIRSTRUN_MENU_POS_Y     50

#define MAIN_MENU_SIZE_X        120
#define MAIN_MENU_SIZE_Y        0
#define MAIN_MENU_POS_X         (SCREEN_W >> 1) - (MAIN_MENU_SIZE_X >> 1)
#define MAIN_MENU_POS_Y         120

#define OPTIONS_MENU_SIZE_X     160
#define OPTIONS_MENU_SIZE_Y     0
#define OPTIONS_MENU_POS_X      (SCREEN_W >> 1) - (OPTIONS_MENU_SIZE_X >> 1)
#define OPTIONS_MENU_POS_Y      50

#define GAMEOVER_MENU_SIZE_X    60
#define GAMEOVER_MENU_SIZE_Y    0
#define GAMEOVER_MENU_POS_X     (SCREEN_W >> 1) - (GAMEOVER_MENU_SIZE_X >> 1)
#define GAMEOVER_MENU_POS_Y     150

#define PLAY_MENU_SIZE_X        120
#define PLAY_MENU_SIZE_Y        0
#define PLAY_MENU_POS_X         (GAME_W >> 1) - (PLAY_MENU_SIZE_X >> 1)
#define PLAY_MENU_POS_Y         40

#define CONTROLS_MENU_SIZE_X    140
#define CONTROLS_MENU_SIZE_Y    0
#define CONTROLS_MENU_POS_X     (SCREEN_W >> 1) - (CONTROLS_MENU_SIZE_X >> 1)
#define CONTROLS_MENU_POS_Y     50

#define PAUSE_MENU_SIZE_X       140
#define PAUSE_MENU_SIZE_Y       0
#define PAUSE_MENU_POS_X        (GAME_W >> 1) - (PAUSE_MENU_SIZE_X >> 1)
#define PAUSE_MENU_POS_Y        GAME_H >> 1

//game states
enum E_GAME_STATE
{
    E_GAME_ST_LOGO,
    E_GAME_ST_DOS_LOGO,
    E_GAME_ST_FIRST_RUN_MENU,
    E_GAME_ST_INTRO,
    E_GAME_ST_TITLE,
    E_GAME_ST_MAIN_MENU,
    E_GAME_ST_OPTIONS_MENU,
    E_GAME_ST_TUTORIAL,
    E_GAME_ST_INIT,
    E_GAME_ST_SELECT_LEVEL,
    E_GAME_ST_LOAD_LEVEL,
    E_GAME_ST_INIT_LEVEL,
    E_GAME_ST_PLAY_LEVEL,
    E_GAME_ST_MOVE_TO_DOOR,
    E_GAME_ST_PAUSE_LEVEL,
    E_GAME_ST_PLAY_MENU,
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
    E_GAME_LEVEL_EGYPT,
    E_GAME_LEVEL_MEDIEVAL,
    E_GAME_LEVEL_WEST,               
    E_GAME_LEVEL_TUTORIAL,

    E_GAME_NUM_LEVELS,
};

//game keys
enum E_GAME_KEYS
{
    E_G_KEY_UP,
    E_G_KEY_DOWN,
    E_G_KEY_LEFT,
    E_G_KEY_RIGHT,
    E_G_KEY_JUMP,
    E_G_KEY_ACTION,
    E_G_KEY_PAUSE,
    E_G_KEY_EXIT,
    E_G_KEY_ENTER,
    E_G_KEY_D,
    E_G_KEY_S,
    E_G_KEY_I,
    E_G_KEY_T,
    E_GAME_KEYS_NUM,
};

enum E_GAME_TEXTS
{
    E_TXT_GAME_TITLE,
    E_TXT_PRESS_TO_START,
    E_TXT_MENU_PLAY,
    E_TXT_MENU_OPTIONS,
    E_TXT_MENU_EXIT,
    E_TXT_MENU_LANG,
    E_TXT_MENU_LANG_OPTIONS,
    E_TXT_MENU_CONTROLS,
    E_TXT_MENU_SFX_VOLUME,
    E_TXT_MENU_MUSIC_VOLUME,
    E_TXT_MENU_CONTINUE,
    E_TXT_MENU_EXIT_TO_TITLE,
    E_TXT_MENU_EXIT_TO_DOS,
    E_TXT_MENU_RESTART_LEVEL,
    E_TXT_PAUSE,
    E_TXT_HUD_LIVES,
    E_TXT_HUD_LIFE,
    E_TXT_HUD_SCORE,
    E_TXT_HUD_TIME,
    E_TXT_MENU_CTRL_UP,
    E_TXT_MENU_CTRL_DOWN,
    E_TXT_MENU_CTRL_LEFT,
    E_TXT_MENU_CTRL_RIGHT,
    E_TXT_MENU_CTRL_JUMP,
    E_TXT_MENU_CTRL_ACTION,
    E_TXT_MENU_PRESS_KEY,
    E_TXT_YES,
    E_TXT_NO,
    E_TXT_GAME_OVER,
    E_TXT_CONTINUE_QUESTION,
    E_TXT_SELECT_LEVEL,
    E_TXT_TUTORIAL_WELCOME,
    E_TXT_TUTORIAL_MOVE,
    E_TXT_TUTORIAL_JUMP,
    E_TXT_TUTORIAL_CROUCH,
    E_TXT_TUTORIAL_ATTACK,
    E_TXT_TUTORIAL_TAKE,
    E_TXT_TUTORIAL_THROW,
    E_TXT_TUTORIAL_SHORT_THROW,
    E_TXT_TUTORIAL_DOOR,
    E_TXT_INTRO_1,
    E_TXT_INTRO_2,
    E_TXT_INTRO_3,
    E_TXT_INTRO_4,
    E_TXT_INTRO_5,
    E_TXT_INTRO_6,
    E_TXT_ENDING_1,
    E_TXT_ENDING_2,
    E_TXT_ENDING_3,

    E_TXT_NUM,
};

enum E_TEXT_LANGUAGES
{    
    E_LANG_ENG,
    E_LANG_ESP,
    E_NUM_LANGS,
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
    E_SFX_OBJECT_VOICE,
    E_SFX_NUM_VOICES,
};

//game sfx
enum E_GAME_SFX
{
    E_SFX_GAME_POINT,
    E_SFX_GAME_POINT_END,
    E_SFX_GAME_MENU_NAV,
    E_SFX_GAME_MENU_SELECT,
    E_SFX_GAME_LEVEL_OK,
    E_SFX_GAME_LEVEL_BLUE,
    E_SFX_GAME_NUM,
};

//game fonts
enum E_GAME_FONTS
{
    E_GAME_FONT,
    E_GAME_FONT_BIG,
    E_GAME_FONT_MID,

    E_GAME_NUM_FONTS,
};

//fade types
enum E_FADE_TYPE
{
    E_FADE_TYPE_NONE,
    E_FADE_TYPE_NORMAL,
    E_FADE_TYPE_0_63,
    E_FADE_TYPE_64_255,
};

//game data
typedef struct
{
    uint8_t state;
    uint8_t prevState;
    uint8_t actualLevel;
    uint8_t lives;
    uint8_t life;
    uint8_t loseLive;
    uint16_t score;
    uint16_t time;    
    uint8_t fadeIn;
    uint8_t fadeOut;    
    uint8_t fadeState;
    uint8_t viewMap;
    uint8_t scrollMode;
    int8_t actualCompletedLevel;
    uint8_t levelComplete[E_GAME_NUM_LEVELS - 1];
    uint8_t doorId;
} tGame;

//options for debug mode
#ifdef DEBUGMODE
    #define DEBUG_TRACE_CREATE_ENTITIES     1
    #define DEBUG_TRACE_ENTITIES            0
    #define DEBUG_TRACE_COLL_POINTS_ARRAY   0
    #define DEBUG_NO_LEVEL_TIME             0
    #define DEBUG_TRACE_PLAYER              1
    #define DEBUG_LOCAL_DATA_MEMORY         0
    #define DEBUG_FPS_ON_CLOCK              1
    #define DEBUG_PROFILE                   0
    #define DEBUG_INI_GAME_STATE            E_GAME_ST_LOAD_LEVEL
    #define DEBUG_INI_GAME_LEVEL            E_GAME_LEVEL_JURASSIC

    typedef struct
    {
        uint8_t showDebugInfo; 
        uint8_t stepByStep;
        uint8_t invencible;
    } tDebugOptions;
#endif

typedef struct
{ 
    char *mapFile;			    //binary file of map and entities
    char *dataFile;             //datafile file for level
    uint8_t tileFileIndex;		//index of bitmap file for map tiles on datafile
    uint8_t palFileIndex;		//index of palette file for map tiles on datafile
    uint8_t musicFileIndex;		//index of music file of level on datafile
} tLevelData;

//sequence struct
typedef struct
{
    uint8_t step;               //step of sequence
    uint16_t timeCounter;       //time of sequence    
} tSequence;

extern tGame game;
extern uint8_t gameExit;

#ifdef DEBUGMODE
extern tDebugOptions debugOptions;
#endif

//function to load initial game resources
void game_load_resources();
//function to init game
void game_init();
//function to updated game
void game_update();
//function to draw game
void game_draw();
//function to destroy game (unload resources)
void game_destroy();
//function for print loading game progress
void game_loading_text();
#endif