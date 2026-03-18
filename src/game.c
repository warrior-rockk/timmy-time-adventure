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

#include "data/gdata.h"
#include "data/judata.h"

#define TRACE_FLAG  "[GAME]"

//debug option structure
#ifdef DEBUGMODE
    tDebugOptions debugOptions = {0};   
#endif

double deltaTime;                   //deltaTime
uint8_t gameExit = false;           //flag to exit to main

BITMAP *buffer;                     //screen buffer
BITMAP *worldScreen;                //map window buffer
RGB* gamePal;                       //game palette
FONT *gameFont;                     //game font
SAMPLE *gameSfx[E_SFX_GAME_NUM];    //game sfx
MIDI* gameMusic;                    //current MIDI game music
DATAFILE_INDEX *gameDataIndex; 
tGame game;                         //game structure
tSequence gameSeq;                  //game sequence
tScroll scroll;                     //game scroll
tLevelDataFile levelDataFile[E_GAME_NUM_LEVELS];    //level files
DATAFILE *levelDAT;

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

static void game_load_level(uint8_t numLevel);
static void game_destroy_level();
static void game_do_fade();
static void game_pause_sound();
static void game_resume_sound();
static void game_hud_init();
static void game_hud_update();
static void game_hud_draw();
static void game_debug_update();
static void game_debug_info();

void game_update()
{   
    timer_start_frame();
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
                    
                    BITMAP *logo = load_datafile_object_indexed(gameDataIndex, WARCOM_BMP)->dat;
                    
                    draw_sprite(buffer, logo, (SCREEN_W>>1) - (logo->w>>1), (SCREEN_H>>1) - (logo->h>>1));    
                    destroy_bitmap(logo);
                    
                    textout_centre_ex(buffer, gameFont, "WARCOM SOFT 2026", SCREEN_W>>1, SCREEN_H - 16, 30, 251);

                    gameMusic = load_datafile_object_indexed(gameDataIndex, WARCOM_MID)->dat;
                    music_play(gameMusic, 0);
                                        
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
                    clear(buffer);
                                        
                    BITMAP *logo = load_datafile_object_indexed(gameDataIndex, DOSCLUB_BMP)->dat;
                                        
                    draw_sprite(buffer, logo, (SCREEN_W>>1) - (logo->w>>1), (SCREEN_H>>1) - (logo->h>>1));    
                    destroy_bitmap(logo);   
                                        
                    gameMusic = load_datafile_object_indexed(gameDataIndex, DOSCLUB_MID)->dat;
                    music_play(gameMusic, 0);
                    
                    gameSeq.step++;
                break;
                case 1:
                    if (music_get_pos() < 0 || input_any_key_pressed())
                    {
                        game.state = E_GAME_ST_TITLE;
                        gameSeq.timeCounter = 0;
                        gameSeq.step = 0;
                        game.fadeOut = true;
                        music_stop();
                    }                    
                break;
            }
        break;
        case E_GAME_ST_TITLE:
            switch (gameSeq.step)
            {
                case 0:
                    clear(buffer);                    
                    game.fadeIn = true;
                    gameSeq.step++;                    
                case 1:
                    textout_centre_ex(buffer, gameFont, "DOS PLATFORM GAME", SCREEN_W>>1, SCREEN_H>>1, WHITE_COLOR, BLACK_COLOR);
                    textout_centre_ex(buffer, gameFont, "PRESS KEY TO START", SCREEN_W>>1, (SCREEN_H>>1) + 16, WHITE_COLOR, BLACK_COLOR);
                    
                    if (input_any_key_pressed())
                    {
                        if (input_key_press(E_G_KEY_EXIT))
                        {
                            game.state = E_GAME_ST_EXIT;
                        }
                        else
                        {
                            game.state = E_GAME_ST_INIT;
                            game.fadeOut = true;
                        }
                        gameSeq.timeCounter = 0;
                        gameSeq.step = 0;
                    }
                break;
            }
        break;
        case E_GAME_ST_INIT:
            game.lives          = GAME_INI_LIVES;
            game.life           = GAME_INI_LIFE;
            game.score          = 0;
            game.loseLive       = false;
            game.actualLevel    = 0;     
                        
            game.state = E_GAME_ST_LOAD_LEVEL;            
        break;
        case E_GAME_ST_LOAD_LEVEL:
            clear_to_color(buffer, BLACK_COLOR);
            
            object_system_init();
            enemy_system_init();

            game_load_level(game.actualLevel);
            
            game.state = E_GAME_ST_INIT_LEVEL;
        break;        
        case E_GAME_ST_INIT_LEVEL:
            switch (gameSeq.step)
            {
                case 0: //Init level data and entities
                    game.life       = GAME_INI_LIFE;            
                    game.loseLive   = false;            
                    game.viewMap    = true;

                    entities_init();
                    scroll_init(&scroll, &entity_get(PLAYER_ENTITY_ID)->pos);
                    scroll_update(&scroll, &entity_get(PLAYER_ENTITY_ID)->pos);
                    game_hud_init();                    

                    entities_update(&scroll);
                    scroll_update(&scroll, &entity_get(PLAYER_ENTITY_ID)->pos);
                    game_hud_update();                    

                    music_play(gameMusic, -1);
                    
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
                        map_draw(worldScreen, &scroll, false);                    
                        entities_draw(worldScreen, &scroll);                    
                        map_draw(worldScreen, &scroll, true);                    
                        game_hud_draw();
                        
                        gameSeq.timeCounter += clock_tick_get();
                    }
                break;                
            }
        break;
        case E_GAME_ST_PLAY_LEVEL:            
            entities_update(&scroll);
            scroll_update(&scroll, &entity_get(PLAYER_ENTITY_ID)->pos);        
            game_hud_update();

            map_draw(worldScreen, &scroll, false);
            entities_draw(worldScreen, &scroll);
            map_draw(worldScreen, &scroll, true);                    
            game_hud_draw();           

            //check game lose life
            if (game.loseLive)
            {                
                game.lives--;                
                music_stop(gameMusic);
                game.state = E_GAME_ST_LOSE_LIVE;                
            }            

            //game time
            if (clock_tick_1sec_get())
                game.time--;
            
            if (input_key_down(E_G_KEY_PAUSE))
                game.state = E_GAME_ST_PAUSE_LEVEL;

            if (game.levelComplete)
                game.state = E_GAME_ST_COMPLETE_LEVEL;

            #ifdef DEBUGMODE
                if (key[KEY_R])
                    game.state = E_GAME_ST_INIT_LEVEL;
                
                if (key[KEY_C])
                    game.state = E_GAME_ST_COMPLETE_LEVEL;

                if (input_key_press(E_G_KEY_EXIT))
                    game.state = E_GAME_ST_DESTROY_LEVEL;            
            #endif
        break;
        case E_GAME_ST_PAUSE_LEVEL:            
            map_draw(worldScreen, &scroll, false);
            entities_draw(worldScreen, &scroll);
            map_draw(worldScreen, &scroll, true);                    
            game_hud_draw();           
            
            textprintf_centre_ex(worldScreen, gameFont, GAME_W >> 1, GAME_H >> 1,  WHITE_COLOR, BLACK_COLOR, "PAUSE");
            
            game_pause_sound();

            if (input_key_down(E_G_KEY_PAUSE))
            {
                game.state = E_GAME_ST_PLAY_LEVEL;
                game_resume_sound();
            }
        break;
        case E_GAME_ST_LOSE_LIVE:
            scroll_update(&scroll, &entity_get(PLAYER_ENTITY_ID)->pos);        
            
            entities_draw(worldScreen, &scroll);
                       

            //TODO: replace with the duration of dead music
            if (gameSeq.timeCounter >= GAME_DEAD_WAIT_TIME)
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
            }
            else
                gameSeq.timeCounter += clock_tick_get();            
        break;
        case E_GAME_ST_COMPLETE_LEVEL:
            switch (gameSeq.step)
            {
                case 0:
                    scroll_update(&scroll, &entity_get(PLAYER_ENTITY_ID)->pos);                    
                    entities_draw(worldScreen, &scroll);

                    music_stop(gameMusic);

                    //TODO: replace with the duration of complete music
                    if (gameSeq.timeCounter >= GAME_DEAD_WAIT_TIME)
                    {                
                        gameSeq.timeCounter = 0;
                        gameSeq.step++;                        
                    }
                    else
                        gameSeq.timeCounter += clock_tick_get();
                break;
                case 1:
                    //obtain score for time left                         
                    if (game.time > 0 )
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
                break;
                case 2:
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
                case 3:
                    MY_TRACE_FLAG( "Completed level\n");
                    game_destroy_level();
                    game.actualLevel++;
                    gameSeq.step = 0;          
                    game.levelComplete = false;          
                    if (game.actualLevel == E_GAME_NUM_LEVELS)
                        game.state = E_GAME_ST_ENDING;
                    else    
                        game.state = E_GAME_ST_LOAD_LEVEL;
                break;
            }
        break;
        case E_GAME_ST_GAME_OVER:            
            switch (gameSeq.step)
            {
                case 0:
                    game_destroy_level();                            
                    textout_centre_ex(buffer, gameFont, "GAME OVER", SCREEN_W>>1, SCREEN_H>>1, WHITE_COLOR, BLACK_COLOR);
                    game.fadeIn = true;
                    MY_TRACE_FLAG( "Game Over\n");
                    gameSeq.step++;
                break;
                case 1:
                    if (gameSeq.timeCounter >= 800 || input_any_key_pressed())
                    {
                        game.state = E_GAME_ST_TITLE;
                        gameSeq.timeCounter = 0;
                        gameSeq.step = 0;
                        game.fadeOut = true;
                    }
                    else
                        gameSeq.timeCounter += clock_tick_get();
                break;
            }
        break;
        case E_GAME_ST_ENDING:            
            switch (gameSeq.step)
            {
                case 0:
                    textout_centre_ex(buffer, gameFont, "CONGRATULATIONS!", SCREEN_W>>1, SCREEN_H>>1, WHITE_COLOR, BLACK_COLOR);
                    game.fadeIn = true;

                    gameSeq.step++;
                break;
                case 1:
                    if (gameSeq.timeCounter >= 800 || input_any_key_pressed())
                    {
                        game.state = E_GAME_ST_TITLE;
                        gameSeq.timeCounter = 0;
                        gameSeq.step = 0;
                        game.fadeOut = true;
                    }
                    else
                        gameSeq.timeCounter += clock_tick_get();
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

void game_init()
{
    MY_TRACE_FLAG( "Init game\n");

    //create game data file index for fast open individual data objects
    gameDataIndex = create_datafile_index("game.dat");

    //game palette
    gamePal = load_datafile_object_indexed(gameDataIndex, INTRO_PAL)->dat;
    set_palette(gamePal);
    
    //loads game font 
    gameFont = grab_font_from_bitmap(load_datafile_object_indexed(gameDataIndex, FONT4_BMP)->dat);
    
    //initialize buffer screen
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    clear(buffer);

    //load hud image
    hud.hudImg = load_datafile_object_indexed(gameDataIndex, HUD_BMP)->dat;
    //hud.hudImg = load_bmp("res/game/hud.bmp", NULL);
    hud.hudLifeOff = create_bitmap(15, 14);
    hud.hudLifeOn  = create_bitmap(15, 14);
    blit(hud.hudImg, hud.hudLifeOn, 65, 5, 0, 0, 15, 14);
    blit(hud.hudImg, hud.hudLifeOff, 99, 5, 0, 0, 15, 14);

    entity_system_init();
    collision_system_init();    
    debug_init();
    timer_init(GAME_CLOCK_TICK);
    sfx_init(load_datafile_object_indexed(gameDataIndex, POINT_WAV)->dat, E_SFX_NUM_VOICES);
    
    
    //load game sfx
    gameSfx[E_SFX_GAME_POINT]       = load_datafile_object_indexed(gameDataIndex, POINT_WAV)->dat;
    gameSfx[E_SFX_GAME_POINT_END]   = load_datafile_object_indexed(gameDataIndex, POINTEND_WAV)->dat;

    //initialize map bitmap
    worldScreen = create_bitmap(GAME_W, GAME_H);

    //initialize levels data
    //levelDataFile[E_GAME_LEVEL_TEST].mapFile        = "res/maps/level00.bin";
    //levelDataFile[E_GAME_LEVEL_TEST].tileFile       = "res/tiles/tsheet.bmp";
    //levelDataFile[E_GAME_LEVEL_TEST].musicFile      = NULL;            
    levelDataFile[E_GAME_LEVEL_JURASSIC].mapFile        = "jurassic.bin";
    levelDataFile[E_GAME_LEVEL_JURASSIC].dataFile       = "jurassic.dat";
    levelDataFile[E_GAME_LEVEL_JURASSIC].tileFileIndex  = JURASSIC_BMP;
    levelDataFile[E_GAME_LEVEL_JURASSIC].palFileIndex   = JURASSIC_PAL;
    levelDataFile[E_GAME_LEVEL_JURASSIC].musicFileIndex = JUNGLE_MID;
    //levelDataFile[E_GAME_LEVEL_WEST].mapFile        = "res/maps/west.bin";
    //levelDataFile[E_GAME_LEVEL_WEST].tileFile       = "res/tiles/west.bmp";
    //levelDataFile[E_GAME_LEVEL_WEST].musicFile      = NULL;
    //levelDataFile[E_GAME_LEVEL_MEDIEVAL].mapFile    = "res/maps/medieval.bin";
    //levelDataFile[E_GAME_LEVEL_MEDIEVAL].tileFile   = "res/tiles/medieval.bmp";
    //levelDataFile[E_GAME_LEVEL_MEDIEVAL].musicFile  = NULL;
        
    #ifdef DEBUGMODE
        game.state      = E_GAME_ST_INIT;
    #else
        game.state      = E_GAME_ST_LOGO;
    #endif
    game.prevState      = E_GAME_ST_LOAD_LEVEL;
    game.actualLevel    = 0;
    game.lives          = GAME_INI_LIVES;
    game.life           = GAME_INI_LIFE;
    game.score          = 0;
    game.fadeState      = E_FADED_IN;    
    game.fadeOut        = true;
    hud.refresh         = E_REFRESH_HUD_ALL;
    game.viewMap        = false;

    gameSeq.step = 0;
    gameSeq.timeCounter = 0;
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
    timer_end_frame(&deltaTime);
    
    //blit to screen
    blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);  
    
    //do pending fades
    game_do_fade();
}

static void game_debug_update()
{
#ifdef DEBUGMODE

    if (debugOptions.showDebugInfo)
        game_debug_info();
    
    //force game exit
    if (key[KEY_X] && (key_shifts & KB_CTRL_FLAG))
        game.state = E_GAME_ST_EXIT;
    
    if (input_key_down(E_G_KEY_D))
        debugOptions.showDebugInfo = debugOptions.showDebugInfo < 2 ? debugOptions.showDebugInfo + 1 : 0;
    if (input_key_down(E_G_KEY_S))
        debugOptions.stepByStep = !debugOptions.stepByStep;
    if (input_key_down(E_G_KEY_I))
    {
        debugOptions.invencible = !debugOptions.invencible;
        game.life = 3;
    }

    //trace state          
    if (game.state != game.prevState)
    {
        MY_TRACE_FLAG("Game changes from state %i to state %i\n", game.prevState, game.state);
    }
#endif
}

static void game_debug_info()
{
    //debug info
    show_debug("FPS: %d", fps_get());
    //show_debug("s.x: %d, s.x: %d", scroll.pos.x, scroll.pos.y);
    //show_debug( "p.vX: %f", fixtof(entity_get(PLAYER_ENTITY_ID)->fixVel.x));
    //show_debug( "p.vY: %f", fixtof(entity_get(PLAYER_ENTITY_ID)->fixVel.y));
    //show_debug( "p.x: %d", entity_get(PLAYER_ENTITY_ID)->pos.x);
    //show_debug( "p.y: %d", entity_get(PLAYER_ENTITY_ID)->pos.y);
    //show_debug("Lives:%i Life:%i", game.lives, game.life);
    //show_debug("State: %i", game.state);
}

//testing
static void game_load_level(uint8_t numLevel)
{
    //load level data file
    levelDAT = load_datafile(levelDataFile[numLevel].dataFile);
    
    //load level palette
    gamePal = levelDAT[levelDataFile[numLevel].palFileIndex].dat;

    //load map and entities    
    map_load(levelDataFile[numLevel].mapFile, (BITMAP *)levelDAT[levelDataFile[numLevel].tileFileIndex].dat, (tVector){GAME_W, GAME_H});
    
    //TODO: read scroll mode from map
    game.scrollMode = E_SCROLL_BY_WINDOW_Y_MODE;
    //TODO: read level time from map
    game.time = 300;

    //calculate scroll dimensions based on map dimensions
    tVector mapDimension = map_get_dimensions();
    mapDimension.x = mapDimension.x - GAME_W;
    mapDimension.y = mapDimension.y - GAME_H;
    
    //create scroll    
    scroll = scroll_create((tVector){GAME_W,GAME_H}, mapDimension, game.scrollMode);   
    
    //load music level
    gameMusic = (MIDI *)levelDAT[levelDataFile[numLevel].musicFileIndex].dat;
    
}

void game_destroy()
{
    MY_TRACE_FLAG("Destroying game\n");
    
    //free fonts
    destroy_font(gameFont);
    
    //free samples
    for (uint8_t i = 0; i < E_SFX_GAME_NUM; i++)
    {
        if (gameSfx[i])
        {
            destroy_sample(gameSfx[i]);
            gameSfx[i] = NULL;
        }
    }

    //destroy music
    if (gameMusic)
    {
        destroy_midi(gameMusic);
        gameMusic = NULL;
    }

    //destroy game data index
    destroy_datafile_index(gameDataIndex);
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
    if (game.fadeIn)
    {
        if (game.fadeState == E_FADED_OFF)
        {
            fade_in(gamePal, GAME_FADE_SPEED);
            game.fadeState = E_FADED_IN;
        }
        
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

    textout_centre_ex(buffer, gameFont, "LIVES",   HUD_POSITION_X + 22, HUD_POSITION_Y - 7, WHITE_COLOR, -1);
    textout_centre_ex(buffer, gameFont, "LIFE",    HUD_POSITION_X + 90, HUD_POSITION_Y - 7, WHITE_COLOR, -1);
    textout_centre_ex(buffer, gameFont, "SCORE",   HUD_POSITION_X + 160, HUD_POSITION_Y - 7, WHITE_COLOR, -1);
    textout_centre_ex(buffer, gameFont, "TIME",    HUD_POSITION_X + 220, HUD_POSITION_Y - 7, WHITE_COLOR, -1);

    textout_centre_ex(buffer, gameFont, "X",   HUD_POSITION_X + 23, HUD_POSITION_Y + 5, WHITE_COLOR, -1);

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
        if (!(game.score % SCORE_POINT_EXTRA_LIFE) && game.score > 0)
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
        textprintf_centre_ex(buffer, gameFont, HUD_POSITION_X + 31, HUD_POSITION_Y + 5, WHITE_COLOR, BLACK_COLOR, "%u", game.lives);
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
        textprintf_centre_ex(buffer, gameFont, HUD_POSITION_X + 160, HUD_POSITION_Y + 5, WHITE_COLOR, BLACK_COLOR, "%04u", game.score);
    }

    //update time
    if (CHECK_FLAG(hud.refresh, E_REFRESH_HUD_TIME))
    {
        textprintf_centre_ex(buffer, gameFont, HUD_POSITION_X + 220, HUD_POSITION_Y + 5, WHITE_COLOR, BLACK_COLOR, "%03u", game.time);
    }

    //reset refresh flags
    hud.refresh = 0x00;
}

static void game_pause_sound()
{
    for (uint8_t i = 0; i < E_SFX_NUM_VOICES; i++)
    {
        sfx_pause(i);
    }

    music_pause();
}

static void game_resume_sound()
{
    for (uint8_t i = 0; i < E_SFX_NUM_VOICES; i++)
    {
        sfx_resume(i);
    }

    music_resume();
}