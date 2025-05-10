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
    #define MY_TRACE                TRACE
#endif

//vector int16 object
typedef struct tVector
{
    int16_t x;
    int16_t y;
} tVector;

//vector fixed object
typedef struct tFixVector
{
    fixed x;
    fixed y;
} tFixVector;

double deltaTime;

//clamp value between min and max
int clamp(int value, int min, int max);
//function to abort program with critical error
void abort_on_error(const char *format, ...);

#endif