#include <stdio.h>
#include "allegro.h"

//SMS resolution: 256x192 (testing 256x208: extra Y tile to center screen)
#define SCREEN_X        320
#define SCREEN_Y        240
#define GAME_X          256
#define GAME_Y          208
#define TILE_W          16
#define TILE_H          16
#define NUM_TILES       3

BITMAP *tiles[NUM_TILES];
BITMAP *mapScreen;
RGB* gamePal;

//function declarations
void draw_map(BITMAP *mapScreen);

int main()
{    
    /* you should always do this at the start of Allegro programs */
    if (allegro_init() != 0)
        return 1;

    /* set up the keyboard handler */
    install_keyboard(); 
    
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

    //initialize map bitmap
    mapScreen = create_bitmap(GAME_X, GAME_Y);

    /* set the color palette */
    set_palette(desktop_palette);

    /* clear the screen to white */
    clear_to_color(screen, 3);
    
    draw_map(mapScreen);
    draw_sprite(screen, mapScreen, (SCREEN_W>>1) - ((mapScreen->w)>>1), (SCREEN_H>>1) - ((mapScreen->h)>>1));
    
    /* you don't need to do this, but on some platforms (eg. Windows) things
    * will be drawn more quickly if you always acquire the screen before
    * trying to draw onto it.
    */
    acquire_screen();

    /* you must always release bitmaps before calling any input functions */
    release_screen();

    /* wait for a key press */
    readkey();

    return 0;
}
END_OF_MAIN()

void draw_map(BITMAP *mapScreen)
{
    for (int j = 0; j < (GAME_Y / TILE_H); j++)
    {
        for (int i = 0; i < (GAME_X / TILE_W); i++)        
        {
            /* blit tile*/            
            draw_sprite(mapScreen, tiles[rand() % NUM_TILES], (i * 16) , (j * 16) );
        }    
    }    
}

