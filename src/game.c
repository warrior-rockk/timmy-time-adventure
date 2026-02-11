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

//game structure
tGame game;

//debug option structure
#ifdef DEBUGMODE
tDebugOptions debugOptions;
#endif

BITMAP *buffer;
BITMAP *worldScreen;
RGB* gamePal;
double deltaTime;
uint8_t gameExit = false;
tScroll scroll;

tLevelDataFile levelDataFile[E_GAME_NUM_LEVELS];

static void game_load_level(uint8_t numLevel);
static void game_debug_info();
static void destroy_level();

void game_update()
{   
    timer_start_frame();
    input_keys_update();
  
    switch(game.state)
    {
        case E_GAME_ST_LOAD_LEVEL:
            game_load_level(E_GAME_LEVEL_TEST);
            game.state = E_GAME_ST_INIT;
        break;
        case E_GAME_ST_INIT:
            game.loseLive = false;
            game.life = GAME_INI_LIFE;
            
            entities_init();
            scroll_init(&scroll);
            game.state = E_GAME_ST_PLAY_LEVEL;
        break;
        case E_GAME_ST_PLAY_LEVEL:
            entities_update(&scroll);
            scroll_update(&scroll, &entity_get(PLAYER_ENTITY_ID)->pos);        

            map_draw(worldScreen, &scroll, (tVector){GAME_W, GAME_H});
            entities_draw(worldScreen, &scroll);

            if (key[KEY_R])
                game.state = E_GAME_ST_INIT;
            
            if (input_key_press(G_KEY_EXIT))
                game.state = E_GAME_ST_DESTROY_LEVEL;            

            if (game.loseLive)
            {
                game.lives--;
                game.state = E_GAME_ST_LOSE_LIVE;
            }
        break;
        case E_GAME_ST_LOSE_LIVE:
            scroll_update(&scroll, &entity_get(PLAYER_ENTITY_ID)->pos);        
            
            entities_draw(worldScreen, &scroll);

            if (get_clock_count(60))
            {                
                game.state = game.lives > 0 ? E_GAME_ST_INIT : E_GAME_ST_GAME_OVER;                
            }
        break;
        case E_GAME_ST_GAME_OVER:
            game.state = E_GAME_ST_DESTROY_LEVEL;
        break;
        case E_GAME_ST_DESTROY_LEVEL:
            destroy_level();
            game.state = E_GAME_ST_EXIT;
        break;
        case E_GAME_ST_EXIT:
        default:
            game_destroy();
            gameExit = true;
        break;
    }
    
    #ifdef DEBUGMODE
        if (debugOptions.showDebugInfo)
            game_debug_info();
        
        //force game exit
        if (key[KEY_X] && (key_shifts & KB_CTRL_FLAG))
            game.state = E_GAME_ST_EXIT;
        
        if (input_key_pressed(G_KEY_D))
            debugOptions.showDebugInfo = debugOptions.showDebugInfo < 2 ? debugOptions.showDebugInfo + 1 : 0;
        if (input_key_pressed(G_KEY_S))
            debugOptions.stepByStep = !debugOptions.stepByStep;
    #endif
}


static void destroy_level()
{
    //destroy entities
    entity_destroy_all();
    object_system_destroy();
    enemy_system_destroy();
    //unload map and map resources
    map_unload();
}

void game_init()
{
    /* set the color palette */
    //temporaly
    free(load_bmp("res/tiles/tsheet.bmp", desktop_palette));
    set_palette(desktop_palette);
    
    //initialize buffer screen
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    clear_to_color(buffer, BORDER_COLOR);

    entity_system_init();
    collision_system_init();
    object_system_init();
    enemy_system_init();

    debug_init();
    #ifdef DEBUGMODE
        debugOptions.showDebugInfo = true;
    #endif
    timer_init(GAME_CLOCK_TICK);

    //initialize map bitmap
    worldScreen = create_bitmap(GAME_W, GAME_H);

    //initialize levels data
    levelDataFile[E_GAME_LEVEL_TEST].mapFile    = "res/maps/level00.bin";
    levelDataFile[E_GAME_LEVEL_TEST].tileFile   = "res/tiles/tsheet.bmp";
    
    game.state          = E_GAME_ST_LOAD_LEVEL;
    game.prevState      = E_GAME_ST_LOAD_LEVEL;
    game.actualLevel    = E_GAME_LEVEL_TEST;    
    game.lives          = GAME_INI_LIVES;
    game.life           = GAME_INI_LIFE;
    game.score          = 0;
}

void game_draw()
{   
     /*
    -850-780fps: draw mapScreen to screen directly
    -850-719fps: draw mapScreen to buffer and blit to screen <-

    -600-570: draw_sprite mapScreen to buffer
    -700-680: blit mapScreen to buffer <--
    */

    //blit worldScreen on buffer (centered on screen)
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
}

static void game_debug_info()
{
    //debug info
    show_debug("FPS: %d", get_fps());
    //show_debug("s.x: %d, s.x: %d", scroll.pos.x, scroll.pos.y);
    show_debug( "p.vX: %f", fixtof(entity_get(PLAYER_ENTITY_ID)->fixVel.x));
    show_debug( "p.vY: %f", fixtof(entity_get(PLAYER_ENTITY_ID)->fixVel.y));
    //show_debug( "p.x: %d", entity_get(PLAYER_ENTITY_ID)->pos.x);
    //show_debug( "p.y: %d", entity_get(PLAYER_ENTITY_ID)->pos.y);
    show_debug("Lives:%i Life:%i", game.lives, game.life);
}

//testing
static void game_load_level(uint8_t numLevel)
{
    map_load(levelDataFile[numLevel].mapFile, levelDataFile[numLevel].tileFile);
    
    //create scroll
    tVector mapDimension = map_get_dimensions();
    scroll = scroll_create((tVector){GAME_W,GAME_H},(tVector){(mapDimension.x - GAME_W) - 1,(mapDimension.y - GAME_H) - 1});
    scroll_init(&scroll);    
}

void game_destroy()
{
    //TODO: NOTHING FOR THE MOMENT. UNLOAD GENERAL RESOURCES NO RELATIVE TO LEVEL
}