/********************************************************************
* Main game file
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "allegro.h"

#include "globals.h"
#include "game.h"
#include "entity.h"
#include "collisions.h"
#include "scroll.h"
#include "player.h"
#include "objects.h"

uint8_t map[MAP_TILE_H][MAP_TILE_W] =
{
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

BITMAP *tiles[NUM_TILES];
BITMAP *mapScreen;
BITMAP *buffer;
RGB* gamePal;

bool gameExit = false;
int fps;
int frameCount;
tScroll scroll;

//function declarations
void create_rand_map();
void draw_map(BITMAP *mapScreen);
void init_level();

//update fps callback
static void update_fps(void)
{
    fps = frameCount;
    frameCount = 0;
}
END_OF_FUNCTION(update_fps);

int main()
{    
    if (allegro_init() != 0)
        return 1;

    install_timer();
    install_keyboard(); 
    
    fps = 0;
    frameCount = 0;
    LOCK_VARIABLE(fps);
    LOCK_VARIABLE(frameCount);
    install_int_ex(update_fps, BPS_TO_TIMER(1));

    int trace = 0;

    set_color_depth(8);

    if (set_gfx_mode(GFX_AUTODETECT, SCREEN_X, SCREEN_Y, 0, 0) != 0) 
    {
        if (set_gfx_mode(GFX_SAFE, SCREEN_X, SCREEN_Y, 0, 0) != 0) 
        {
            set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
            allegro_message("Unable to set any graphic mode\n%s\n", allegro_error);
            return 1;
        }
    }
    
    game_init();

    //load tiles
    tiles[0] = load_bmp("res/tiles/001.bmp", desktop_palette);
    tiles[1] = load_bmp("res/tiles/002.bmp", NULL);
    tiles[2] = load_bmp("res/tiles/003.bmp", NULL);

    /* set the color palette */
    set_palette(desktop_palette);
    
    //initialize buffer screen
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    clear_to_color(buffer, 3);

    entity_system_init();
    object_system_init();
    debug_init();

    //initialize map bitmap
    mapScreen = create_bitmap(GAME_W, GAME_H);
    //create scroll
    scroll = scroll_create((tVector){GAME_W,GAME_H},(tVector){((MAP_TILE_W * TILE_W) - GAME_W) - 1,((MAP_TILE_H * TILE_H) - GAME_H) - 1});
    
    init_level();
    scroll_init(&scroll);
    
    //main loop
    while (!gameExit)
    {
        trace = retrace_count;

        if (key[KEY_ESC])
            gameExit = true;

        switch(game.state)
        {
            case E_PLAY_LEVEL_GAME_STATE:
                entities_update();
                scroll_update(&scroll, &get_entity(PLAYER_ENTITY_ID)->pos);        

                draw_map(mapScreen);
                entities_draw(mapScreen, &scroll);
            break;
            default:
                gameExit = true;
            break;
        }
        
        game_debug_info();
        
        game_draw();
        
        vsync();
        
        frameCount++;

        if (trace != retrace_count)
            deltaTime = (double)(retrace_count-trace);
        
        /*
        -850-780fps: draw mapScreen to screen directly
        -850-719fps: draw mapScreen to buffer and blit to screen <-

        -600-570: draw_sprite mapScreen to buffer
        -700-680: blit mapScreen to buffer <--
        */
    }

    return 0;
}
END_OF_MAIN()

void create_rand_map()
{
    //Randomize map
    for (int j = 0; j < (MAP_TILE_H); j++)
    {
        for (int i = 0; i < (MAP_TILE_W); i++)        
        {
            map[i][j] = rand() % NUM_TILES;
        }    
    }   
}

void draw_map(BITMAP *mapScreen)
{
    uint8_t tileNum;
    int16_t lx = (GAME_W / TILE_W) + 1; //limit scroll x
    int16_t ly = (GAME_H / TILE_H);     //limit scroll y
    int16_t sx = scroll.pos.x % TILE_W; //tile pos x on scroll
    int16_t sy = scroll.pos.y % TILE_H; //tile pos y on scroll
    int16_t tx = scroll.pos.x / TILE_W; //tile num x on scroll
    int16_t ty = scroll.pos.y / TILE_H; //tile num y on scroll   

    clear_to_color(mapScreen, 1);

    for (int y = 0; y < ly; y++)
    {
        for (int x = 0; x < lx; x++)        
        {
            tileNum = map[y + ty][x + tx];

            /* blit tile*/
            if (tileNum != 0)            
                draw_sprite(mapScreen, tiles[tileNum - 1], (x * 16) - sx, (y * 16) - sy);
        }    
    }    
}

void init_level()
{
    //player    
    entity_create((tVector){16,10}, load_bmp("res/004.bmp",NULL), E_PLAYER_ENTITY_TYPE, NULL, &player_update);
    
    //test objects
    entity_create((tVector){60,110}, load_bmp("res/stone.bmp",NULL), E_STONE_OBJECT_TYPE, &object_create, &object_update);
    entity_create((tVector){50,120}, load_bmp("res/stone.bmp",NULL), E_STONE_OBJECT_TYPE, &object_create, &object_update);
    entity_create((tVector){100,160}, load_bmp("res/object.bmp",NULL), E_GEM_OBJECT_TYPE, &object_create, &object_update);
    entity_create((tVector){30,120}, load_bmp("res/object.bmp",NULL), E_GEM_OBJECT_TYPE, &object_create, &object_update);
    entity_create((tVector){50,100}, load_bmp("res/stone.bmp",NULL), E_STONE_OBJECT_TYPE, &object_create, &object_update);
    entity_create((tVector){90,150}, load_bmp("res/object.bmp",NULL), E_GEM_OBJECT_TYPE, &object_create, &object_update);
}

void game_init()
{
    game.state = E_PLAY_LEVEL_GAME_STATE;
    game.prevState = E_PLAY_LEVEL_GAME_STATE;
}

void game_draw()
{
    blit(mapScreen, buffer, 0, 0, GAME_X, GAME_Y, GAME_W, GAME_H);
        
    debug_draw(buffer);

    //blit to screen
    blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);
}

void game_debug_info()
{
    //debug info
    show_debug("FPS: %d", fps);
    show_debug("s.x: %d", scroll.pos.x);
    show_debug( "p.vX: %f", fixtof(get_entity(PLAYER_ENTITY_ID)->fixVel.x));
    show_debug( "p.vY: %f", fixtof(get_entity(PLAYER_ENTITY_ID)->fixVel.y));
    show_debug( "p.x: %d", get_entity(PLAYER_ENTITY_ID)->pos.x);
    show_debug( "p.y: %d", get_entity(PLAYER_ENTITY_ID)->pos.y);
}