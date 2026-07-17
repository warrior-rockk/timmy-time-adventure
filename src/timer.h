/********************************************************************
* Timer System
*
* 15/05/2025
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#ifndef _H_TIMER_
#define _H_TIMER_

#include <stdbool.h>
#include "allegro.h"

#define DELTA_TIME_LIMIT        6
#define MAX_ACUMULATED_TICKS    10

//inits the timer system specifing game tick duration on ms
void timer_init(long gameTickDuration);
//indicates to timer the start of game frame to calculate deltaTime
void timer_start_frame();
//indicates to end the start of game frame to calculate deltaTime and fps
void timer_end_frame(double *deltaTime);
//gets the current fps
uint16_t fps_get();
//gets the current clock tick. Returns numbers of ticks for configured game tick duration base
uint16_t clock_tick_get();
//get 1 second clock tick
bool clock_tick_1sec_get();
//return 1 each time times the clock counter (periodic counters)
bool clock_counter_check(uint16_t time);
//returns clock counter value 
int16_t clock_counter_get();
#endif