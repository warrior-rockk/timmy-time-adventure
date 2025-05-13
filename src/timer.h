/********************************************************************
* Timer System
*
*
* Warcom Soft. 13/05/2025
********************************************************************/
#ifndef _H_TIMER_
#define _H_TIMER_

#include "allegro.h"

uint16_t fps;
uint16_t frameCount;
uint16_t tick;
uint16_t trace;

void timer_init();
void timer_start_frame();
void timer_end_frame(double *deltaTime);
uint16_t get_fps();
#endif