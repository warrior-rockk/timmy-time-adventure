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

//game structure
struct game
{
    uint8_t state;
    uint8_t prevState;
} game;

BITMAP *buffer;
BITMAP *worldScreen;
RGB* gamePal;
double deltaTime;
uint8_t gameExit = false;
tScroll scroll;

static void game_load_level();
static void game_debug_info();
static void destroy_level();

void game_update()
{   
    timer_start_frame();

    if (key[KEY_ESC])
        gameExit = true;

    switch(game.state)
    {
        case E_LOAD_LEVEL_GAME_STATE:
            game_load_level();
            game.state = E_INIT_LEVEL_GAME_STATE;
        break;
        case E_INIT_LEVEL_GAME_STATE:
            entities_init();
            scroll_init(&scroll);
            game.state = E_PLAY_LEVEL_GAME_STATE;
        break;
        case E_PLAY_LEVEL_GAME_STATE:
            entities_update(&scroll);
            scroll_update(&scroll, &get_entity(PLAYER_ENTITY_ID)->pos);        

            map_draw(worldScreen, &scroll, (tVector){GAME_W, GAME_H});
            entities_draw(worldScreen, &scroll);

            if (key[KEY_R])
                game.state = E_INIT_LEVEL_GAME_STATE;
            
            if (key[KEY_E])
                game.state = E_DESTROY_LEVEL_GAME_STATE;
        break;
        case E_DESTROY_LEVEL_GAME_STATE:
            destroy_level();
            game.state = E_EXIT_GAME_STATE;
        break;
        case E_EXIT_GAME_STATE:
            gameExit = true;
        break;
        default:
            gameExit = true;
        break;
    }
    
    game_debug_info();
}


static void destroy_level()
{
    entity_destroy_all();
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
    clear_to_color(buffer, 3);

    entity_system_init();
    collision_system_init();
    object_system_init();
    debug_init();
    timer_init(GAME_CLOCK_TICK);

    //initialize map bitmap
    worldScreen = create_bitmap(GAME_W, GAME_H);
    
    game.state      = E_LOAD_LEVEL_GAME_STATE;
    game.prevState  = E_LOAD_LEVEL_GAME_STATE;
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
    //draw debug info
    debug_draw(buffer);
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
    show_debug("s.x: %d", scroll.pos.x);
    show_debug( "p.vX: %f", fixtof(get_entity(PLAYER_ENTITY_ID)->fixVel.x));
    show_debug( "p.vY: %f", fixtof(get_entity(PLAYER_ENTITY_ID)->fixVel.y));
    show_debug( "p.x: %d", get_entity(PLAYER_ENTITY_ID)->pos.x);
    show_debug( "p.y: %d", get_entity(PLAYER_ENTITY_ID)->pos.y);
}

//testing
static void game_load_level()
{
    map_load();
    
    //create scroll
    tVector mapDimension = map_get_dimensions();
    scroll = scroll_create((tVector){GAME_W,GAME_H},(tVector){(mapDimension.x - GAME_W) - 1,(mapDimension.y - GAME_H) - 1});
    scroll_init(&scroll);    
}