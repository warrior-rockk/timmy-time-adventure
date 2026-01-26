/********************************************************************
* Main game file
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "allegro.h"

#include "globals.h"
#include "game.h"
#include "timer.h"
#include "entity.h"
#include "scroll.h"
#include "map.h"
#include "collisions.h"
#include "player.h"
#include "objects.h"

BITMAP *mapScreen;
BITMAP *buffer;
RGB* gamePal;
double deltaTime;
bool gameExit = false;
tScroll scroll;
tLevel level;

static void main_init();
void create_level();
void destroy_level();

int main()
{    
    main_init();
    
    game_init();

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
    mapScreen = create_bitmap(GAME_W, GAME_H);
    //create scroll
    scroll = scroll_create((tVector){GAME_W,GAME_H},(tVector){((MAP_TILE_W * TILE_W) - GAME_W) - 1,((MAP_TILE_H * TILE_H) - GAME_H) - 1});
    scroll_init(&scroll);
    
    //main loop
    while (!gameExit)
    {
        timer_start_frame();

        if (key[KEY_ESC])
            gameExit = true;

        switch(game.state)
        {
            case E_LOAD_LEVEL_GAME_STATE:
                create_level();
                map_load();
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

                map_draw(mapScreen, &scroll, (tVector){GAME_W, GAME_H});
                entities_draw(mapScreen, &scroll);

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
        game_draw();

        vsync();
        
        timer_end_frame(&deltaTime);
        
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

void create_level()
{
    //player    
    entity_create((tVector){40,20},(tVector){20,41}, load_bmp("res/004.bmp",NULL), E_PLAYER_ENTITY_TYPE, E_COLLISIONS_ON_PROPERTY, &player_init, NULL, &player_update);
    
    //test objects
    /*
    entity_create((tVector){60,110}, load_bmp("res/stone.bmp",NULL), E_STONE_OBJECT_TYPE, E_COLLISIONS_ON_PROPERTY, &object_init, &object_create, &object_update);
    entity_create((tVector){50,120}, load_bmp("res/stone.bmp",NULL), E_STONE_OBJECT_TYPE, 0x00, &object_init, &object_create, &object_update);
    entity_create((tVector){100,160}, load_bmp("res/object.bmp",NULL), E_GEM_OBJECT_TYPE, E_COLLISIONS_ON_PROPERTY, &object_init, &object_create, &object_update);
    entity_create((tVector){30,120}, load_bmp("res/object.bmp",NULL), E_GEM_OBJECT_TYPE, E_COLLISIONS_ON_PROPERTY, &object_init, &object_create, &object_update);
    entity_create((tVector){392,100}, load_bmp("res/stone.bmp",NULL), E_STONE_OBJECT_TYPE, 0x00, &object_init, &object_create, &object_update);
    entity_create((tVector){90,150}, load_bmp("res/object.bmp",NULL), E_GEM_OBJECT_TYPE, E_COLLISIONS_ON_PROPERTY, &object_init, &object_create, &object_update);
    */
}

void destroy_level()
{
    entity_destroy_all();
}

void game_init()
{
    game.state      = E_LOAD_LEVEL_GAME_STATE;
    game.prevState  = E_LOAD_LEVEL_GAME_STATE;
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
    show_debug("FPS: %d", get_fps());
    show_debug("s.x: %d", scroll.pos.x);
    show_debug( "p.vX: %f", fixtof(get_entity(PLAYER_ENTITY_ID)->fixVel.x));
    show_debug( "p.vY: %f", fixtof(get_entity(PLAYER_ENTITY_ID)->fixVel.y));
    show_debug( "p.x: %d", get_entity(PLAYER_ENTITY_ID)->pos.x);
    show_debug( "p.y: %d", get_entity(PLAYER_ENTITY_ID)->pos.y);
}

//general initialization
static void main_init()
{
    //set env var trace log file
    setenv("ALLEGRO_TRACE", "GAME.LOG", 1);

    TRACE("%s %i.%i\n", GAME_TITLE, MAJOR_VERSION, MINOR_VERSION);
    TRACE("Initializing systems and modules\n");
    
    //set unicode format
    //this is no longer necessary with vscode UTF-8 codification
    //set_uformat(U_ASCII);

    //TODO: option for pc speaker?
    /*
    //prompt for sound driver
    printf("Starting %s v%i.%i\n\n", GAME_TITLE, MAJOR_VERSION, MINOR_VERSION);      
    printf("Select sound system:\n\n");
    printf("1. Sound Blaster and compatible\n");
    printf("2. PC Speaker\n");
    printf("3. No sound\n\n");

    printf("Enter choice: ");
        switch (getkey())
    {
        case 0x31:
            TRACE("Sound option selected: Sound Blaster\n");
            sound_set_mode(SB_SND_MODE);
            break;
        case 0x32:
            TRACE("Sound option selected: Speaker\n");
            sound_set_mode(PC_SPEAKER_SND_MODE);    
            break;
        case 0x33:
            TRACE("Sound option selected: None\n");
            sound_set_mode(NO_SOUND_SND_MODE);
            break;
        default:
            exit(-1);
    }
    */

    printf("Starting %s v%i.%i\n", GAME_TITLE, MAJOR_VERSION, MINOR_VERSION);
    
    //initialize and install modules
    if (allegro_init() != 0)
        abort_on_error("Error iniciando libreria Allegro");
    if (install_timer() != 0)
        abort_on_error("Error iniciando el modulo timer");
    #ifdef ALLEGRO_USES_KEYBOARD
        if (install_keyboard() != 0)
            abort_on_error("Error iniciando el teclado");
    #endif
    #ifdef ALLEGRO_USES_MOUSE
        if (install_mouse() < 0)
            abort_on_error("Error iniciando el mouse");
    #endif
    #ifdef ALLEGRO_USES_SOUND
        if (sound_init() != 0)
            abort_on_error("Error iniciando el sonido");
    #endif

    TRACE("All system and modules initialized\n");
    
    //set video mode
    if (set_gfx_mode(GFX_VGA, SCREEN_X, SCREEN_Y, 0, 0) != 0)
        abort_on_error("Error seteando modo grafico");
    set_color_depth(8);
}