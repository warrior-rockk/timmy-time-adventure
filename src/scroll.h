/********************************************************************
* Scroll system
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#ifndef _H_SCROLL_
#define _H_SCROLL_

#include "globals.h"

typedef struct tScroll
{
    tVector pos;
    tVector window;
    tVector limit;
} tScroll;

//public functions
tScroll create_scroll(tVector window, tVector limit);
void scroll_init(tScroll *scroll);
void scroll_update(tScroll *scroll, tVector *targetPos);

#endif