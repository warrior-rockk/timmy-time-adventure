/********************************************************************
* Scroll System
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#include "scroll.h"

tScroll scroll_create(tVector window, tVector limit)
{
    tScroll scroll;

    scroll.pos.x    = 0;
    scroll.pos.y    = 0;
    scroll.window   = window;
    scroll.limit    = limit;

    return scroll;
}

void scroll_init(tScroll *scroll)
{
    MY_ASSERT(scroll);
    
    scroll->pos.x = 0;
    scroll->pos.y = 0;    
}

void scroll_update(tScroll *scroll, tVector *targetPos)
{
    MY_ASSERT(scroll);
    MY_ASSERT(targetPos);

    //updates scroll position following target position
    if (targetPos->x > (scroll->window.x >> 1) + scroll->pos.x + scrollOffset)
        scroll->pos.x = targetPos->x - (scroll->window.x >> 1) - scrollOffset;
    else if (targetPos->x < (scroll->window.x >> 1) + scroll->pos.x - scrollOffset)
        scroll->pos.x = targetPos->x - (scroll->window.x >> 1) + scrollOffset;
    
    //limit scroll position
    scroll->pos.x = (int16_t)clamp(scroll->pos.x, 0, scroll->limit.x);      
}

bool scroll_rect_on_region(tRectangle rect, tScroll *scroll)
{	
    return  ((rect.pos.x + rect.size.x) >= scroll->pos.x) && (rect.pos.x < (scroll->pos.x + scroll->window.x)) &&
            ((rect.pos.y + rect.size.y) >= scroll->pos.y) && (rect.pos.y < (scroll->pos.y + scroll->window.y));
            
}
