/********************************************************************
* Main game file
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include <stdio.h>
#include "allegro.h"

#include "utils.h"
#include "game.h"

static void main_init();

int main()
{    
    main_init();
    game_init();

    //main loop
    do
    {
        game_draw();
        game_update();
        
    } while (!gameExit);
    
    return EXIT_SUCCESS;
}
END_OF_MAIN()

//main initialization
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
    if (set_gfx_mode(GAME_GFX_MODE, SCREEN_X, SCREEN_Y, 0, 0) != 0)
        abort_on_error("Error seteando modo grafico");
    set_color_depth(GAME_COLOR_DEPTH);
}