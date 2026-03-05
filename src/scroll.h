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
#define SCROLL_IN_REGION_OFFSET_X   32
#define SCROLL_IN_REGION_OFFSET_Y   32
#define SCROLL_BY_WINDOW_RANGE      32
#define SCROLL_BY_WINDOW_VEL_Y      10

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
    tVector pos;        //scroll actual position
    tVector target;
    tVector window;     //scroll window size
    tVector limit;      //scroll limit position
    tFixVector fixPos;
    tFixVector fixVel;
    uint8_t moving;
    uint8_t mode;
} tScroll;

//Offset target position to follow by scroll. 
//If the target is in the offset zone, the scroll doesn't move allowing a tiny zone with not scroll movement
static const uint16_t scrollOffsetX = 8;
static const uint16_t scrollOffsetY = 14;

//creates scroll with the window dimensions and limits. Returns a scroll object
tScroll scroll_create(tVector window, tVector limit, uint8_t mode);
//inits scroll object
void scroll_init(tScroll *scroll);
//updates scroll object. Receives the target follow position or NULL
void scroll_update(tScroll *scroll, tVector *cameraTarget);

//check if rectangle is on scroll region
bool scroll_rect_on_region(tRectangle rect, tScroll *scroll);
#endif