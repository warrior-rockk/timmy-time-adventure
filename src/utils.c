/********************************************************************
* Utilities code
*
* 28/01/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include <stdio.h>
#include "utils.h"

//debug structure
#ifdef DEBUGMODE
struct debug
{
    int16_t numMsgs;
    char lineMsg[DEBUG_MAX_MSGS][DEBUG_MSG_MAX_CHARS];
} debug;
#endif

//clamp value between min and max
int clamp(int value, int min, int max)
{
    if (value < min)
        return min;
    else if (value > max)
        return max;
    else
        return value;
}

//function to abort program with critical error
void abort_on_error(const char *format, ...)
{
    char buf[1024];

    //get the arguments of formatted string
    va_list arglist;
    va_start(arglist, format);
    vsprintf(buf, format, arglist);
    va_end(arglist);

    #ifndef DEBUGMODE
    allegro_message(buf);
    #else
    allegro_message(buf);
    allegro_message(allegro_error);
    #endif
    exit(EXIT_FAILURE);
}

//funcion to init the debug vars
void debug_init()
{
#ifdef DEBUGMODE
    debug.numMsgs = 0;
#endif
}

//draws debug info
void debug_draw(BITMAP *buffer)
{
#ifdef DEBUGMODE
    
    //writes all the debug vars
    for (int i = 0; i < debug.numMsgs; i++)
    {
        textprintf_ex(buffer, font, 0, DEBUG_Y + (DEBUG_FONT_HEIGHT*i), DEBUG_FONT_COLOR, DEBUG_FONT_BACK_COLOR, "%s", debug.lineMsg[i]);
    }
    
    //reset debug msgs
    debug.numMsgs = 0;
    
#endif //DEBUGMODE
}

void show_debug(const char *format, ...)
{
#ifdef DEBUGMODE

    char buf[DEBUG_MSG_MAX_CHARS];

    //get the arguments of formatted string
    va_list arglist;
    va_start(arglist, format);
    vsprintf(buf, format, arglist);
    va_end(arglist);

    if (debug.numMsgs < DEBUG_MAX_MSGS)
    {
        strcpy(debug.lineMsg[debug.numMsgs], buf);    
        debug.numMsgs++;            
    }    
#endif
}