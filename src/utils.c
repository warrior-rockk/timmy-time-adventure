/********************************************************************
* Utilities code
*
* 28/01/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include <stdio.h>
#include "utils.h"

//general frame counter
uint16_t frameCounter = 0;

//debug structure
#ifdef DEBUGMODE
struct debug
{
    int16_t numMsgs;
    char lineMsg[DEBUG_MAX_MSGS][DEBUG_MSG_MAX_CHARS];
} debug;
#endif

//playtime counter struct
struct
{
    uint8_t seconds;
    uint8_t minutes;
    uint16_t hours;
} playTime;

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

fixed fix_clamp(fixed value, fixed min, fixed max)
{
    if (value < min)
        return min;
    else if (value > max)
        return max;
    else
        return value;
}

//retuns true if the value is on target +/- the offset
bool in_range(int value, int target, int offset)
{
    return ( value <= (target + offset) && value >= (target - offset) );
}

bool in_range_vector(tVector value, tVector target, tVector offset)
{
    return (value.x <= (target.x + offset.x) && value.x >= (target.x - offset.x)) &&
           (value.y <= (target.y + offset.y) && value.y >= (target.y - offset.y));
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
        MY_TRACE("[ERROR]: %s\n", buf);
        MY_TRACE("[ERROR]: %s\n", allegro_error);
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

//function to init playTime
void playTime_init()
{
    playTime.seconds        = 0;
    playTime.minutes        = 0;
    playTime.hours          = 0;
}

//function to update playTime
void playTime_update(bool clock1sec)
{
    if (clock1sec)
    {
        if (playTime.seconds >= 60)
        {
            if (playTime.minutes >= 59)
            {
                playTime.hours++;
                playTime.minutes = 0;
            }
            else
                playTime.minutes++;

            playTime.seconds = 0;
        }
        else
            playTime.seconds++;
    }
}

uint16_t playTime_get_hours()
{
    return playTime.hours;
}

uint8_t playTime_get_minutes()
{
    return playTime.minutes;
}

uint16_t get_frame_counter()
{
    return frameCounter;
}

void inc_frame_counter()
{
    frameCounter++;
}