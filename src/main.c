#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "allegro.h"

//SMS resolution: 256x192 (testing 256x208: extra Y tile to center screen)
#define SCREEN_X        320
#define SCREEN_Y        240
#define GAME_X          256
#define GAME_Y          208
#define TILE_W          16
#define TILE_H          16
#define NUM_TILES       3
#define MAP_TILE_W      (GAME_X / TILE_W) * 2
#define MAP_TILE_H      (GAME_Y / TILE_H) * 1

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

typedef struct tVector
{
    int16_t x;
    int16_t y;
} tVector;

typedef struct tScroll
{
    tVector pos;
} tScroll;

bool gameExit = false;
int fps;
int frameCount;
tScroll scroll;

//function declarations
void create_map();
void draw_map(BITMAP *mapScreen);

//update fps callback
static void update_fps(void)
{
    fps = frameCount;
    frameCount = 0;
}
END_OF_FUNCTION(update_fps);

int main()
{    
    /* you should always do this at the start of Allegro programs */
    if (allegro_init() != 0)
        return 1;

    /* set up the keyboard handler */
    install_timer();
    install_keyboard(); 
    
    fps = 0;
    frameCount = 0;
    LOCK_VARIABLE(fps);
    LOCK_VARIABLE(frameCount);
    install_int_ex(update_fps, BPS_TO_TIMER(1));

    set_color_depth(8);

    /* set a graphics mode sized 320x200 */
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

    //initialize buffer screen
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    clear_to_color(buffer, 3);

    //initialize map bitmap
    mapScreen = create_bitmap(GAME_X, GAME_Y);
    //create_map();

    //init scroll
    scroll.pos.x = 0;
    scroll.pos.y = 0;
    
    /* set the color palette */
    set_palette(desktop_palette);

    while (!gameExit)
    {
        if (key[KEY_ESC])
            gameExit = true;
        
        if (key[KEY_RIGHT] && scroll.pos.x < ((MAP_TILE_W * TILE_W) - GAME_X) - 1)
            scroll.pos.x++;
        
        if (key[KEY_LEFT] && scroll.pos.x > 0)
            scroll.pos.x--;
        
        /* you don't need to do this, but on some platforms (eg. Windows) things
        * will be drawn more quickly if you always acquire the screen before
        * trying to draw onto it.
        */
        //acquire_screen();

        //clear_to_color(buffer, 3);
        clear_to_color(mapScreen, 1);
    
        draw_map(mapScreen);
        draw_sprite(buffer, mapScreen, (SCREEN_W>>1) - ((mapScreen->w)>>1), (SCREEN_H>>1) - ((mapScreen->h)>>1));
    
        textprintf_ex(buffer, font, 0, 0, 0, 3, "FPS: %d", fps);
        textprintf_ex(buffer, font, 0, 8, 0, 3, "s.x: %d", scroll.pos.x);

        //blit to screen
        blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);
        
        frameCount++;
        //vsync();

        /* you must always release bitmaps before calling any input functions */
        //release_screen();
    }

    return 0;
}
END_OF_MAIN()

void create_map()
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

    for (int y = 0; y < (GAME_Y / TILE_H); y++)
    {
        for (int x = 0; x < (GAME_X / TILE_W) + 1; x++)        
        {
            tileNum = map[y+(scroll.pos.y / TILE_H)][x+(scroll.pos.x / TILE_W)];

            /* blit tile*/
            if (tileNum != 0)            
                draw_sprite(mapScreen, tiles[tileNum - 1], (x * 16) - (scroll.pos.x % TILE_W) , (y * 16) - (scroll.pos.y % TILE_H));
        }    
    }    
}

