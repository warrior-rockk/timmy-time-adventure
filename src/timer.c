/********************************************************************
* Timer system
*
*
* Warcom Soft. 13/05/2025
********************************************************************/
#include "timer.h"

uint16_t fps;           //fps counter
uint16_t frameCount;    //count of frames
uint16_t tick;          //tick counter
uint16_t trace;         //trace video counter

//update fps callback
static void update_fps(void)
{
    //calculate how many frames per sec
    fps = frameCount;
    frameCount = 0;
}
END_OF_FUNCTION(update_fps);

//timer function callback
static void update_tick(void)
{
    //increment tick
    tick++;
}
END_OF_FUNCTION(update_tick);

void timer_init()
{
    fps = 0;
    frameCount = 0;
    trace = 0;
    LOCK_VARIABLE(fps);
    LOCK_VARIABLE(frameCount);
    LOCK_VARIABLE(tick);
    LOCK_FUNCTION(update_fps);
    LOCK_FUNCTION(update_tick);
    install_int_ex(update_fps, BPS_TO_TIMER(1));
    install_int(update_tick, 10);   //10ms game tick
}

void timer_start_frame()
{
    trace = retrace_count;
}

void timer_end_frame(double *deltaTime)
{
    frameCount++;

    if (trace != retrace_count)
        *deltaTime = (double)(retrace_count-trace);
}

uint16_t get_fps()
{
    return fps;
}