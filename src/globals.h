/********************************************************************
* Game Globals
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#ifndef _H_GLOBALS_
#define _H_GLOBALS_

#include "allegro.h"

#ifdef ALLEGRO_H
    #define MY_ASSERT(condition)    ASSERT(condition)
#endif

typedef struct tVector
{
    int16_t x;
    int16_t y;
} tVector;

double deltaTime;

//clamp value between min and max
int clamp(int value, int min, int max);

#endif