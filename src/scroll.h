/********************************************************************
* Scroll System
*
* 07/05/2025
* Warcom Soft.  - warrior.rockk@gmail.com
********************************************************************/
#ifndef _H_SCROLL_
#define _H_SCROLL_

#include "utils.h"

//scroll definitions
#define SCROLL_OFFSET_X             8   //Offset X target position to follow camera
#define SCROLL_OFFSET_Y             14  //Offset Y target position to follow camera
#define SCROLL_BY_WINDOW_RANGE      32  //Range to scroll by window
#define SCROLL_BY_WINDOW_VEL_Y      10  //Scroll velocity Y moving by window

#define SCROLL_IN_REGION_OFFSET_X   32  //Offset X to scroll region check
#define SCROLL_IN_REGION_OFFSET_Y   32  //Offset Y to scroll region check

//scroll modes
enum E_SCROLL_MODE
{
    E_SCROLL_NONE_MODE,             //no scroll
    E_SCROLL_NORMAL_MODE,           //continuous follow camera
    E_SCROLL_BY_WINDOW_MODE,        //continuous follow camera adjusting to scroll window screen
    E_SCROLL_BY_WINDOW_Y_MODE,      //continuous follow camera adjusting only Y to scroll window screen
};

//scroll movements on by Window mode
enum E_SCROLL_BY_WINDOW_MOVE
{
    E_SCROLL_MOVE_NONE,
    E_SCROLL_MOVE_LEFT,
    E_SCROLL_MOVE_RIGHT,
    E_SCROLL_MOVE_UP,
    E_SCROLL_MOVE_DOWN,
};

//scroll object
typedef struct
{    
    uint8_t mode;       //scroll mode
    uint8_t moving;     //scroll moving flags
    tVector pos;        //scroll actual position
    tVector target;     //scroll target position
    tVector window;     //scroll window size
    tVector limit;      //scroll limit position
    tFixVector fixPos;  //scroll actual position fixed point
    tFixVector fixVel;  //scroll velocity fixed point
} tScroll;

//creates scroll with the window dimensions, limits and mode. Returns a scroll object
tScroll scroll_create(tVector window, tVector limit, uint8_t mode);
//inits scroll object
void scroll_init(tScroll *scroll, tVector *initPos);
//updates scroll object. Receives the camera target follow position
void scroll_update(tScroll *scroll, tVector *cameraTarget);
//check if rectangle is on scroll region
bool scroll_rect_on_region(tRectangle rect, tScroll *scroll);
#endif