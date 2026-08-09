/********************************************************************
* Game code
*
* 28/01/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "allegro.h"

#include "utils.h"
#include "game.h"
#include "timer.h"
#include "entity.h"
#include "scroll.h"
#include "map.h"
#include "collisions.h"
#include "player.h"
#include "objects.h"
#include "input.h"
#include "enemies.h"
#include "sound.h"
#include "resources.h"
#include "interface.h"
#include "lang.h"
#include "text.h"

#include "data/gdata.h"
#include "data/judata.h"
#include "data/wedata.h"
#include "data/medata.h"
#include "data/egydata.h"
#include "data/tutdata.h"

#define TRACE_FLAG  "[GAME]"

//debug option structure
#ifdef DEBUGMODE
    tDebugOptions debugOptions = {0};   
#endif

uint8_t gameExit = false;                   //flag to exit to main
bool firstRun = false;                      //flag to set if first run (to show language selection menu)
uint8_t loadingProgress = 0;
uint8_t sceneCounter = 0;                   //intro counter
BITMAP *buffer;                             //screen buffer
BITMAP *worldScreen;                        //map window buffer
FONT *gameFont[E_GAME_NUM_FONTS];           //game font array
SAMPLE *gameSfx[E_SFX_GAME_NUM];            //game sfx
MIDI* gameMusic;                            //current MIDI game music
MIDI* jingleMusic;                          //jingle MIDI music
DATAFILE_INDEX *gameDataIndex;              //game data index
tGame game;                                 //game structure
tSequence gameSeq;                          //game sequence
tLevelData levelData[E_GAME_NUM_LEVELS];    //level data
DATAFILE *levelDAT;                         //level datafile
tDialog gameDialog;                         //game dialog object

//palettes
RGB *currentPal;                            //current palette setted
RGB *introPal;                              //pal for intro sequence
RGB *gamePal;                               //palette of 64 persistent colors for menus/title/hud/player

BITMAP *gameSprite;                         //general use game sprite (title logo, game over animation...)
tAnimation animSprite;                      //general animation structure for game sprite

//game controls string array
char  *gameControlStrings[E_GAME_KEYS_NUM];
char *keyStrings[] = {"", "A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","0","1","2","3","4","5","6","7","8","9","0_PAD","1_PAD","2_PAD","3_PAD","4_PAD","5_PAD","6_PAD","7_PAD","8_PAD","9_PAD","F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12","ESC","TILDE","MINUS","EQUALS","BACKSPACE","TAB","OPENBRACE","CLOSEBRACE","ENTER","COLON","QUOTE","BACKSLASH","BACKSLASH2","COMMA","STOP","SLASH","SPACE","INSERT","DEL","HOME","END","PGUP","PGDN","LEFT","RIGHT","UP","DOWN","SLASH_PAD","ASTERISK","MINUS_PAD","PLUS_PAD","DEL_PAD","ENTER_PAD"};

struct hud
{
    BITMAP *hudImg;
    BITMAP *hudLifeOff;
    BITMAP *hudLifeOn;
    uint8_t last_lives;
    uint8_t last_life;
    uint8_t last_score;
    uint16_t last_time;
    uint8_t refresh;     
} hud;

//game configuration
struct gameConfig
{
    uint8_t lang;                           //game language
    uint8_t sfxVolume;                      //sfx volume (0..255)
    uint8_t musicVolume;                    //music volume (0..255)
    uint8_t gameKeys[E_G_KEY_ACTION + 1];   //configured game keys
} gameConfig;

static void game_load_level(uint8_t numLevel);
static void game_destroy_level();
static void game_update_level();
static void game_draw_level();
static void game_do_fade();
static void game_pause_sound();
static void game_resume_sound();
static void game_hud_init();
static void game_hud_update();
static void game_hud_draw();
static bool game_navigation_menu(tDialog *dialog, BITMAP *drawBuffer);
static void game_load_config();
static void game_save_config();
static void game_create_options_menu();
static void game_process_options_menu();
static void game_update_controls_menu(BITMAP *drawBuffer, uint8_t stepReturn);
static void game_create_options_play_menu();
static void game_load_control_strings();
static void game_init_flags();
static void game_draw_object(tVector pos, uint8_t dir, tVector size, uint8_t axis, tAnimation *anim, BITMAP *sprite, BITMAP *buffer);

#ifdef DEBUGMODE
static void game_debug_update();
static void game_debug_info();
#endif

void game_update()
{   
    input_keys_update();
  
    //update state
    game.prevState = game.state; 

    switch(game.state)
    {
        case E_GAME_ST_LOGO:
            switch (gameSeq.step)
            {
                case 0:
                    clear(buffer);
                    game.fadeOut = true;                                        
                    gameSeq.step++;
                break;                                
                case 1:
                    game.fadeIn = true;        
                    
                    BITMAP *logo = load_dat_bmp_indexed(gameDataIndex, WARCOM_BMP);
                    
                    draw_sprite(buffer, logo, (SCREEN_W>>1) - (logo->w>>1) + 20, (SCREEN_H>>1) - (logo->h>>1));    
                    destroy_bitmap(logo);
                    
                    textout_centre_ex(buffer, gameFont[E_GAME_FONT], "WARCOM SOFT 2026", SCREEN_W>>1, SCREEN_H - 16, WHITE_COLOR, 0);

                    gameMusic = load_dat_midi_indexed(gameDataIndex, WARCOM_MID);
                    music_play(gameMusic, false);
                                        
                    gameSeq.step++;
                break;
                case 2:
                    if (music_get_pos() < 0 || input_any_key_pressed())
                    {
                        game.state++;
                        gameSeq.timeCounter = 0;
                        gameSeq.step = 0;
                        game.fadeOut = true;
                        music_stop();
                    }
                break;
            }
        break;
        case E_GAME_ST_DOS_LOGO:
            switch (gameSeq.step)
            {
                case 0:
                    game.fadeIn = true;
                    clear_to_color(buffer, 188);
                                        
                    BITMAP *logo = load_dat_bmp_indexed(gameDataIndex, DOSCLUB_BMP);
                                        
                    draw_sprite(buffer, logo, (SCREEN_W>>1) - (logo->w>>1), (SCREEN_H>>1) - (logo->h>>1));    
                    destroy_bitmap(logo);   
                                        
                    gameMusic = load_dat_midi_indexed(gameDataIndex, DOSCLUB_MID);
                    music_play(gameMusic, false);
                    
                    gameSeq.step++;
                break;
                case 1:
                    if (music_get_pos() < 0 || input_any_key_pressed())
                    {
                        if (firstRun)
                            game.state = E_GAME_ST_FIRST_RUN_MENU;
                        else
                            game.state = E_GAME_ST_INTRO;
                        gameSeq.timeCounter = 0;
                        gameSeq.step = 0;                        
                        game.fadeOut = true;
                        music_stop();
                    }                    
                break;
            }
        break;
        case E_GAME_ST_FIRST_RUN_MENU:
            switch (gameSeq.step)
            {
                case 0: //create init language menu                    
                    gameDialog = dialog_create((tRectangle){(tVector){FIRSTRUN_MENU_POS_X, FIRSTRUN_MENU_POS_Y}, (tVector){FIRSTRUN_MENU_SIZE_X, FIRSTRUN_MENU_SIZE_Y}}, DIALOG_TEXT_COLOR, DIALOG_SEL_TEXT_COLOR, true);
                    
                    dialog_add_option(&gameDialog, "ENGLISH");
                    dialog_add_option(&gameDialog, "ESPAÑOL");

                    clear_to_color(buffer, BLACK_COLOR);
                    dialog_draw(&gameDialog, buffer);
                    
                    game.fadeIn = true;
                    gameSeq.step++;                 
                break;
                case 1: 
                    //process init menu
                    game_navigation_menu(&gameDialog, buffer);

                    if (input_key_down(E_G_KEY_ENTER))
                    {
                        sfx_play(gameSfx[E_SFX_GAME_MENU_SELECT], E_SFX_GAME_VOICE);
                        
                        //sets selected lang
                        gameConfig.lang = gameDialog.optionSelected;
                        lang_set(gameConfig.lang);
                        game_load_control_strings();
                        game_save_config();
                        
                        game.state = E_GAME_ST_INTRO;
                        game.fadeOut = true;
                        gameSeq.step = 0;
                        dialog_destroy(&gameDialog);                                                
                    }                 
                break;                
            }
        break;
        case E_GAME_ST_INTRO:
            BITMAP *intro;
            switch (gameSeq.step)
            {
                case 0: 
                    clear_to_color(buffer, BLACK_COLOR);
                    intro = load_dat_bmp_indexed(gameDataIndex, INTRO1_BMP + sceneCounter);
                    draw_sprite(buffer, intro, (SCREEN_W>>1) - (intro->w>>1), 0);    
                    destroy_bitmap(intro);

                    text_multiline_draw(buffer, gameFont[E_GAME_FONT_MID], lang_get_txt(E_TXT_INTRO_1 + sceneCounter), SCREEN_W>>1, 140, WHITE_COLOR, BLACK_COLOR);
                    
                    game.fadeIn = E_FADE_TYPE_0_63;

                    gameSeq.step++;
                break;
                case 1:
                    if (gameSeq.timeCounter >= 20)
                    {
                        gameSeq.timeCounter = 0;
                        game.fadeIn = E_FADE_TYPE_64_255;
                        gameSeq.step++;
                    }
                    else
                        gameSeq.timeCounter += clock_tick_get();
                break;
                case 2:
                    if (input_any_key_pressed())
                    {
                        sceneCounter++;
                        if (sceneCounter >= INTRO_SCENES || input_key_down(E_G_KEY_EXIT))
                        {
                            game.state = E_GAME_ST_TITLE;
                            currentPal = gamePal;
                            sceneCounter = 0;
                        }
                        gameSeq.step = 0;    
                        game.fadeOut = true;
                    }
                break;
            }
        break;
        case E_GAME_ST_TITLE:
            switch (gameSeq.step)
            {
                case 0:                                      
                    currentPal = load_dat_pal_indexed(gameDataIndex, TITLE_PAL);
                    clear_to_color(buffer, BLACK_COLOR);                                        
                    gameSeq.step++;      
                    
                    //play title music
                    jingleMusic = load_dat_midi_indexed(gameDataIndex, TITLE_MID);
                    music_play(jingleMusic, false);
                    
                case 1:
                    if (gameSeq.timeCounter >= 200)
                    {
                        game.state = E_GAME_ST_MAIN_MENU;
                        gameSeq.timeCounter = 0;
                        gameSeq.step = 0;    
                        //load title logo
                        gameSprite = load_dat_bmp_indexed(gameDataIndex, TITLE_BMP);
                        animSprite.frame = 0;
                    }
                    else
                        gameSeq.timeCounter += clock_tick_get();
                break;
            }
        break;
        case E_GAME_ST_MAIN_MENU:
            
            switch (gameSeq.step)
            {
                case 0: //create main menu dialog 
                    //draw title logo
                    play_animation(&animSprite, 0, 9, 16, ANIM_LOOP);
                    game_draw_object((tVector){SCREEN_W >> 1, SCREEN_H>>2}, E_ENT_DIR_RIGHT, (tVector){197,87}, E_ENT_AXIS_CENTER, &animSprite, gameSprite, buffer);   

                    gameDialog = dialog_create((tRectangle){(tVector){MAIN_MENU_POS_X, MAIN_MENU_POS_Y}, (tVector){MAIN_MENU_SIZE_X, MAIN_MENU_SIZE_Y}}, DIALOG_TEXT_COLOR, DIALOG_SEL_TEXT_COLOR, true);
                    dialog_add_option(&gameDialog, lang_get_txt(E_TXT_MENU_PLAY));
                    dialog_add_option(&gameDialog, lang_get_txt(E_TXT_MENU_OPTIONS));
                    dialog_add_option(&gameDialog, lang_get_txt(E_TXT_MENU_EXIT));

                    dialog_draw(&gameDialog, buffer);

                    
                    game.fadeIn = true;
                    gameSeq.step++;
                break;
                case 1: //process main menu dialog
                    play_animation(&animSprite, 0, 9, 16, ANIM_LOOP);
                    game_draw_object((tVector){SCREEN_W >> 1, SCREEN_H>>2}, E_ENT_DIR_RIGHT, (tVector){197,87}, E_ENT_AXIS_CENTER, &animSprite, gameSprite, buffer);
                
                    game_navigation_menu(&gameDialog, buffer);

                    if (input_key_down(E_G_KEY_ENTER))
                    {
                        sfx_play(gameSfx[E_SFX_GAME_MENU_SELECT], E_SFX_GAME_VOICE);

                        switch (gameDialog.optionSelected)
                        {
                            case 0: //PLAY
                                game.state = E_GAME_ST_INIT;
                                game.fadeOut = true;
                                gameSeq.step = 0;
                                dialog_destroy(&gameDialog);
                                destroy_bitmap(gameSprite);
                            break;
                            case 1: //OPTIONS
                                game.state = E_GAME_ST_OPTIONS_MENU;
                                gameSeq.step = 0;
                                dialog_destroy(&gameDialog);                                
                            break;
                            case 2: //EXIT
                                game.state = E_GAME_ST_EXIT;
                                gameSeq.step = 0;
                                game.fadeOut = true;
                                dialog_destroy(&gameDialog);                                
                                destroy_bitmap(gameSprite);
                            break;
                        }
                    }
                break;
            }    
        break;
        case E_GAME_ST_OPTIONS_MENU:
            switch (gameSeq.step)
            {
                case 0:
                    clear_to_color(buffer, 1);
                    gameDialog = dialog_create((tRectangle){(tVector){OPTIONS_MENU_POS_X, OPTIONS_MENU_POS_Y}, (tVector){OPTIONS_MENU_SIZE_X, OPTIONS_MENU_SIZE_Y}}, DIALOG_TEXT_COLOR, DIALOG_SEL_TEXT_COLOR, true);
                    
                    game_create_options_menu();
                    
                    dialog_draw(&gameDialog, buffer);
                    gameSeq.step++;
                break;
                case 1:
                    //if option changed
                    if (game_navigation_menu(&gameDialog, buffer))
                    {
                        game_process_options_menu();    
                    }

                    if (input_key_down(E_G_KEY_ENTER))
                    {
                        sfx_play(gameSfx[E_SFX_GAME_MENU_SELECT], E_SFX_GAME_VOICE);

                        switch (gameDialog.optionSelected)
                        {
                            case 1: //CONTROLS
                                gameSeq.step = 10;
                                dialog_destroy(&gameDialog);
                            break;
                            case 4: //EXIT
                                game.state = E_GAME_ST_MAIN_MENU;
                                gameSeq.step = 0;                                
                                dialog_destroy(&gameDialog);             
                                clear_to_color(buffer, 1);                   
                            break;
                        }
                    }

                    if (input_key_down(E_G_KEY_EXIT))
                    {
                        game.state = E_GAME_ST_MAIN_MENU;
                        gameSeq.step = 0;                                
                        dialog_destroy(&gameDialog);             
                        clear_to_color(buffer, 1);                      
                    }
                break;
                case 10:    //draw controls menu
                    clear_to_color(buffer, 1);
                    game_update_controls_menu(buffer, 0);
                break;
                case 11:    //select key
                case 12:    //redefine
                    game_update_controls_menu(buffer, 0);
                break;
            }    
        break;
        case E_GAME_ST_INIT:
            game.lives          = GAME_INI_LIVES;
            game.life           = GAME_INI_LIFE;
            game.score          = 0;
            game.loseLive       = false;
            game.actualLevel    = E_GAME_LEVEL_TUTORIAL;     
                        
            game.state = E_GAME_ST_LOAD_LEVEL;            
        break;
        case E_GAME_ST_SELECT_LEVEL:
            switch (gameSeq.step)
            {
                case 0: //draw timeline
                    game.fadeIn = true;
                    currentPal  = load_dat_pal_indexed(gameDataIndex, TIMELINE_PAL);
                    clear(buffer);
                                  
                    //draw timeline image
                    BITMAP *timeline = load_dat_bmp_indexed(gameDataIndex, TIMELINE_BMP);               
                    draw_sprite(buffer, timeline, (SCREEN_W>>1) - (timeline->w>>1), (SCREEN_H>>1) - (timeline->h>>1));    
                    destroy_bitmap(timeline);   
                    //draw text title
                    textout_centre_ex(buffer, gameFont[E_GAME_FONT_BIG], lang_get_txt(E_TXT_SELECT_LEVEL), SCREEN_W>>1, 20, 56, BLACK_COLOR);                    

                    //draw static image completed levels
                    for (uint8_t i = 0; i < E_GAME_NUM_LEVELS - 1; i++)
                    {
                        if (game.levelComplete[i])
                        {
                            switch (i)
                            {
                                case E_GAME_LEVEL_JURASSIC:
                                    gameSprite = load_dat_bmp_indexed(gameDataIndex, JUOK_BMP);
                                break;
                                case E_GAME_LEVEL_EGYPT:
                                    gameSprite = load_dat_bmp_indexed(gameDataIndex, EGOK_BMP);
                                break;
                                case E_GAME_LEVEL_MEDIEVAL:
                                    gameSprite = load_dat_bmp_indexed(gameDataIndex, MEOK_BMP);
                                break;
                                case E_GAME_LEVEL_WEST:
                                    gameSprite = load_dat_bmp_indexed(gameDataIndex, WEOK_BMP);
                                break;
                            }
                            animSprite.frame = 8;
                            game_draw_object((tVector){36 + (69 * i), 120}, E_ENT_DIR_RIGHT, (tVector){40,38}, E_ENT_AXIS_LEFT_DOWN, &animSprite, gameSprite, buffer);
                            destroy_bitmap(gameSprite);
                        }           
                    }
                    
                    //draw completed levels ring
                    BITMAP *completeRing = load_dat_bmp_indexed(gameDataIndex, RINGC_BMP);          
                    for (uint8_t i = 0; i < E_GAME_NUM_LEVELS - 1; i++)
                    {
                        if (game.levelComplete[i])
                            draw_sprite(buffer, completeRing, 32 + (69 * i), 78);    
                    }
                    destroy_bitmap(completeRing); 

                    //check recent completed level
                    if (!game.levelComplete[game.actualCompletedLevel])
                    {
                        //set level completed
                        game.levelComplete[game.actualCompletedLevel] = true;
                        //load animation sprite
                        switch (game.actualCompletedLevel)
                            {
                                case E_GAME_LEVEL_JURASSIC:
                                    gameSprite = load_dat_bmp_indexed(gameDataIndex, JUOK_BMP);
                                break;
                                case E_GAME_LEVEL_EGYPT:
                                    gameSprite = load_dat_bmp_indexed(gameDataIndex, EGOK_BMP);
                                break;
                                case E_GAME_LEVEL_MEDIEVAL:
                                    gameSprite = load_dat_bmp_indexed(gameDataIndex, MEOK_BMP);
                                break;
                                case E_GAME_LEVEL_WEST:
                                    gameSprite = load_dat_bmp_indexed(gameDataIndex, WEOK_BMP);
                                break;
                            }
                        animSprite.frame = -1;
                        gameSeq.step = 1;
                    }
                    else
                    {
                        gameSeq.step = 2;
                    }
                break;
                case 1: //level completed animation
                    //play animation sfx
                    if (animSprite.frame == -1)
                    {
                        sfx_play(gameSfx[E_SFX_GAME_LEVEL_OK], E_SFX_GAME_VOICE);
                        animSprite.frame = 0;
                    }

                    game_draw_object((tVector){36 + (69 * game.actualCompletedLevel), 120}, E_ENT_DIR_RIGHT, (tVector){40,38}, E_ENT_AXIS_LEFT_DOWN, &animSprite, gameSprite, buffer);
                    if (play_animation(&animSprite, 0, 8, 16, ANIM_ONCE))
                    {
                        game.actualCompletedLevel = -1;
                        destroy_bitmap(gameSprite);
                        
                        //check levels completed
                        uint8_t levelsCompleted = 0;
                        for (uint8_t i = 0; i < E_GAME_NUM_LEVELS - 1; i++)
                        {
                            if (game.levelComplete[i])
                                levelsCompleted++;
                        }
                        MY_TRACE_FLAG( "Levels completed: %i\n", levelsCompleted);

                        //jump to state
                        if (levelsCompleted == E_GAME_NUM_LEVELS - 1)
                        {
                            //draw completed levels ring
                            completeRing = load_dat_bmp_indexed(gameDataIndex, RINGC_BMP);          
                            for (uint8_t i = 0; i < E_GAME_NUM_LEVELS - 1; i++)
                            {
                                if (game.levelComplete[i])
                                    draw_sprite(buffer, completeRing, 32 + (69 * i), 78);    
                            }
                            destroy_bitmap(completeRing); 

                            gameSeq.step = 4;   //wait for ending
                        }
                        else
                            gameSeq.step++;
                        sfx_play(gameSfx[E_SFX_GAME_LEVEL_BLUE], E_SFX_GAME_VOICE);
                    }
                break;
                case 2: //draw ring and selection cursor
                    //draw select levelcursor and ring
                    BITMAP *cursor = load_dat_bmp_indexed(gameDataIndex, SELECT_BMP);               
                    draw_sprite(buffer, cursor, 47 + (69 * game.actualLevel), 130);    
                    destroy_bitmap(cursor);   

                    BITMAP *ring = load_dat_bmp_indexed(gameDataIndex, RING_BMP);               
                    draw_sprite(buffer, ring, 32 + (69 * game.actualLevel), 78);    
                    destroy_bitmap(ring);   

                    //draw completed levels ring
                    completeRing = load_dat_bmp_indexed(gameDataIndex, RINGC_BMP);          
                    for (uint8_t i = 0; i < E_GAME_NUM_LEVELS - 1; i++)
                    {
                        if (game.levelComplete[i])
                            draw_sprite(buffer, completeRing, 32 + (69 * i), 78);    
                    }
                    destroy_bitmap(completeRing); 

                    gameSeq.step++;
                break;
                case 3: //handle selection
                    if (input_key_down(E_G_KEY_RIGHT) && game.actualLevel < E_GAME_NUM_LEVELS - 2)
                    {
                        sfx_play(gameSfx[E_SFX_GAME_MENU_NAV], E_SFX_GAME_VOICE);
                        game.actualLevel++;
                        gameSeq.step = 0;
                    }
                    if (input_key_down(E_G_KEY_LEFT) && game.actualLevel > 0)
                    {
                        sfx_play(gameSfx[E_SFX_GAME_MENU_NAV], E_SFX_GAME_VOICE);
                        game.actualLevel--;
                        gameSeq.step = 0;
                    }
                    if (input_key_down(E_G_KEY_ENTER) && !game.levelComplete[game.actualLevel])
                    {
                        sfx_play(gameSfx[E_SFX_GAME_MENU_SELECT], E_SFX_GAME_VOICE);
                        gameSeq.step = 0;
                        game.state = E_GAME_ST_LOAD_LEVEL;
                        game.fadeOut = true;
                    }
                    if (input_key_down(E_G_KEY_EXIT))
                    {
                        gameSeq.step = 0;
                        game.state = E_GAME_ST_TITLE;
                        game.fadeOut = true;
                    }
                break;
                case 4: //wait for ending
                    if (gameSeq.timeCounter >= 160)
                    {
                        gameSeq.step = 0;
                        gameSeq.timeCounter = 0;
                        game.fadeOut = true;
                        game.state = E_GAME_ST_ENDING;
                    }
                    else
                        gameSeq.timeCounter += clock_tick_get();
                break;
            }
        break;
        case E_GAME_ST_LOAD_LEVEL:
            clear_to_color(buffer, BLACK_COLOR);
            
            object_system_init();
            enemy_system_init();

            game_load_level(game.actualLevel);
            
            if (game.actualLevel != E_GAME_LEVEL_TUTORIAL)
                game.state = E_GAME_ST_INIT_LEVEL;
            else
                game.state = E_GAME_ST_TUTORIAL;
        break;        
        case E_GAME_ST_INIT_LEVEL:
            switch (gameSeq.step)
            {
                case 0: //Init level data and entities
                    game.life       = GAME_INI_LIFE;            
                    game.loseLive   = false;            
                    game.viewMap    = true;
                    collision_set_player_platform_id(-1);
                    scroll_set_scroll_mode(GAME_DEFAULT_SCROLL_MODE);
                    //init level time from map
                    game.time = map_get_level_time();

                    entities_init();
                    //MY_TRACE_FLAG("player on x %i y %i\n", entity_get(entity_get_player_id())->pos.x, entity_get(entity_get_player_id())->pos.y);
                    scroll_init(entity_get(entity_get_player_id())->pos);
                    //MY_TRACE_FLAG("Scroll x %i y %i\n", scroll_get_position().x, scroll_get_position().y);
                    scroll_update(entity_get(entity_get_player_id())->pos);
                    //MY_TRACE_FLAG("1Scroll x %i y %i\n", scroll_get_position().x, scroll_get_position().y);
                    game_hud_init();                    

                    game_update_level();
                    entities_update();  //second update after update scroll
                    game_hud_update();
                    //MY_TRACE_FLAG("2Scroll x %i y %i\n", scroll_get_position().x, scroll_get_position().y);
                    
                    music_play(gameMusic, true);
                    
                    gameSeq.step++;
                break;                
                case 1: //level start delay
                    game.fadeIn = true;

                    if (gameSeq.timeCounter >= GAME_INIT_LEVEL_DELAY)
                    {
                        game.state = E_GAME_ST_PLAY_LEVEL;                        
                        gameSeq.step = 0;
                        gameSeq.timeCounter = 0;                        
                    }
                    else
                    {
                        game_draw_level();  
                        game_hud_draw();    
                        gameSeq.timeCounter += clock_tick_get();
                    }
                break;                
            }
        break;
        case E_GAME_ST_TUTORIAL:            
            switch (gameSeq.step)
            {
                case 0: //Init tutorial
                    game.life       = GAME_INI_LIFE;            
                    game.loseLive   = false;            
                    game.viewMap    = true;
                    collision_set_player_platform_id(-1);
                    scroll_set_scroll_mode(GAME_DEFAULT_SCROLL_MODE);
                    //init level time from map
                    game.time = map_get_level_time();

                    entities_init();
                    scroll_init(entity_get(entity_get_player_id())->pos);
                    
                    game_update_level();
                    game_draw_level(); 
                    music_play(gameMusic, true);
                    //start tutorial record
                    input_log_play("tutorial.rec");
                    //input_log_record("tutorial.rec");
                    game.fadeIn = true;
                    gameSeq.step++;
                break;                
                case 1: //play tutorial
                    if (input_log_play_finished() || key[KEY_ESC])
                    {
                        gameSeq.step++;
                        input_log_stop();
                    }
                    else
                    {               
                        switch (input_log_get_frame())
                        {
                            case TUTORIAL_INI_FRAME ... TUTORIAL_INI_FRAME + TUTORIAL_MSG_FRAME_OFFSET:
                                rectfill(buffer, 0, GAME_H, SCREEN_W, SCREEN_H, BLACK_COLOR);
                                textprintf_centre_ex(buffer, gameFont[E_GAME_FONT], TUTORIAL_TXT_POSITION_X, TUTORIAL_TXT_POSITION_Y, WHITE_COLOR, BLACK_COLOR, lang_get_txt(E_TXT_TUTORIAL_WELCOME));
                            break;
                            case TUTORIAL_MSG_1_FRAME ... TUTORIAL_MSG_1_FRAME + TUTORIAL_MSG_FRAME_OFFSET:
                                rectfill(buffer, 0, GAME_H, SCREEN_W, SCREEN_H, BLACK_COLOR);
                                textprintf_centre_ex(buffer, gameFont[E_GAME_FONT], TUTORIAL_TXT_POSITION_X, TUTORIAL_TXT_POSITION_Y, WHITE_COLOR, BLACK_COLOR, lang_get_txt(E_TXT_TUTORIAL_MOVE), keyStrings[input_get_defined_key(E_G_KEY_LEFT)], keyStrings[input_get_defined_key(E_G_KEY_RIGHT)]);
                            break;
                            case TUTORIAL_MSG_2_FRAME ... TUTORIAL_MSG_2_FRAME + TUTORIAL_MSG_FRAME_OFFSET:
                                rectfill(buffer, 0, GAME_H, SCREEN_W, SCREEN_H, BLACK_COLOR);
                                textprintf_centre_ex(buffer, gameFont[E_GAME_FONT], TUTORIAL_TXT_POSITION_X, TUTORIAL_TXT_POSITION_Y, WHITE_COLOR, BLACK_COLOR, lang_get_txt(E_TXT_TUTORIAL_JUMP), keyStrings[input_get_defined_key(E_G_KEY_JUMP)]);
                            break;
                            case TUTORIAL_MSG_3_FRAME ... TUTORIAL_MSG_3_FRAME + TUTORIAL_MSG_FRAME_OFFSET:
                                rectfill(buffer, 0, GAME_H, SCREEN_W, SCREEN_H, BLACK_COLOR);
                                textprintf_centre_ex(buffer, gameFont[E_GAME_FONT], TUTORIAL_TXT_POSITION_X, TUTORIAL_TXT_POSITION_Y, WHITE_COLOR, BLACK_COLOR, lang_get_txt(E_TXT_TUTORIAL_CROUCH), keyStrings[input_get_defined_key(E_G_KEY_DOWN)]);
                            break;
                            case TUTORIAL_MSG_4_FRAME ... TUTORIAL_MSG_4_FRAME + TUTORIAL_MSG_FRAME_OFFSET:
                                rectfill(buffer, 0, GAME_H, SCREEN_W, SCREEN_H, BLACK_COLOR);
                                textprintf_centre_ex(buffer, gameFont[E_GAME_FONT], TUTORIAL_TXT_POSITION_X, TUTORIAL_TXT_POSITION_Y, WHITE_COLOR, BLACK_COLOR, lang_get_txt(E_TXT_TUTORIAL_ATTACK), keyStrings[input_get_defined_key(E_G_KEY_ACTION)]);
                            break;
                            case TUTORIAL_MSG_5_FRAME ... TUTORIAL_MSG_5_FRAME + TUTORIAL_MSG_FRAME_OFFSET:
                                rectfill(buffer, 0, GAME_H, SCREEN_W, SCREEN_H, BLACK_COLOR);
                                textprintf_centre_ex(buffer, gameFont[E_GAME_FONT], TUTORIAL_TXT_POSITION_X, TUTORIAL_TXT_POSITION_Y, WHITE_COLOR, BLACK_COLOR, lang_get_txt(E_TXT_TUTORIAL_TAKE), keyStrings[input_get_defined_key(E_G_KEY_ACTION)]);
                            break;
                            case TUTORIAL_MSG_6_FRAME ... TUTORIAL_MSG_6_FRAME + TUTORIAL_MSG_FRAME_OFFSET:
                                rectfill(buffer, 0, GAME_H, SCREEN_W, SCREEN_H, BLACK_COLOR);
                                textprintf_centre_ex(buffer, gameFont[E_GAME_FONT], TUTORIAL_TXT_POSITION_X, TUTORIAL_TXT_POSITION_Y, WHITE_COLOR, BLACK_COLOR, lang_get_txt(E_TXT_TUTORIAL_THROW), keyStrings[input_get_defined_key(E_G_KEY_ACTION)]);
                            break;
                            case TUTORIAL_MSG_7_FRAME ... TUTORIAL_MSG_7_FRAME + TUTORIAL_MSG_FRAME_OFFSET:
                                rectfill(buffer, 0, GAME_H, SCREEN_W, SCREEN_H, BLACK_COLOR);
                                textprintf_centre_ex(buffer, gameFont[E_GAME_FONT], TUTORIAL_TXT_POSITION_X, TUTORIAL_TXT_POSITION_Y, WHITE_COLOR, BLACK_COLOR, lang_get_txt(E_TXT_TUTORIAL_SHORT_THROW), keyStrings[input_get_defined_key(E_G_KEY_DOWN)]);
                            break;
                            case TUTORIAL_MSG_8_FRAME ... TUTORIAL_MSG_8_FRAME + TUTORIAL_MSG_FRAME_OFFSET:
                                rectfill(buffer, 0, GAME_H, SCREEN_W, SCREEN_H, BLACK_COLOR);
                                textprintf_centre_ex(buffer, gameFont[E_GAME_FONT], TUTORIAL_TXT_POSITION_X, TUTORIAL_TXT_POSITION_Y, WHITE_COLOR, BLACK_COLOR, lang_get_txt(E_TXT_TUTORIAL_DOOR), keyStrings[input_get_defined_key(E_G_KEY_UP)]);
                            break;
                        }
                    }

                    game_update_level();
                    game_draw_level();
                break;   
                case 2: //fade out
                    game.fadeOut = true;
                    gameSeq.step++;
                break;      
                case 3: //jump to new state
                    game_destroy_level();
                    gameSeq.step = 0;
                    game.actualLevel = 0;
                    game.state = E_GAME_ST_SELECT_LEVEL;
                break;       
            }
        break;
        case E_GAME_ST_PLAY_LEVEL:            
            game_update_level();
            game_hud_update();

            game_draw_level();
            game_hud_draw();

            //check game lose life
            if (game.loseLive)
            {                
                game.lives--;                
                music_stop(gameMusic);
                game.state = E_GAME_ST_LOSE_LIVE;                
            }            

            //game time
            if (game.time <= 0)
                game.loseLive = true;
            else if (clock_tick_1sec_get())
            {
                game.time--;
                #ifdef DEBUGMODE
                    #if DEBUG_NO_LEVEL_TIME
                        game.time++;
                    #endif
                #endif
                //speed up music on 10 seconds left
                if (game.time == GAME_HURRY_TIME_LEFT)
                    music_set_tempo(GAME_HURRY_MUSIC_TEMPO);
                if (game.time <= GAME_HURRY_TIME_LEFT)
                    sfx_play(gameSfx[E_SFX_GAME_POINT], E_SFX_GAME_VOICE);
            }
            
            if (input_key_down(E_G_KEY_PAUSE))
                game.state = E_GAME_ST_PAUSE_LEVEL;

            if (game.actualCompletedLevel != -1)
                game.state = E_GAME_ST_COMPLETE_LEVEL;
            
            if (input_key_down(E_G_KEY_EXIT))
                    game.state = E_GAME_ST_PLAY_MENU;            
            
            if (game.doorId != 0)
            {
                game.fadeOut = true;
                game.state = E_GAME_ST_MOVE_TO_DOOR;                
            }

            #ifdef DEBUGMODE
                if (key[KEY_R] && (key_shifts & KB_CTRL_FLAG))
                    game.state = E_GAME_ST_INIT_LEVEL;
                
                if (key[KEY_C] && (key_shifts & KB_CTRL_FLAG))
                    game.actualCompletedLevel = game.actualLevel;
            #endif
        break;
        case E_GAME_ST_MOVE_TO_DOOR:
            //search door-out id                     
            tEntity *searchEntity;
            for (uint8_t i = 0; i < entities_get_num(); i++)
            {
                searchEntity = entity_get(i);
                //if it's door out object with spare equal to door-id
                if (searchEntity->entClass == E_ENT_CLASS_TRIGGER && searchEntity->entType == E_DOOR_OUT_OBJECT_TYPE && searchEntity->spare == game.doorId)
                {
                    //set player to door position and stops
                    tEntity *player = entity_get(entity_get_player_id());
                    player->fixPos = searchEntity->fixPos;
                    player->pos = searchEntity->pos;
                    player->fixVel = (tFixVector){0, 0};
                    player->dir = searchEntity->dir;
                    //reinit scroll and update-draw level
                    scroll_init(entity_get(entity_get_player_id())->pos);
                    game_update_level();
                    scroll_init(entity_get(entity_get_player_id())->pos);
                    game_update_level();
                    entities_update();  //second update after update scroll
                    game_hud_update();

                    game_draw_level();
                    game_hud_draw();
                }
            }
            //return to play
            game.fadeIn = true;
            game.state = E_GAME_ST_PLAY_LEVEL;
            game.doorId = 0;
        break;
        case E_GAME_ST_PAUSE_LEVEL:            
            switch (gameSeq.step)
            {
                case 0:
                    game_pause_sound();
            
                    //create pause dialog
                    gameDialog = dialog_create((tRectangle){(tVector){PAUSE_MENU_POS_X, PAUSE_MENU_POS_Y}, (tVector){PAUSE_MENU_SIZE_X, PAUSE_MENU_SIZE_Y}}, DIALOG_TEXT_COLOR, DIALOG_SEL_TEXT_COLOR, true);
                    dialog_add_text(&gameDialog, lang_get_txt(E_TXT_PAUSE));
                    dialog_draw(&gameDialog, worldScreen);

                    gameSeq.step++;
                break;
                case 1:
                    if (input_key_down(E_G_KEY_PAUSE))
                    {
                        gameSeq.step = 0;
                        game.state = E_GAME_ST_PLAY_LEVEL;
                        dialog_destroy(&gameDialog);
                        game_resume_sound();
                    }
                break;
            }
        break;
        case E_GAME_ST_PLAY_MENU:            
            switch (gameSeq.step)
            {
                case 0:
                    game_pause_sound();
            
                    //create play menu dialog
                    game_create_options_play_menu();
                    dialog_draw(&gameDialog, worldScreen);

                    gameSeq.step++;
                break;
                case 1:
                    game_navigation_menu(&gameDialog, worldScreen);
                    
                    if (input_key_down(E_G_KEY_ENTER))
                    {
                        sfx_play(gameSfx[E_SFX_GAME_MENU_SELECT], E_SFX_GAME_VOICE);

                        switch (gameDialog.optionSelected)
                        {
                            case 0: //CONTINUE
                                game.state = E_GAME_ST_PLAY_LEVEL;
                                game_resume_sound();

                                gameSeq.step = 0;
                                dialog_destroy(&gameDialog);
                            break;
                            case 1: //OPTIONS
                                gameSeq.step++;
                                dialog_destroy(&gameDialog);
                            break;
                            case 2: //RESTART LEVEL
                                game.fadeOut = true; 
                                gameSeq.step = 0;
                                game.state = E_GAME_ST_INIT_LEVEL;
                                dialog_destroy(&gameDialog);    
                            break;
                            case 3: //EXIT TO TITLE
                                game_destroy_level();                                
                                game_init_flags();
                                game.fadeOut = true;
                                game.state = E_GAME_ST_TITLE;
                                
                                gameSeq.step = 0;                               
                                dialog_destroy(&gameDialog); 
                            break;
                            case 4: //EXIT TO DOS
                                game_destroy_level(); 
                                game.state = E_GAME_ST_EXIT;
                                game.fadeOut = true;

                                gameSeq.step = 0;
                                dialog_destroy(&gameDialog);                                
                            break;
                        }
                    }

                    if (input_key_down(E_G_KEY_EXIT))
                    {
                        gameSeq.step = 0;
                        game.state = E_GAME_ST_PLAY_LEVEL;
                        dialog_destroy(&gameDialog);
                        game_resume_sound();
                    }
                break;
                case 2: //options menu    
                    gameDialog = dialog_create((tRectangle){(tVector){OPTIONS_MENU_POS_X, OPTIONS_MENU_POS_Y}, (tVector){OPTIONS_MENU_SIZE_X, OPTIONS_MENU_SIZE_Y}}, DIALOG_TEXT_COLOR, DIALOG_SEL_TEXT_COLOR, true);
                    
                    game_create_options_menu();
                    
                    //game_draw_level();
                    dialog_draw(&gameDialog, worldScreen);
                    gameSeq.step++;
                break;
                case 3:
                    //if option changed
                    if (game_navigation_menu(&gameDialog, worldScreen))
                    {
                        game_process_options_menu();    
                    }

                    if (input_key_down(E_G_KEY_ENTER))
                    {
                        sfx_play(gameSfx[E_SFX_GAME_MENU_SELECT], E_SFX_GAME_VOICE);

                        switch (gameDialog.optionSelected)
                        {
                            case 1: //controls
                                gameSeq.step = 10;
                                dialog_destroy(&gameDialog);
                            break;
                            case 4: //EXIT
                                gameSeq.step = 0;                                
                                dialog_destroy(&gameDialog);             
                                clear_to_color(worldScreen, BLACK_COLOR); 
                                game_draw_level();
                                game_hud_draw();
                            break;
                        }
                    }

                    if (input_key_down(E_G_KEY_EXIT))
                    {
                        gameSeq.step = 0;                                
                        dialog_destroy(&gameDialog);             
                        clear_to_color(worldScreen, BLACK_COLOR); 
                        game_draw_level();
                        game_hud_draw();
                    }
                break;
                case 10:    //draw controls menu
                case 11:    //select key
                case 12:    //redefine
                    game_update_controls_menu(worldScreen, 2);
                break;
            }
        break;
        case E_GAME_ST_LOSE_LIVE:
            switch (gameSeq.step)
            {
                case 0:
                    //sfx_play(gameSfx[E_SFX_GAME_PLAYER_LOSE], E_SFX_GAME_VOICE);
                    //play game over music
                    jingleMusic = load_dat_midi_indexed(gameDataIndex, LOSELIVE_MID);
                    music_play(jingleMusic, false);

                    entities_draw(worldScreen);
                    gameSeq.step++;
                break;
                case 1:
                    //wait for dead jingle to end
                    if (music_get_pos() < 0)
                    {
                        MY_TRACE_FLAG( "Lose live\n");                        
                        if (game.lives > 0) 
                        {
                            game.fadeOut = true; 
                            game.state = E_GAME_ST_INIT_LEVEL;
                        }
                        else
                        {
                            game.fadeOut = true; 
                            game.state = E_GAME_ST_GAME_OVER;
                        } 
                        gameSeq.timeCounter = 0;      
                        gameSeq.step = 0;    
                        music_stop();      
                    }
                break;
            }
        break;
        case E_GAME_ST_COMPLETE_LEVEL:
            switch (gameSeq.step)
            {
                case 0:
                    //scroll_update(entity_get(entity_get_player_id())->pos);                    
                    //entities_draw(worldScreen);

                    music_stop(gameMusic);
                    
                    if (gameSeq.timeCounter >= 100)
                    {                
                        //play game over music
                        jingleMusic = load_dat_midi_indexed(gameDataIndex, END_MID);
                        music_play(jingleMusic, false);
                        gameSeq.timeCounter = 0;
                        gameSeq.step++;                        
                    }
                    else
                        gameSeq.timeCounter += clock_tick_get();

                    
                break;                
                case 1:
                    if (music_get_pos() < 0)
                    {                
                        gameSeq.timeCounter = 0;
                        gameSeq.step++;                        
                    }
                break;
                case 2:
                    //obtain score for time left                         
                    if (game.time > 0)
                    {                        
                        game.time--;
                        game.score += SCORE_POINT_TIME_LEFT;
                        game_hud_update();
                        game_hud_draw();                        
                        //play score add sound                            
                        if (!sfx_voice_is_playing(E_SFX_GAME_VOICE))
                            sfx_play(gameSfx[E_SFX_GAME_POINT], E_SFX_GAME_VOICE);                        
                    }
                    else
                    {
                        game_hud_update();
                        game_hud_draw();
                        //play end score point
                        sfx_play(gameSfx[E_SFX_GAME_POINT_END], E_SFX_GAME_VOICE);                        
                        gameSeq.step++;
                    }
                    //bypass gain time score
                    if (input_any_key_pressed())
                    {
                        game.score += (SCORE_POINT_TIME_LEFT * game.time);
                        game.time = 0;
                    }
                break;
                case 3:
                    //wait 2 seconds
                    if (gameSeq.timeCounter >= 2)
                    {                
                        game.fadeOut = true;                         
                        gameSeq.timeCounter = 0;
                        gameSeq.step++;                        
                    }
                    else
                        gameSeq.timeCounter += clock_tick_1sec_get();    
                break;
                case 4:                    
                    MY_TRACE_FLAG( "Completed level number: %i\n", game.actualLevel);
                    game_destroy_level();

                    gameSeq.step = 0;          
                    
                    game.state = E_GAME_ST_SELECT_LEVEL;
                break;
            }
        break;
        case E_GAME_ST_GAME_OVER:            
            switch (gameSeq.step)
            {
                case 0:                    
                    game_destroy_level();                                                

                    //load pal
                    currentPal = gamePal;
                    //put game over text
                    textout_centre_ex(buffer, gameFont[E_GAME_FONT_BIG], lang_get_txt(E_TXT_GAME_OVER), SCREEN_W>>1, 20, WHITE_COLOR, BLACK_COLOR);                    
                    //prepare systems to create an entity on screen
                    scroll_create((tVector){SCREEN_W, SCREEN_H}, (tVector){SCREEN_W, SCREEN_H}, E_SCROLL_MODE_BY_WINDOW_Y_ONLY);
                    object_system_init();
                    entity_system_init();                   
                    //create entity of player crying animation
                    entity_create(E_ENT_CLASS_OBJECT, E_GAME_OVER_OBJECT_TYPE, (tVector){(SCREEN_W>>1) - 14, 60}, E_ENT_DIR_RIGHT, 0);
                    //update and draw the entity animation
                    entities_update();
                    entities_draw(buffer);

                    //play game over music
                    gameMusic = load_dat_midi_indexed(gameDataIndex, GAMEOVER_MID);
                    music_play(gameMusic, true);

                    game.fadeIn = true;                    
                    gameSeq.step++;
                    MY_TRACE_FLAG( "Game Over\n");
                break;
                case 1:
                    //update and draw the entity animation
                    entities_update();
                    entities_draw(buffer);

                    if (gameSeq.timeCounter >= 400 || input_any_key_pressed())
                    {
                        textout_centre_ex(buffer, gameFont[E_GAME_FONT], lang_get_txt(E_TXT_CONTINUE_QUESTION), SCREEN_W>>1, 138, DIALOG_SEL_TEXT_COLOR, BLACK_COLOR);
                        //create continue menu
                        gameDialog = dialog_create((tRectangle){(tVector){GAMEOVER_MENU_POS_X, GAMEOVER_MENU_POS_Y}, (tVector){GAMEOVER_MENU_SIZE_X, GAMEOVER_MENU_SIZE_Y}}, DIALOG_TEXT_COLOR, DIALOG_SEL_TEXT_COLOR, true);
                        dialog_add_option(&gameDialog, lang_get_txt(E_TXT_YES));
                        dialog_add_option(&gameDialog, lang_get_txt(E_TXT_NO));                    
                        dialog_draw(&gameDialog, buffer);

                        gameSeq.step++;
                        gameSeq.timeCounter = 0;
                    }      
                    else
                        gameSeq.timeCounter += clock_tick_get();                             
                break;
                case 2:
                    //update and draw the entity animation
                    entities_update();
                    entities_draw(buffer);

                    game_navigation_menu(&gameDialog, buffer);
                    if (input_key_down(E_G_KEY_ENTER))
                    {
                        sfx_play(gameSfx[E_SFX_GAME_MENU_SELECT], E_SFX_GAME_VOICE);
                        music_stop();
                        switch (gameDialog.optionSelected)
                        {
                            case 0: //CONTINUE: YES
                                //destroy systems
                                entities_destroy_all();
                                object_system_destroy();

                                game.state = E_GAME_ST_SELECT_LEVEL;
                                gameSeq.timeCounter = 0;
                                gameSeq.step = 0;
                                game.fadeOut = true;
                                dialog_destroy(&gameDialog);
                            break;
                            case 1: //CONTINUE: NO
                                //destroy systems
                                entities_destroy_all();
                                object_system_destroy();

                                game_init_flags();
                                game.state = E_GAME_ST_TITLE;
                                gameSeq.timeCounter = 0;
                                gameSeq.step = 0;
                                game.fadeOut = true;
                                dialog_destroy(&gameDialog);
                            break;                            
                        }
                    }                         
                break;
            }
        break;
        case E_GAME_ST_ENDING:            
            BITMAP *ending;
            switch (gameSeq.step)
            {
                case 0: 
                    currentPal = introPal;    
                    game.fadeIn = true;
                    
                    clear_to_color(buffer, BLACK_COLOR);
                    ending = load_dat_bmp_indexed(gameDataIndex, ENDING1_BMP + sceneCounter);
                    draw_sprite(buffer, ending, (SCREEN_W>>1) - (ending->w>>1), 0);    
                    destroy_bitmap(ending);

                    text_multiline_draw(buffer, gameFont[E_GAME_FONT_MID], lang_get_txt(E_TXT_ENDING_1 + sceneCounter), SCREEN_W>>1, 140, WHITE_COLOR, BLACK_COLOR);
                                        
                    game.fadeIn = E_FADE_TYPE_0_63;

                    gameSeq.step++;
                break;
                case 1:
                    if (gameSeq.timeCounter >= 20)
                    {
                        gameSeq.timeCounter = 0;
                        game.fadeIn = E_FADE_TYPE_64_255;
                        gameSeq.step++;
                    }
                    else
                        gameSeq.timeCounter += clock_tick_get();
                break;
                case 2:
                    if (input_any_key_pressed())
                    {
                        sceneCounter++;
                        if (sceneCounter >= ENDING_SCENES)
                        {
                            game.state = E_GAME_ST_TITLE;
                            currentPal = gamePal;
                            sceneCounter = 0;
                        }
                        gameSeq.step = 0;    
                        game.fadeOut = true;
                    }
                break;
            }
        break;
        case E_GAME_ST_DESTROY_LEVEL:
            game_destroy_level();
            game.state = E_GAME_ST_EXIT;
        break;
        case E_GAME_ST_EXIT:
        default:
            game_destroy();
            gameExit = true;
        break;
    }
       
    #ifdef DEBUGMODE
        game_debug_update();    
    #endif

    //update sfx
    sfx_update();
    //clear keyboard buffer to use keypressed()
    clear_keybuf();
    //updates frame counter
    inc_frame_counter();    
}


static void game_destroy_level()
{
    MY_TRACE_FLAG( "Destroying level\n");
    //destroy entities
    entities_destroy_all();
    object_system_destroy();
    enemy_system_destroy();    
    //unload map and map resources
    map_unload();    

    unload_datafile(levelDAT);

    clear_to_color(worldScreen, BLACK_COLOR);
    clear_to_color(buffer, BLACK_COLOR);   
    game.viewMap = false;
}

static void game_init_flags()
{
    game.actualLevel            = 0;
    game.actualCompletedLevel   = -1;
    #ifdef DEBUGMODE
        game.actualLevel        = DEBUG_INI_GAME_LEVEL;        
    #endif
    memset(&game.levelComplete, 0, sizeof(game.levelComplete));
    /*game.levelComplete[0] = true;
    game.levelComplete[1] = true;
    game.levelComplete[2] = true;
    game.levelComplete[3] = true;*/
    
    game.lives          = GAME_INI_LIVES;
    game.life           = GAME_INI_LIFE;
    game.score          = 0;
    game.viewMap        = false;
    hud.refresh         = E_REFRESH_HUD_ALL;
    game.doorId         = 0;
}

void game_load_resources()
{
    //create game data file index for fast open individual data objects
    gameDataIndex = create_dat_index("game.dat");
    game_loading_text();

    //load palettes
    introPal = load_dat_pal_indexed(gameDataIndex, INTRO_PAL);
    game_loading_text();
    gamePal  = load_dat_pal_indexed(gameDataIndex, HUD_PAL);
    game_loading_text();

    //loads game fonts
    gameFont[E_GAME_FONT]        = grab_font_from_bitmap(load_dat_bmp_indexed(gameDataIndex, FONT_BMP));
    game_loading_text();
    gameFont[E_GAME_FONT_BIG]    = grab_font_from_bitmap(load_dat_bmp_indexed(gameDataIndex, BIGFONT_BMP));
    game_loading_text();
    gameFont[E_GAME_FONT_MID]    = grab_font_from_bitmap(load_dat_bmp_indexed(gameDataIndex, MIDFONT_BMP));
    game_loading_text();

    //load hud image
    hud.hudImg = load_dat_bmp_indexed(gameDataIndex, HUD_BMP);
    game_loading_text();

    //load game sfx
    gameSfx[E_SFX_GAME_POINT]           = load_dat_wav_indexed(gameDataIndex, POINT_WAV);
    gameSfx[E_SFX_GAME_POINT_END]       = load_dat_wav_indexed(gameDataIndex, POINTEND_WAV);
    gameSfx[E_SFX_GAME_MENU_NAV]        = load_dat_wav_indexed(gameDataIndex, SELECT_WAV);
    gameSfx[E_SFX_GAME_MENU_SELECT]     = load_dat_wav_indexed(gameDataIndex, SELECTED_WAV);
    gameSfx[E_SFX_GAME_LEVEL_OK]        = load_dat_wav_indexed(gameDataIndex, LEVELOK_WAV);
    gameSfx[E_SFX_GAME_LEVEL_BLUE]      = load_dat_wav_indexed(gameDataIndex, LEVELBLU_WAV);
    game_loading_text();
}

void game_init()
{
    MY_TRACE_FLAG( "Init game\n");

    //set current game palette
    currentPal = introPal;
    set_palette(currentPal);
    
    //initialize buffer screen
    #if GAME_GFX_DOBLE_BUFFER
        buffer = create_bitmap(SCREEN_W, SCREEN_H);
    #else
        buffer = screen;
    #endif
    clear(buffer);
    //initialize map bitmap
    worldScreen = create_bitmap(GAME_W, GAME_H);
    clear(worldScreen);
    
    //hud.hudImg = load_bmp("res/game/hud.bmp", NULL);
    hud.hudLifeOff = create_bitmap(15, 14);
    hud.hudLifeOn  = create_bitmap(15, 14);
    blit(hud.hudImg, hud.hudLifeOn, 65, 5, 0, 0, 15, 14);
    blit(hud.hudImg, hud.hudLifeOff, 99, 5, 0, 0, 15, 14);

    //init systems
    #ifdef DEBUGMODE
        debug_init();
    #endif    
    timer_init(GAME_CLOCK_TICK, ALLEGRO_USES_TIMER);    
    lang_init(E_NUM_LANGS, E_TXT_NUM);
    entity_system_init();
    collision_system_init();
    interface_init(load_dat_bmp_indexed(gameDataIndex, DIALOG_BMP), gameFont[E_GAME_FONT]);
    sfx_init(load_dat_wav_indexed(gameDataIndex, POINT_WAV), E_SFX_NUM_VOICES);
    input_keys_init(E_GAME_KEYS_NUM);    

    //loads language texts and set language by default
    lang_load_mem((char *)load_datafile_object_indexed(gameDataIndex, ENG_TXT)->dat, E_LANG_ENG);
    //TODO: translate texts to spanish
    lang_load_mem((char *)load_datafile_object_indexed(gameDataIndex, ESP_TXT)->dat, E_LANG_ESP);
    MY_TRACE_MARK;
    
    //initialize levels data    
    levelData[E_GAME_LEVEL_TUTORIAL].mapFile        = "tutorial.bin";
    levelData[E_GAME_LEVEL_TUTORIAL].dataFile       = "tutorial.dat";
    levelData[E_GAME_LEVEL_TUTORIAL].tileFileIndex  = TUTORIAL_BMP;
    levelData[E_GAME_LEVEL_TUTORIAL].palFileIndex   = TUTORIAL_PAL;
    levelData[E_GAME_LEVEL_TUTORIAL].musicFileIndex = TUTORIAL_MID;
    
    levelData[E_GAME_LEVEL_JURASSIC].mapFile        = "jurassic.bin";
    levelData[E_GAME_LEVEL_JURASSIC].dataFile       = "jurassic.dat";
    levelData[E_GAME_LEVEL_JURASSIC].tileFileIndex  = JURASSIC_BMP;
    levelData[E_GAME_LEVEL_JURASSIC].palFileIndex   = JURASSIC_PAL;
    levelData[E_GAME_LEVEL_JURASSIC].musicFileIndex = JUNGLE_MID;

    levelData[E_GAME_LEVEL_EGYPT].mapFile           = "egypt.bin";
    levelData[E_GAME_LEVEL_EGYPT].dataFile          = "egypt.dat";
    levelData[E_GAME_LEVEL_EGYPT].tileFileIndex     = EGYPT_BMP;
    levelData[E_GAME_LEVEL_EGYPT].palFileIndex      = EGYPT_PAL;
    levelData[E_GAME_LEVEL_EGYPT].musicFileIndex    = EGYPT_MID;
    
    levelData[E_GAME_LEVEL_WEST].mapFile            = "west.bin";
    levelData[E_GAME_LEVEL_WEST].dataFile           = "west.dat";
    levelData[E_GAME_LEVEL_WEST].tileFileIndex      = WEST_BMP;
    levelData[E_GAME_LEVEL_WEST].palFileIndex       = WEST_PAL;
    levelData[E_GAME_LEVEL_WEST].musicFileIndex     = WEST_MID;

    levelData[E_GAME_LEVEL_MEDIEVAL].mapFile        = "medieval.bin";
    levelData[E_GAME_LEVEL_MEDIEVAL].dataFile       = "medieval.dat";
    levelData[E_GAME_LEVEL_MEDIEVAL].tileFileIndex  = MEDIEVAL_BMP;
    levelData[E_GAME_LEVEL_MEDIEVAL].palFileIndex   = MEDIEVAL_PAL;
    levelData[E_GAME_LEVEL_MEDIEVAL].musicFileIndex = MEDIEVAL_MID;

    //set inital game state
    #ifdef DEBUGMODE
        game.state      = DEBUG_INI_GAME_STATE;
    #else
        game.state      = E_GAME_ST_LOGO;
    #endif
    
    //initialize game flags
    game_init_flags();
    game.prevState      = 255;
    game.fadeState      = E_FADED_IN;    
    game.fadeOut        = true;    
    
    //load game config
    game_load_config();
}

void game_draw()
{   
    //blit worldScreen on buffer (centered on screen)
    if (game.viewMap)        
        blit(worldScreen, buffer, 0, 0, GAME_X, GAME_Y, GAME_W, GAME_H);        
           
    #ifdef DEBUGMODE
        //draw debug info
        if (debugOptions.showDebugInfo)
            debug_draw(buffer);
    #endif
    
    //wait for vsync
    vsync();    
    timer_end_frame();
    
    //double buffer: blit to screen
    #if GAME_GFX_DOBLE_BUFFER
        blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);  
    #endif
    
    //do pending fades
    game_do_fade();
}

#ifdef DEBUGMODE
static void game_debug_update()
{
    if (debugOptions.showDebugInfo)
        game_debug_info();
    
    //force game exit
    if (key[KEY_X] && (key_shifts & KB_CTRL_FLAG))
        game.state = E_GAME_ST_EXIT;
    
    //toggle debug options
    if (input_key_down(E_G_KEY_D) && (key_shifts & KB_CTRL_FLAG))
        debugOptions.showDebugInfo = debugOptions.showDebugInfo < 2 ? debugOptions.showDebugInfo + 1 : 0;
    //toggle step by step
    if (input_key_down(E_G_KEY_S) && (key_shifts & KB_CTRL_FLAG))
        debugOptions.stepByStep = !debugOptions.stepByStep;
    //toggle invencible mode
    if (input_key_down(E_G_KEY_I) && (key_shifts & KB_CTRL_FLAG))
    {
        debugOptions.invencible = !debugOptions.invencible;
        game.life = 3;
        
        //recolor life hearts to indicate invencible
        RGB lifeColor;        
        lifeColor.r = !debugOptions.invencible ? 255    : 0;
        lifeColor.g = !debugOptions.invencible ? 0      : 0;
        lifeColor.b = !debugOptions.invencible ? 0      : 255;        
        set_color(21, &lifeColor);
    }
    //insert log mark
    if (key[KEY_M] && (key_shifts & KB_CTRL_FLAG))
        MY_TRACE_MARK;
    //toogle deltaTime
    if (input_key_down(E_G_KEY_T) && (key_shifts & KB_CTRL_FLAG))
        toggle_delta_time();    

    //record demo
    if (key[KEY_I] && (key_shifts & KB_CTRL_FLAG) && !input_log_recording())
        input_log_record("tutorial.rec");
    //stop recording demo
    if (key[KEY_O] && (key_shifts & KB_CTRL_FLAG) && (input_log_recording() || input_log_playing()))
        input_log_stop();
    //play recorded demo
    if (key[KEY_P] && (key_shifts & KB_CTRL_FLAG) && !input_log_playing())
        input_log_play("tutorial.rec");

    //trace state          
    if (game.state != game.prevState)
    {
        MY_TRACE_FLAG("Game changes from state %i to state %i\n", game.prevState, game.state);
    }
}

static void game_debug_info()
{
    //debug info
    show_debug("FPS: %d", fps_get());
    show_debug("s.x: %d, s.y: %d", scroll_get_position().x, scroll_get_position().y);
    //show_debug( "p.vX: %f", fixtof(entity_get(entity_get_player_id())->fixVel.x));
    //show_debug( "p.vY: %f", fixtof(entity_get(entity_get_player_id())->fixVel.y));
    //show_debug( "p.x: %d", entity_get(entity_get_player_id())->pos.x);
    //show_debug( "p.y: %d", entity_get(entity_get_player_id())->pos.y);
    //show_debug("Lives:%i Life:%i", game.lives, game.life);
    //show_debug("State: %i", game.state);
}
#endif

//testing
static void game_load_level(uint8_t numLevel)
{
    //load level data file
    levelDAT = load_datafile(levelData[numLevel].dataFile);
    
    //load level palette
    currentPal = levelDAT[levelData[numLevel].palFileIndex].dat;

    //load map and entities    
    map_load(levelData[numLevel].mapFile, (BITMAP *)levelDAT[levelData[numLevel].tileFileIndex].dat, (tVector){GAME_W, GAME_H});
    
    //TODO: read scroll mode from map
    game.scrollMode =  E_SCROLL_MODE_BY_WINDOW_Y_ONLY;
    
    //calculate scroll dimensions based on map dimensions
    tVector mapDimension = map_get_dimensions();
    mapDimension.x = mapDimension.x - GAME_W;
    mapDimension.y = mapDimension.y - GAME_H;
    
    //create scroll    
    scroll_create((tVector){GAME_W,GAME_H}, mapDimension, game.scrollMode);   
    
    //load music level
    gameMusic = (MIDI *)levelDAT[levelData[numLevel].musicFileIndex].dat;
}

void game_destroy()
{
    MY_TRACE_FLAG("Destroying game\n");
    
    //free fonts
    MY_TRACE_FLAG("\tDestroying fonts\n");
    for (uint8_t i = 0; i < E_GAME_NUM_FONTS; i++)
    {
        if (gameFont[i])
        {
            destroy_font(gameFont[i]);
            gameFont[i] = NULL;
        }
    }
    
    //free samples
    MY_TRACE_FLAG("\tDestroying game sfx\n");
    for (uint8_t i = 0; i < E_SFX_GAME_NUM; i++)
    {
        if (gameSfx[i])
        {
            destroy_sample(gameSfx[i]);
            gameSfx[i] = NULL;
        }
    }

    //destroy game data index
    MY_TRACE_FLAG("\tDestroying gameDataIndex\n");
    destroy_dat_index(gameDataIndex);

    //destroy bitmaps
    MY_TRACE_FLAG("\tDestroying bitmaps\n");
    destroy_bitmap(hud.hudImg);
    destroy_bitmap(worldScreen);
    destroy_bitmap(buffer);

    MY_TRACE_FLAG("\tDestroying input key system\n");
    input_keys_destroy();
    MY_TRACE_FLAG("\tDestroying language system\n");
    lang_destroy();
    MY_TRACE_FLAG("\tDestroying interface system\n");
    interface_destroy();
}

static void game_do_fade()
{
    if (game.fadeOut)
    {
        if (game.fadeState == E_FADED_IN)
        {
            fade_out(GAME_FADE_SPEED);
            game.fadeState = E_FADED_OFF;
        }
        
        game.fadeOut = false;        
    }

    //fade in
    switch (game.fadeIn)
    {
        case E_FADE_TYPE_NORMAL:
            if (game.fadeState == E_FADED_OFF)
            {
                fade_in(currentPal, GAME_FADE_SPEED);
            }
        break;
        case E_FADE_TYPE_0_63:
            fade_in_range(currentPal, GAME_FADE_SPEED, 0, 63);
        break;
        case E_FADE_TYPE_64_255:
            fade_in_range(currentPal, GAME_FADE_SPEED, 64, 255);
        break;
    }

    if (game.fadeIn != E_FADE_TYPE_NONE)
    {
        game.fadeState = E_FADED_IN;
        game.fadeIn = false;
    }
}

//inits the hud
void game_hud_init()
{
    draw_sprite(buffer, hud.hudImg, HUD_POSITION_X, HUD_POSITION_Y);
    draw_sprite(buffer, hud.hudLifeOn, HUD_POSITION_X + 65, HUD_POSITION_Y + 5);
    draw_sprite(buffer, hud.hudLifeOn, HUD_POSITION_X + 82, HUD_POSITION_Y + 5);
    draw_sprite(buffer, hud.hudLifeOn, HUD_POSITION_X + 99, HUD_POSITION_Y + 5);

    textout_centre_ex(buffer, gameFont[E_GAME_FONT], lang_get_txt(E_TXT_HUD_LIVES),   HUD_POSITION_X + 22, HUD_POSITION_Y - 7, WHITE_COLOR, 1);
    textout_centre_ex(buffer, gameFont[E_GAME_FONT], lang_get_txt(E_TXT_HUD_LIFE),    HUD_POSITION_X + 90, HUD_POSITION_Y - 7, WHITE_COLOR, 1);
    textout_centre_ex(buffer, gameFont[E_GAME_FONT], lang_get_txt(E_TXT_HUD_SCORE),   HUD_POSITION_X + 160, HUD_POSITION_Y - 7, WHITE_COLOR, 1);
    textout_centre_ex(buffer, gameFont[E_GAME_FONT], lang_get_txt(E_TXT_HUD_TIME),    HUD_POSITION_X + 220, HUD_POSITION_Y - 7, WHITE_COLOR, 1);

    textout_centre_ex(buffer, gameFont[E_GAME_FONT], "X",   HUD_POSITION_X + 23, HUD_POSITION_Y + 5, WHITE_COLOR, -1);

    hud.refresh = E_REFRESH_HUD_ALL;
}

//updated the hud
void game_hud_update()
{
    if (game.life != hud.last_life)    
    {
        hud.refresh |= E_REFRESH_HUD_LIFE;    
        hud.last_life = game.life;
    }
    if (game.lives != hud.last_lives)
    {
        hud.refresh |= E_REFRESH_HUD_LIVES;
        hud.last_lives = game.lives;
    }
    if (game.score != hud.last_score)
    {
        //check extra life by score
        if (!(game.score % SCORE_POINT_EXTRA_LIFE) && game.score > 0 && game.score != hud.last_score)
        {
            game.lives++;
        }
        //check overflow  (gain 3 lives)       
        if (game.score >= SCORE_OVERFLOW)
        {
            game.score = 0;
            game.lives += 3;
        }

        hud.refresh |= E_REFRESH_HUD_SCORE;
        hud.last_score = game.score;
    }
    if (game.time != hud.last_time)
    {
        hud.refresh |= E_REFRESH_HUD_TIME;
        hud.last_time = game.time;
    }
}

//draws the hud
void game_hud_draw()
{   
    //update lives
    if (CHECK_FLAG(hud.refresh, E_REFRESH_HUD_LIVES))
    {
        textprintf_centre_ex(buffer, gameFont[E_GAME_FONT], HUD_POSITION_X + 31, HUD_POSITION_Y + 5, WHITE_COLOR, BLACK_COLOR, "%u", game.lives);
    }

    //update life
    if (CHECK_FLAG(hud.refresh, E_REFRESH_HUD_LIFE))
    {
        switch (game.life)
        {
            case 3:
                draw_sprite(buffer, hud.hudLifeOn, HUD_POSITION_X + 65, HUD_POSITION_Y + 5);
                draw_sprite(buffer, hud.hudLifeOn, HUD_POSITION_X + 82, HUD_POSITION_Y + 5);
                draw_sprite(buffer, hud.hudLifeOn, HUD_POSITION_X + 99, HUD_POSITION_Y + 5);
            break;
            case 2:                                
                draw_sprite(buffer, hud.hudLifeOn, HUD_POSITION_X + 65, HUD_POSITION_Y + 5);
                draw_sprite(buffer, hud.hudLifeOn, HUD_POSITION_X + 82, HUD_POSITION_Y + 5);
                draw_sprite(buffer, hud.hudLifeOff, HUD_POSITION_X + 99, HUD_POSITION_Y + 5);
            break;
            case 1:
                draw_sprite(buffer, hud.hudLifeOn, HUD_POSITION_X + 65, HUD_POSITION_Y + 5);
                draw_sprite(buffer, hud.hudLifeOff, HUD_POSITION_X + 82, HUD_POSITION_Y + 5);
                draw_sprite(buffer, hud.hudLifeOff, HUD_POSITION_X + 99, HUD_POSITION_Y + 5);
            break;
            default:
                draw_sprite(buffer, hud.hudLifeOff, HUD_POSITION_X + 65, HUD_POSITION_Y + 5);
                draw_sprite(buffer, hud.hudLifeOff, HUD_POSITION_X + 82, HUD_POSITION_Y + 5);
                draw_sprite(buffer, hud.hudLifeOff, HUD_POSITION_X + 99, HUD_POSITION_Y + 5);
            break;
        }
    }

    //update score
    if (CHECK_FLAG(hud.refresh, E_REFRESH_HUD_SCORE))
    {
        textprintf_centre_ex(buffer, gameFont[E_GAME_FONT], HUD_POSITION_X + 160, HUD_POSITION_Y + 5, WHITE_COLOR, BLACK_COLOR, "%04u", game.score);
    }

    //update time
    if (CHECK_FLAG(hud.refresh, E_REFRESH_HUD_TIME))
    {
        #if DEBUG_FPS_ON_CLOCK
            textprintf_centre_ex(buffer, gameFont[E_GAME_FONT], HUD_POSITION_X + 220, HUD_POSITION_Y + 5, game.time <= GAME_HURRY_TIME_LEFT && game.state == E_GAME_ST_PLAY_LEVEL ? RED_COLOR : WHITE_COLOR, BLACK_COLOR, "%03u", fps_get());
        #else
            textprintf_centre_ex(buffer, gameFont[E_GAME_FONT], HUD_POSITION_X + 220, HUD_POSITION_Y + 5, game.time <= GAME_HURRY_TIME_LEFT && game.state == E_GAME_ST_PLAY_LEVEL ? RED_COLOR : WHITE_COLOR, BLACK_COLOR, "%03u", game.time);
        #endif
    }

    //reset refresh flags
    hud.refresh = 0x00;
}

//pauses all sfx voices except game voice
static void game_pause_sound()
{
    for (uint8_t i = 0; i < E_SFX_NUM_VOICES; i++)
    {
        if (i != E_SFX_GAME_VOICE)
            sfx_pause(i);
    }

    music_pause();
}

//resumes all sfx voices except game voice
static void game_resume_sound()
{
    for (uint8_t i = 0; i < E_SFX_NUM_VOICES; i++)
    {
        if (i != E_SFX_GAME_VOICE)
            sfx_resume(i);
    }

    music_resume();
}

//process navigation dialog menu trought game controls. Return true if value changes
static bool game_navigation_menu(tDialog *dialog, BITMAP *drawBuffer)
{
    bool changedValue = false;

    if (input_key_down(E_G_KEY_RIGHT))
    {
        if (dialog_inc_option_value(dialog))
        {
            sfx_play(gameSfx[E_SFX_GAME_MENU_NAV], E_SFX_GAME_VOICE);
            dialog_draw(dialog, drawBuffer);
            changedValue = true;
        }
    }
    if (input_key_down(E_G_KEY_LEFT))
    {
        if (dialog_dec_option_value(dialog))
        {
            sfx_play(gameSfx[E_SFX_GAME_MENU_NAV], E_SFX_GAME_VOICE);
            dialog_draw(dialog, drawBuffer);
            changedValue = true;
        }
    }
    if (input_key_down(E_G_KEY_DOWN))
    {
        if (dialog_next_option(dialog))
        {
            sfx_play(gameSfx[E_SFX_GAME_MENU_NAV], E_SFX_GAME_VOICE);
            dialog_draw(dialog, drawBuffer);
        }
    }
    if (input_key_down(E_G_KEY_UP))
    {
        if (dialog_prev_option(dialog))
        {
            sfx_play(gameSfx[E_SFX_GAME_MENU_NAV], E_SFX_GAME_VOICE);
            dialog_draw(dialog, drawBuffer);
        }
    }

    return changedValue;
}

//loads game configuration from file
static void game_load_config()
{
    //load config file
    FILE *file = fopen(CONFIG_FILE, "rb");
    
    if (!file) {
        //if not exists, create
        MY_TRACE_FLAG("Config file doesn't exist. Creating with default values\n");
        
        //default values
        gameConfig.lang                     = E_LANG_ENG;    
        gameConfig.sfxVolume                = 255;
        gameConfig.musicVolume              = 255;
        gameConfig.gameKeys[E_G_KEY_UP]     = KEY_UP;
        gameConfig.gameKeys[E_G_KEY_DOWN]   = KEY_DOWN;
        gameConfig.gameKeys[E_G_KEY_LEFT]   = KEY_LEFT;
        gameConfig.gameKeys[E_G_KEY_RIGHT]  = KEY_RIGHT;
        gameConfig.gameKeys[E_G_KEY_JUMP]   = KEY_Z;
        gameConfig.gameKeys[E_G_KEY_ACTION] = KEY_X;


        game_save_config();        

        //set first run flag
        firstRun = true;
    }
    else
    {
        //read configuration
        MY_TRACE_FLAG("Loading config file\n");
        
        //read each config data to avoid padding problems
        fread(&gameConfig.lang,         sizeof(gameConfig.lang),            1, file);
        fread(&gameConfig.sfxVolume,    sizeof(gameConfig.sfxVolume),       1, file);
        fread(&gameConfig.musicVolume,  sizeof(gameConfig.musicVolume),     1, file);
        for (uint8_t i = 0; i <= E_G_KEY_ACTION; i++)
            fread(&gameConfig.gameKeys[i],  sizeof(uint8_t),     1, file);

        fclose(file);

        //set config
        lang_set(gameConfig.lang);
        sfx_set_volume(gameConfig.sfxVolume);
        music_set_volume(gameConfig.musicVolume);

        MY_TRACE_FLAG("Config file readed\n");
    }

    //redefine configurable keys
    for (uint8_t i = 0; i <= E_G_KEY_ACTION; i++)
        input_key_redefine(i,      gameConfig.gameKeys[i]);
    //redefine fixed game keys
    input_key_redefine(E_G_KEY_PAUSE,   KEY_SPACE);
    input_key_redefine(E_G_KEY_EXIT,    KEY_ESC);
    input_key_redefine(E_G_KEY_ENTER,   KEY_ENTER);
    input_key_redefine(E_G_KEY_D,       KEY_D);
    input_key_redefine(E_G_KEY_S,       KEY_S);
    input_key_redefine(E_G_KEY_I,       KEY_I);
    input_key_redefine(E_G_KEY_T,       KEY_T);
    //load string of controls of current language
    game_load_control_strings();
}

//saves game configuration to a file
static void game_save_config()
{    
    FILE *file = fopen(CONFIG_FILE, "wb");

    fwrite(&gameConfig.lang,         sizeof(gameConfig.lang),            1, file);
    fwrite(&gameConfig.sfxVolume,    sizeof(gameConfig.sfxVolume),       1, file);
    fwrite(&gameConfig.musicVolume,  sizeof(gameConfig.musicVolume),     1, file);
    for (uint8_t i = 0; i <= E_G_KEY_ACTION; i++)
        fwrite(&gameConfig.gameKeys[i],  sizeof(uint8_t),     1, file);

    fclose(file);
    MY_TRACE_FLAG("Config file saved\n");     
}

//creates menu options
static void game_create_options_menu()
{
    dialog_add_text_option(&gameDialog, lang_get_txt(E_TXT_MENU_LANG), lang_get_txt(E_TXT_MENU_LANG_OPTIONS), &gameConfig.lang);
    dialog_add_option(&gameDialog, lang_get_txt(E_TXT_MENU_CONTROLS));
    dialog_add_num_option(&gameDialog, lang_get_txt(E_TXT_MENU_SFX_VOLUME), 0, 255, &gameConfig.sfxVolume, 10);
    dialog_add_num_option(&gameDialog, lang_get_txt(E_TXT_MENU_MUSIC_VOLUME), 0, 255, &gameConfig.musicVolume, 10);
    dialog_add_option(&gameDialog, lang_get_txt(E_TXT_MENU_EXIT));
}

//creates play menu options
static void game_create_options_play_menu()
{
    gameDialog = dialog_create((tRectangle){(tVector){PLAY_MENU_POS_X, PLAY_MENU_POS_Y}, (tVector){PLAY_MENU_SIZE_X, PLAY_MENU_SIZE_Y}}, DIALOG_TEXT_COLOR, DIALOG_SEL_TEXT_COLOR, true);
    dialog_add_option(&gameDialog, lang_get_txt(E_TXT_MENU_CONTINUE));
    dialog_add_option(&gameDialog, lang_get_txt(E_TXT_MENU_OPTIONS));
    dialog_add_option(&gameDialog, lang_get_txt(E_TXT_MENU_RESTART_LEVEL));
    dialog_add_option(&gameDialog, lang_get_txt(E_TXT_MENU_EXIT_TO_TITLE));
    dialog_add_option(&gameDialog, lang_get_txt(E_TXT_MENU_EXIT_TO_DOS));
}

//process menu options
static void game_process_options_menu()
{
    switch (gameDialog.optionSelected)
    {
        case 0: //lang                                
            lang_set(gameConfig.lang);
            game_load_control_strings();
            game_save_config();
            //redraw dialog
            dialog_destroy(&gameDialog);
            gameSeq.step--;
        break;
        case 2: //sfx volume
            sfx_set_volume(gameConfig.sfxVolume);
            game_save_config();
        break;
        case 3: //music volume                                
            music_set_volume(gameConfig.musicVolume);                                
            game_save_config();
        break;
    }
}

//creates and updates menu redefine controls
static void game_update_controls_menu(BITMAP *drawBuffer, uint8_t stepReturn)
{
    char txtBuffer[DIALOG_MAX_OPTION_LENGTH];
            
    switch (gameSeq.step)
    {
        case 10: //draw controls menu
            
            gameDialog = dialog_create((tRectangle){(tVector){CONTROLS_MENU_POS_X, CONTROLS_MENU_POS_Y}, (tVector){CONTROLS_MENU_SIZE_X, CONTROLS_MENU_SIZE_Y}}, DIALOG_TEXT_COLOR, DIALOG_SEL_TEXT_COLOR, true);
            
            for (uint8_t i = 0; i < 6; i++)
            {
                //%-8s:%-10s
                sprintf(txtBuffer, "%s: %-10s",lang_get_txt(E_TXT_MENU_CTRL_UP + i), keyStrings[input_get_defined_key(E_G_KEY_UP + i)]);
                dialog_add_option(&gameDialog, txtBuffer);    
            }
            dialog_add_option(&gameDialog, lang_get_txt(E_TXT_MENU_EXIT)); 
            
            dialog_draw(&gameDialog, drawBuffer);
            gameSeq.step++;
        break;
        case 11: //use controls menu
            game_navigation_menu(&gameDialog, drawBuffer);

            //select key to redefine
            if (input_key_down(E_G_KEY_ENTER))
            {
                sfx_play(gameSfx[E_SFX_GAME_MENU_SELECT], E_SFX_GAME_VOICE);
                
                if (gameDialog.optionSelected == 6) //EXIT
                {
                    gameSeq.step = stepReturn;
                    if (stepReturn == 2)
                    {
                        game_draw_level();
                        game_hud_draw();
                        game_create_options_play_menu();
                        dialog_draw(&gameDialog, drawBuffer);
                    }
                    dialog_destroy(&gameDialog);   
                }  
                else
                {
                    char txtBuffer[DIALOG_MAX_OPTION_LENGTH];
                    sprintf(txtBuffer, "%s: %-10s",lang_get_txt(E_TXT_MENU_CTRL_UP + gameDialog.optionSelected), lang_get_txt(E_TXT_MENU_PRESS_KEY));
                    strcpy(gameDialog.option[gameDialog.optionSelected].text, txtBuffer);
                    dialog_draw(&gameDialog, drawBuffer);
                    gameSeq.step++;                        
                }
            }

            if (input_key_down(E_G_KEY_EXIT))
            {
                gameSeq.step = stepReturn;
                if (stepReturn == 2)
                {
                    game_draw_level();
                    game_hud_draw();
                    game_create_options_play_menu();
                    dialog_draw(&gameDialog, drawBuffer);
                }
                dialog_destroy(&gameDialog);       
            }
        break;
        case 12: //redefine selected option key (esc = cancels)
            clear_keybuf();
            uint8_t selectedKey = readkey()>>8;
            //redefine key if not hit ESC
            if (selectedKey != KEY_ESC)
            {
                gameConfig.gameKeys[gameDialog.optionSelected] = selectedKey;
                input_key_redefine(gameDialog.optionSelected, selectedKey);
                game_save_config();
            }
            //restore option text
            sprintf(txtBuffer, "%s: %-10s",lang_get_txt(E_TXT_MENU_CTRL_UP + gameDialog.optionSelected), keyStrings[input_get_defined_key(E_G_KEY_UP + gameDialog.optionSelected)]);
            strcpy(gameDialog.option[gameDialog.optionSelected].text, txtBuffer);
            dialog_draw(&gameDialog, drawBuffer); 
            input_keys_update();
            gameSeq.step = 11;
        break;
    }       
}

//summary function to update the world level
static void game_update_level()
{
    entities_update();
    scroll_update(entity_get(entity_get_player_id())->pos);        
}

//summary function to draw the world level
static void game_draw_level()
{
    map_draw(worldScreen, false);
    entities_draw(worldScreen);
    map_draw(worldScreen, true);
}

static void game_load_control_strings()
{
    //set game control strings langs
    gameControlStrings[E_G_KEY_UP]      = strdup(lang_get_txt(E_TXT_MENU_CTRL_UP));
    gameControlStrings[E_G_KEY_DOWN]    = strdup(lang_get_txt(E_TXT_MENU_CTRL_DOWN));
    gameControlStrings[E_G_KEY_LEFT]    = strdup(lang_get_txt(E_TXT_MENU_CTRL_LEFT));
    gameControlStrings[E_G_KEY_RIGHT]   = strdup(lang_get_txt(E_TXT_MENU_CTRL_RIGHT));
    gameControlStrings[E_G_KEY_JUMP]    = strdup(lang_get_txt(E_TXT_MENU_CTRL_JUMP));
    gameControlStrings[E_G_KEY_ACTION]  = strdup(lang_get_txt(E_TXT_MENU_CTRL_ACTION));

    keyStrings[KEY_LEFT]                = strdup(lang_get_txt(E_TXT_MENU_CTRL_LEFT));
    keyStrings[KEY_RIGHT]               = strdup(lang_get_txt(E_TXT_MENU_CTRL_RIGHT));
    keyStrings[KEY_UP]                  = strdup(lang_get_txt(E_TXT_MENU_CTRL_UP));
    keyStrings[KEY_DOWN]                = strdup(lang_get_txt(E_TXT_MENU_CTRL_DOWN));
}

static void game_draw_object(tVector pos, uint8_t dir, tVector size, uint8_t axis, tAnimation *anim, BITMAP *sprite, BITMAP *buffer)
{
    if (sprite != NULL)
    {
        int16_t drawX, drawY;
        
        //assign current frame sub-bitmap of entity 
        BITMAP *objectSprite = create_sub_bitmap(sprite, anim->frame * size.x, 0, size.x, size.y);
        
        //check alignment axis
        switch (axis)
        {
            case E_ENT_AXIS_DOWN:
                drawX = pos.x - (size.x >>1);
                drawY = pos.y - size.y;
            break;
            case E_ENT_AXIS_UP:
                drawX = pos.x - (size.x >>1);
                drawY = pos.y;
            break;
            case E_ENT_AXIS_LEFT_DOWN:
                drawX = pos.x;
                drawY = pos.y - size.y;
            break;
            case E_ENT_AXIS_RIGHT_DOWN:
                drawX = (pos.x + size.x);
                drawY = pos.y - size.y;
            break;
            case E_ENT_AXIS_CENTER:
            default:
                drawX = pos.x - (size.x >>1);
                drawY = pos.y - (size.y >>1);
            break;
        }
        
        //draw game object
        if (buffer != NULL && objectSprite != NULL)
        {
            if (dir == E_ENT_DIR_RIGHT)               
                draw_sprite(buffer, objectSprite, drawX, drawY);              
            else
                draw_sprite_h_flip(buffer, objectSprite, drawX, drawY);                                        
        }
        else
        {
            if (buffer == NULL) abort_on_error("buffer pointer is null\n");
            if (objectSprite == NULL) abort_on_error("entity sprite pointer is null\n");
        }
        
        //need to destroy bitmap each time
        destroy_bitmap(objectSprite);
    }
}

void game_loading_text()
{
    printf("\rStarting %s v%i.%i (%i%%)", GAME_TITLE, MAJOR_VERSION, MINOR_VERSION, loadingProgress);
    loadingProgress += 9; 
}