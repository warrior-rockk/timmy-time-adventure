/********************************************************************
* Timer system
*
* 13/05/2025
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include "timer.h"

uint16_t fps;               //fps counter
uint16_t frameCount;        //count of frames for fps counter
bool tick;                  //clock tick (set to one 1 frame on update tick time)
uint16_t tickCount;         //counter for tick
uint16_t lastTickCount;     //stores how many clocks ticks has been passed since last frame
bool tick1sec;              //clock 1sec tick (set to one 1 frame on every second)
uint8_t tick1SecCount;      //for tick1sec
uint16_t trace;             //trace video counter for calculate delta time
uint16_t tickCounter;        //general clock tick counter

//update fps callback
static void update_fps(void)
{
    //calculate how many frames per sec
    fps = frameCount;
    frameCount = 0;
    tick1SecCount++;
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
    lastTickCount = 0;
    tickCounter = 0;
    tickCount = 0;
    tick = false;
    tick1sec = false;
    tick1SecCount = 0;
    LOCK_VARIABLE(fps);
    LOCK_VARIABLE(frameCount);    
    LOCK_VARIABLE(tick1SecCount);
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
    tick1sec = false;
    
    if (tickCount)
    {
        //sets clock tick var
        tick = true;
        //saves last tick count
        lastTickCount = tickCount;
        //reset timer interrupt var
        tickCount = 0;
        tickCounter++;
    }

    if (tick1SecCount)
    {
        tick1sec = true;
        tick1SecCount = 0;
    }
}

void timer_end_frame(double *deltaTime)
{
    frameCount++;

    if (trace != retrace_count)
        *deltaTime = (double)(retrace_count-trace);
    
    //limit delta time
    if (*deltaTime > DELTA_TIME_LIMIT)
        *deltaTime = 1;

    //TODO: deltaTime disable
    *deltaTime = 1;
    
    tick = false;
    tick1sec = false;
}

uint16_t fps_get()
{
    return fps;
}

uint16_t clock_tick_get()
{
    if (tick)
        return lastTickCount;
    else   
        return 0;
}

bool clock_counter_check(uint16_t time)
{
    return ((tickCounter % time) == 0 && tick);
}

int16_t clock_counter_get()
{
    return tickCounter;
}

bool clock_tick_1sec_get()
{
    return tick1sec;
}
