/********************************************************************
* Timer System
*
*
* Warcom Soft. 13/05/2025
********************************************************************/
#ifndef _H_TIMER_
#define _H_TIMER_

#include <stdbool.h>
#include "allegro.h"

//inits the timer system specifing game tick duration on ms
void timer_init(long gameTickDuration);
//indicates to timer the start of game frame to calculate deltaTime
void timer_start_frame();
//indicates to end the start of game frame to calculate deltaTime and fps
void timer_end_frame(double *deltaTime);
//gets the current fps
uint16_t get_fps();
//gets the current clock tick
bool get_clock_tick();
#endif