/********************************************************************
* Game Globals
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#ifndef _H_GLOBALS_
#define _H_GLOBALS_

#include <stdbool.h>
#include "allegro.h"

//macros for bit masking
#define SET_BIT(byte, bit)       ((byte) |=  (1<<(bit)))
#define CLEAR_BIT(byte, bit)     ((byte) &= ~(1<<(bit)))
#define TOOGLE_BIT(byte, bit)    ((byte) ^=  (1<<(bit)))
#define CHECK_BIT(byte, bit)     ((byte) &   (1<<(bit)))

#ifdef ALLEGRO_H
    #define MY_ASSERT(condition)    ASSERT(condition)
    #define MY_TRACE                TRACE
#endif

//Debug
#define DEBUG_MAX_MSGS          10
#define DEBUG_MSG_MAX_CHARS     50
#define DEBUG_Y                 0
#define DEBUG_FONT_HEIGHT       8
#define DEBUG_FONT_COLOR        0
#define DEBUG_FONT_BACK_COLOR   3

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

//rectangle object
typedef struct tRectangle
{
    tVector pos;
    tVector size;
} tRectangle;

//debug structure
#ifdef DEBUGMODE
struct debug
{
    int16_t numMsgs;
    char lineMsg[DEBUG_MAX_MSGS][DEBUG_MSG_MAX_CHARS];
    bool showDebugInfo;
} debug;
#endif

double deltaTime;

//clamp value between min and max
int clamp(int value, int min, int max);
//function to abort program with critical error
void abort_on_error(const char *format, ...);
//funcion to init the debug vars
void debug_init();
//draws debug info
void debug_draw(BITMAP *buffer);
//function to show debug info on screen
void show_debug(const char *format, ...);
#endif