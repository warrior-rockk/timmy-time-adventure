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
#include "input.h"
#include "sound.h"
#include "timer.h"

#define TRACE_FLAG "[MAIN]"

static void main_init();
static void main_exit();
#ifdef DEBUGMODE
static void main_stepByStep();
#endif

int main()
{    
    main_init();
    game_init();

    //main loop
    do
    {
        timer_start_frame();
        playTime_update(clock_tick_1sec_get());

        #ifdef DEBUGMODE
            if (debugOptions.stepByStep)
            {
                main_stepByStep();
            }
            else
            {
                game_update();
                game_draw();                   
            }
        #else
            game_update();
            game_draw();
        #endif
    } while (!gameExit);
    
    main_exit();
    return EXIT_SUCCESS;
}
END_OF_MAIN()

//main initialization
static void main_init()
{
    //set env var MY_TRACE log file
    setenv("ALLEGRO_TRACE", "GAME.LOG", 1);

    MY_TRACE_FLAG("%s %i.%i\n", GAME_TITLE, MAJOR_VERSION, MINOR_VERSION);
    MY_TRACE_FLAG("Initializing systems and modules\n");
    
    //set unicode format
    //this is no longer necessary with vscode UTF-8 codification
    //set_uformat(U_ASCII);

    //TODO: option for pc speaker?
    sound_set_mode(E_SOUND_SB_MODE);
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
            MY_TRACE("Sound option selected: Sound Blaster\n");
            sound_set_mode(SB_SND_MODE);
            break;
        case 0x32:
            MY_TRACE("Sound option selected: Speaker\n");
            sound_set_mode(PC_SPEAKER_SND_MODE);    
            break;
        case 0x33:
            MY_TRACE("Sound option selected: None\n");
            sound_set_mode(NO_SOUND_SND_MODE);
            break;
        default:
            exit(-1);
    }
    */

    printf("Starting %s v%i.%i\n", GAME_TITLE, MAJOR_VERSION, MINOR_VERSION);
    
    //initialize and install modules    
    if (allegro_init() != 0)
        abort_on_error("Error initializating Allegro");
    #if ALLEGRO_USES_TIMER
        MY_TRACE_FLAG("Initializing timer\n");
        if (install_timer() != 0)
            abort_on_error("Error initializating timer module");
    #endif
    #if ALLEGRO_USES_KEYBOARD
        MY_TRACE_FLAG("Initializing keyboard\n");
        if (install_keyboard() != 0)
            abort_on_error("Error initializating keyboard");
    #endif
    #if ALLEGRO_USES_MOUSE
        MY_TRACE_FLAG("Initializing mouse\n");
        if (install_mouse() < 0)
            abort_on_error("Error initializating mouse");
    #endif
    #if ALLEGRO_USES_SOUND
        MY_TRACE_FLAG("Initializing sound\n");        
        if (sound_init() != 0)
            abort_on_error("Error initializating sound");
    #else
        sound_set_mode(E_SOUND_OFF_MODE);
        sound_init();
    #endif

    MY_TRACE_FLAG("All system and modules initialized\n");
    
    //set video mode
    if (set_gfx_mode(GAME_GFX_MODE, SCREEN_X, SCREEN_Y, 0, 0) != 0)
        abort_on_error("Error setting graphic mode");
    set_color_depth(GAME_COLOR_DEPTH);

    playTime_init();
}

#ifdef DEBUGMODE
static void main_stepByStep()
{
    input_keys_update();
 
    if (input_key_down(E_G_KEY_S) && (key_shifts & KB_CTRL_FLAG))
        debugOptions.stepByStep = !debugOptions.stepByStep;
 
    if (input_key_down(E_G_KEY_EXIT))
        gameExit = true;
 
    if (input_key_down(E_G_KEY_D) && (key_shifts & KB_CTRL_FLAG))
    {
        game_update();
        game_draw();         
    }           
}
#endif

//function that handles game exit
void main_exit()
{
    MY_TRACE_FLAG("Exiting game\n");
    
    MY_TRACE_FLAG("Quit allegro modules\n");
    MY_TRACE_FLAG("Game played for: %02dh %02dm\n", playTime_get_hours(), playTime_get_minutes());

    //quit allegro modules
    allegro_exit();

    printf("Thanks for playing %s!\n", GAME_TITLE);
    printf("Game played for: %02dhours and %02dminutes\n", playTime_get_hours(), playTime_get_minutes());
}