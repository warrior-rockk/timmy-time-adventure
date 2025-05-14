/********************************************************************
* Timer system
*
*
* Warcom Soft. 13/05/2025
********************************************************************/
#include "timer.h"

uint16_t fps;           //fps counter
uint16_t frameCount;    //count of frames
uint16_t tickCount;     //tick counter
bool tick;               //clock tick
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
    tickCount++;
}
END_OF_FUNCTION(update_tick);

void timer_init(long gameTickDuration)
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
    install_int(update_tick, gameTickDuration);
}

void timer_start_frame()
{
    trace = retrace_count;

    //reset clock tick
    tick = false;

    if (tickCount)
    {
        //sets clock tick var
        tick = true;
        //reset timer interrupt var
        tickCount = 0;
    }
}

void timer_end_frame(double *deltaTime)
{
    frameCount++;

    if (trace != retrace_count)
        *deltaTime = (double)(retrace_count-trace);

    tick = false;
}

uint16_t get_fps()
{
    return fps;
}

bool get_clock_tick()
{
    return tick;
}