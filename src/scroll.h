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

//scroll object
typedef struct tScroll
{
    tVector pos;        //scroll actual position
    tVector window;     //scroll window size
    tVector limit;      //scroll limit position
} tScroll;

//Offset target position to follow by scroll. 
//If the target is in the offset zone, the scroll doesn't move allowing a tiny zone with not scroll movement
static const uint16_t scrollOffsetX = 8;
static const uint16_t scrollOffsetY = 14;

//creates scroll with the window dimensions and limits. Returns a scroll object
tScroll scroll_create(tVector window, tVector limit);
//inits scroll object
void scroll_init(tScroll *scroll);
//updates scroll object. Receives the target follow position or NULL
void scroll_update(tScroll *scroll, tVector *targetPos);

//check if rectangle is on scroll region
bool scroll_rect_on_region(tRectangle rect, tScroll *scroll);
#endif