#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "allegro.h"

#include "globals.h"
#include "game.h"
#include "entity.h"
#include "collisions.h"
#include "scroll.h"
#include "objects.h"

struct player  
{
    tEntity ent;
    tColPoint colPoint[NUM_COL_POINTS];   
} player;

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
double deltaTime;
tScroll scroll;

//function declarations
void create_rand_map();
void draw_map(BITMAP *mapScreen);
void init_player();
void update_player();
void update_scroll();
void draw_player();
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
    
    //load tiles
    tiles[0] = load_bmp("res/tiles/001.bmp", desktop_palette);
    tiles[1] = load_bmp("res/tiles/002.bmp", NULL);
    tiles[2] = load_bmp("res/tiles/003.bmp", NULL);

    /* set the color palette */
    set_palette(desktop_palette);
    
    //initialize buffer screen
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    clear_to_color(buffer, 3);

    //initialize map bitmap
    mapScreen = create_bitmap(GAME_W, GAME_H);
    
    scroll = create_scroll((tVector){GAME_W,GAME_H},(tVector){((MAP_TILE_W * TILE_W) - GAME_W),((MAP_TILE_H * TILE_H) - GAME_H)});

    //init scroll
    //scroll.pos.x = 0;
    //scroll.pos.y = 0;
    
    init_player();
    init_level();
    scroll_init(&scroll);

    //main loop
    while (!gameExit)
    {
        trace = retrace_count;

        if (key[KEY_ESC])
            gameExit = true;

        update_player();
        //update_scroll();
        scroll_update(&scroll, &player.ent.pos);        
        entities_update();

        //clear_to_color(buffer, 3);
        clear_to_color(mapScreen, 1);
    
        draw_map(mapScreen);
        draw_player();
        entities_draw(mapScreen, &scroll);

        blit(mapScreen, buffer, 0, 0, GAME_X, GAME_Y, GAME_W, GAME_H);
        
        //debug
        textprintf_ex(buffer, font, 0, 0, 0, 3, "FPS: %d", fps); 
        textprintf_ex(buffer, font, 0, 8, 0, 3, "s.x: %d", scroll.pos.x);
        textprintf_ex(buffer, font, 0, 16, 0, 3, "p.vX: %f", fixtof(player.ent.vX));
        textprintf_ex(buffer, font, 0, 24, 0, 3, "p.vY: %f", fixtof(player.ent.vY));
        textprintf_ex(buffer, font, 0, 32, 0, 3, "p.x: %d", player.ent.pos.x);
        textprintf_ex(buffer, font, 0, 40, 0, 3, "p.y: %d", player.ent.pos.y);
        //textprintf_ex(buffer, font, 0, 48, 0, 3, "toc: %d", toc);
        //textprintf_ex(buffer, font, 0, 56, 0, 3, "delta: %f", deltaTime);
 
        //blit to screen
        blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);
        
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

void update_player()
{
    fixed accel_x = ftofix(0.06);
    fixed friction = ftofix(0.86);       //more friction, more sloppy (0.94-0.96 is like ice)
    fixed air_friction = ftofix(0.6);   //less than floor friction
    fixed gravity = ftofix(0.2);
    fixed accel_y = ftofix(4.0);        //jump acceleration
    fixed max_vel_x = ftofix(1.0);
    fixed max_vel_y = ftofix(6);

    int16_t floor = 160;
    
    fixed localFriction = player.ent.ground ? friction: air_friction;

    //update controls
    if (key[KEY_RIGHT] && player.ent.vX < max_vel_x)
        //player.ent.vX+= fixmul(accel_x, (itofix(1) - friction));
        player.ent.vX+= fixmul(accel_x, ftofix(deltaTime));

    if (key[KEY_LEFT] && player.ent.vX > -max_vel_x)
        player.ent.vX-= fixmul(accel_x, ftofix(deltaTime));

    if (key[KEY_Z] && player.ent.ground)
    {
        player.ent.vY = -accel_y;
        player.ent.jump = true;
        player.ent.ground = false;
    }

    //update vels
    if (!key[KEY_RIGHT] && !key[KEY_LEFT])
        player.ent.vX = fixmul(player.ent.vX, ftofix(pow(fixtof(friction), (deltaTime * fixtof(friction))))); //this the equivalent formula for vX *= friction with deltaTime
    
    if (player.ent.pos.y >= floor && !player.ent.jump)
    { 
        player.ent.vY = 0;
        player.ent.fY = itofix(floor);
        player.ent.ground = true;
    }
    else
    {
        player.ent.vY += player.ent.vY >= max_vel_y ? 0 : fixmul(gravity, ftofix(deltaTime));
        //player.ent.vY += fixmul(gravity, ftofix(deltaTime));
        player.ent.jump = false;
    }   
    
    //apply velocity
    player.ent.fX += fixmul(player.ent.vX, ftofix(deltaTime));
    player.ent.fY += fixmul(player.ent.vY, ftofix(deltaTime));

    //update position
    player.ent.pos.x = fixtoi(player.ent.fX);
    player.ent.pos.y = fixtoi(player.ent.fY);

    //check collisions
    
}

void draw_player()
{
    draw_sprite(mapScreen, player.ent.img, player.ent.pos.x - scroll.pos.x, player.ent.pos.y - scroll.pos.y);
}

void update_scroll()
{
    //test: follow player 
    //note: in sms games, the scroll moves when the player is offset to center in the direction of movement. A few pixels to right when move to right and viceversa
    //if the player is in the offset zone, the scroll doesn't move allowing a tiny zone with not scroll movement
    const uint16_t scrollOffset = 8;

    if (player.ent.pos.x > (GAME_W >> 1) + scroll.pos.x + scrollOffset)
        scroll.pos.x = player.ent.pos.x - (GAME_W >> 1) - scrollOffset;
    else if (player.ent.pos.x < (GAME_W >> 1) + scroll.pos.x - scrollOffset)
        scroll.pos.x = player.ent.pos.x - (GAME_W >> 1) + scrollOffset;
    
    //scroll limits
    if (scroll.pos.x > (((MAP_TILE_W * TILE_W) - GAME_W) - 1))
        scroll.pos.x = (((MAP_TILE_W * TILE_W) - GAME_W) - 1);
              
    if (scroll.pos.x < 0)
        scroll.pos.x = 0;
        
}

void init_player()
{
    //init player
    player.ent.pos.x = 0;
    player.ent.pos.y = 0;
    player.ent.fX = itofix(16);
    player.ent.fY = itofix(10);
    player.ent.vX = 0;
    player.ent.vY = 0;
    player.ent.img = load_bmp("res/004.bmp", NULL);
    player.ent.size.x = player.ent.img->w;
    player.ent.size.y = player.ent.img->h;
    
    collision_init_entity_points(&player.ent, player.colPoint);
}

void init_level()
{
    entity_add(player.ent);
    create_entity((tVector){100,200}, load_bmp("res/object.bmp",NULL), &object_gem_update);
}