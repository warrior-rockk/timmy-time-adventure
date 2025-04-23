#include <stdio.h>
#include "allegro.h"

int main()
{    
    /* you should always do this at the start of Allegro programs */
    if (allegro_init() != 0)
        return 1;

    /* set up the keyboard handler */
    install_keyboard(); 

    /* set a graphics mode sized 320x200 */
    if (set_gfx_mode(GFX_AUTODETECT, 320, 200, 0, 0) != 0) 
    {
        if (set_gfx_mode(GFX_SAFE, 320, 200, 0, 0) != 0) 
        {
            set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
            allegro_message("Unable to set any graphic mode\n%s\n", allegro_error);
            return 1;
        }
    }

    /* load bitmap */
    BITMAP *bmp = load_bmp("res/logo.bmp", desktop_palette);
    
    /* set the color palette */
    set_palette(desktop_palette);

    /* clear the screen to white */
    clear_to_color(screen, 0);
    
    /* blit bitmap*/
    if (bmp)
        draw_sprite(screen, bmp, (SCREEN_W>>1) - ((bmp->w)>>1), (SCREEN_H>>1) - ((bmp->h)>>1));

    /* you don't need to do this, but on some platforms (eg. Windows) things
    * will be drawn more quickly if you always acquire the screen before
    * trying to draw onto it.
    */
    acquire_screen();

    /* write some text to the screen with black letters and transparent background */
    textout_centre_ex(screen, font, "Hello, world from DOS!", SCREEN_W/2, SCREEN_H/2, makecol(255,255,255), -1);

    /* you must always release bitmaps before calling any input functions */
    release_screen();

    /* wait for a key press */
    readkey();

    return 0;
}
END_OF_MAIN()
