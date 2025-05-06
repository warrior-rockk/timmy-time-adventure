#ifndef _H_SCROLL
#define _H_SCROLL

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