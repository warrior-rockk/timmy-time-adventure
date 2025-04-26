#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "allegro.h"

//SMS resolution: 256x192 (testing 256x208: extra sms Y tile to center screen)
#define SCREEN_X        320
#define SCREEN_Y        240

#define GAME_W          256
#define GAME_H          208
#define GAME_X          (SCREEN_W>>1) - (GAME_W>>1)
#define GAME_Y          (SCREEN_H>>1) - (GAME_H>>1)

#define NUM_TILES       3
#define TILE_W          16
#define TILE_H          16

#define MAP_TILE_W      (GAME_W / TILE_W) * 2
#define MAP_TILE_H      (GAME_H / TILE_H) * 1

typedef struct tVector
{
    int16_t x;
    int16_t y;
} tVector;

typedef struct tScroll
{
    tVector pos;
} tScroll;

typedef struct tEntity
{
    tVector pos;
    fixed fX;
    fixed fY;
    fixed vX;
    fixed vY;
    bool ground;
    bool jump;
} tEntity;

struct player  
{
    tEntity ent;
    BITMAP *img;
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
void update_player();
void update_scroll();
void draw_player();

//update fps callback
static void update_fps(void)
{
    fps = frameCount;
    frameCount = 0;
}
END_OF_FUNCTION(update_fps);

int main()
{    
    int retrace; 

    if (allegro_init() != 0)
        return 1;

    install_timer();
    install_keyboard(); 
    
    fps = 0;
    frameCount = 0;
    LOCK_VARIABLE(fps);
    LOCK_VARIABLE(frameCount);
    install_int_ex(update_fps, BPS_TO_TIMER(1));

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
    
    //init scroll
    scroll.pos.x = 0;
    scroll.pos.y = 0;
    
    //init player
    player.ent.pos.x = 0;
    player.ent.pos.y = 0;
    player.ent.fX = itofix(16);
    player.ent.fY = itofix(10);
    player.ent.vX = 0;
    player.ent.vY = 0;
    player.img = load_bmp("res/004.bmp", NULL);

    //main loop
    while (!gameExit)
    {
        retrace = retrace_count;

        if (key[KEY_ESC])
            gameExit = true;

        update_player();
        update_scroll();

        //clear_to_color(buffer, 3);
        clear_to_color(mapScreen, 1);
    
        draw_map(mapScreen);
        draw_player();

        //draw_sprite(buffer, mapScreen, GAME_X, GAME_Y); //slower
        blit(mapScreen, buffer, 0, 0, GAME_X, GAME_Y, GAME_W, GAME_H);
        
        //debug
        textprintf_ex(buffer, font, 0, 0, 0, 3, "FPS: %d", fps); 
        textprintf_ex(buffer, font, 0, 8, 0, 3, "s.x: %d", scroll.pos.x);
        textprintf_ex(buffer, font, 0, 16, 0, 3, "p.vX: %f", fixtof(player.ent.vX));
        textprintf_ex(buffer, font, 0, 24, 0, 3, "p.vY: %f", fixtof(player.ent.vY));
        textprintf_ex(buffer, font, 0, 32, 0, 3, "p.x: %d", player.ent.pos.x);
        textprintf_ex(buffer, font, 0, 40, 0, 3, "time: %f", deltaTime);
        textprintf_ex(buffer, font, 0, 48, 0, 3, "clock: %d", retrace_count);

        //blit to screen
        //blit(mapScreen, screen, 0, 0, GAME_X, GAME_Y, GAME_W, GAME_H);
        blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);
        
        frameCount++;
        
        vsync();
        deltaTime = (double)(retrace_count - retrace) / CLOCKS_PER_SEC;
        
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

    for (int y = 0; y < (GAME_H / TILE_H); y++)
    {
        for (int x = 0; x < (GAME_W / TILE_W) + 1; x++)        
        {
            tileNum = map[y+(scroll.pos.y / TILE_H)][x+(scroll.pos.x / TILE_W)];

            /* blit tile*/
            if (tileNum != 0)            
                draw_sprite(mapScreen, tiles[tileNum - 1], (x * 16) - (scroll.pos.x % TILE_W) , (y * 16) - (scroll.pos.y % TILE_H));
        }    
    }    
}

void update_player()
{
    fixed accel_x = ftofix(0.3);
    fixed friction = ftofix(0.8);       //more friction, more sloppy (0.94-0.96 is like ice)
    fixed air_friction = ftofix(0.6);   //less than floor friction
    fixed gravity = ftofix(0.2);
    fixed accel_y = ftofix(4.0);        //jump acceleration
    fixed max_vel_x = ftofix(2.5);
    fixed max_vel_y = ftofix(6);

    int16_t floor = 160;
    
    fixed localFriction = player.ent.ground ? friction: air_friction;

    //update controls
    if (key[KEY_RIGHT] && player.ent.vX < max_vel_x)
        player.ent.vX+= fixmul(accel_x, (itofix(1) - localFriction));

    if (key[KEY_LEFT] && player.ent.vX > -max_vel_x)
        player.ent.vX-= fixmul(accel_x, (itofix(1) - localFriction));

    if (key[KEY_Z] && player.ent.ground)
    {
        player.ent.vY = -accel_y;
        player.ent.jump = true;
        player.ent.ground = false;
    }

    //update vels
    if (!key[KEY_RIGHT] && !key[KEY_LEFT])
        player.ent.vX = fixmul(player.ent.vX, localFriction);
    
    if (player.ent.pos.y >= floor && !player.ent.jump)
    { 
        player.ent.vY = 0;
        player.ent.fY = itofix(floor);
        player.ent.ground = true;
    }
    else
    {
        player.ent.vY += player.ent.vY >= max_vel_y ? 0 : gravity;
        player.ent.jump = false;
    }   
    
    //apply velocity
    if (fps > 0)
        player.ent.fX += fixmul(player.ent.vX, ftofix((float)(60/fps)));
    player.ent.fY += player.ent.vY;

    //update position
    player.ent.pos.x = fixtoi(player.ent.fX);
    player.ent.pos.y = fixtoi(player.ent.fY);
}

void draw_player()
{
    draw_sprite(mapScreen, player.img, player.ent.pos.x - scroll.pos.x, player.ent.pos.y - scroll.pos.y);
}

void update_scroll()
{
    //test: follow player 
    scroll.pos.x = player.ent.pos.x - (GAME_W >> 1);
    
    //scroll limits
    if (scroll.pos.x > (((MAP_TILE_W * TILE_W) - GAME_W) - 1))
        scroll.pos.x = (((MAP_TILE_W * TILE_W) - GAME_W) - 1);
              
    if (scroll.pos.x < 0)
        scroll.pos.x = 0;
        
}