/********************************************************************
* Game Globals
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#include "globals.h"

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