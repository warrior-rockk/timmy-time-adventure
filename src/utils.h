/********************************************************************
* Utils definitions
*
* 28/01/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#ifndef _H_UTILS_
#define _H_UTILS_

#include <stdbool.h>
#include "allegro.h"

//macros for bit masking
#define SET_BIT(data, bit)       ((data) |=  (1<<(bit)))
#define CLEAR_BIT(data, bit)     ((data) &= ~(1<<(bit)))
#define TOOGLE_BIT(data, bit)    ((data) ^=  (1<<(bit)))
#define CHECK_BIT(data, bit)     ((data) &   (1<<(bit)))

//macros for bit flag masking
#define SET_FLAG(data, bit)       ((data) |=  ((bit)))
#define CLEAR_FLAG(data, bit)     ((data) &= ~((bit)))
#define TOOGLE_FLAG(data, bit)    ((data) ^=  ((bit)))
#define CHECK_FLAG(data, bit)     (((data) & ((bit))) == bit)

#ifdef ALLEGRO_H
    #define MY_ASSERT(condition)                ASSERT(condition)
    #define MY_TRACE                            TRACE
    #define MY_TRACE_FLAG(data, ...)            { TRACE("(%i)", get_frame_counter()); TRACE(TRACE_FLAG": " data, ##__VA_ARGS__); }
    #define MY_TRACE_MARK                       MY_TRACE("[MARK]\n")
#else
    #define MY_ASSERT(condition)                
    #define MY_TRACE                            
    #define MY_TRACE_FLAG(data, ...)            
    #define MY_TRACE_MARK                       
#endif

#define HALT    exit(-1);

//Debug
#define DEBUG_MAX_MSGS          16
#define DEBUG_MSG_MAX_CHARS     50
#define DEBUG_Y                 0
#define DEBUG_FONT_HEIGHT       8
#define DEBUG_FONT_COLOR        0
#define DEBUG_FONT_BACK_COLOR   3

//vector int16 type
typedef struct tVector
{
    int16_t x;
    int16_t y;
} tVector;

//line path vector type
typedef struct tLinePath
{
    tVector start;
    tVector end;
} tLinePath;

//vector fixed type
typedef struct tFixVector
{
    fixed x;
    fixed y;
} tFixVector;

//fix line path vector type
typedef struct
{
    tFixVector start;
    tFixVector end;
} tFixLinePath;

//rectangle type
typedef struct tRectangle
{
    tVector pos;
    tVector size;
} tRectangle;

//4 directions type
typedef struct
{
    int16_t left;
    int16_t right;
    int16_t down;
    int16_t up;
} t4dir;

//clamp value between min and max
int clamp(int value, int min, int max);
//fixed clamp value between min and max
int fix_clamp(fixed value, fixed min, fixed max);
//retuns true if the value is on target +/- the offset
bool in_range(int value, int target, int offset);
//retuns true if the tVector value is on tVector target +/- the offset
bool in_range_vector(tVector value, tVector target, tVector offset);
//function to abort program with critical error
void abort_on_error(const char *format, ...);
//funcion to init the debug vars
void debug_init();
//draws debug info
void debug_draw(BITMAP *buffer);
//function to show debug info on screen
void show_debug(const char *format, ...);
//function to init playTime
void playTime_init();
//function to update playtine
void playTime_update(bool clock1sec);
//function to get playtime hours
uint16_t playTime_get_hours();
//function to get playtime minutes
uint8_t playTime_get_minutes();
//gets frame counter
uint16_t get_frame_counter();
//increments frame counter (put on game loop cycle)
void inc_frame_counter();
//converts tVector to tFixVector
tFixVector vector2fixvector(tVector vector);
#endif