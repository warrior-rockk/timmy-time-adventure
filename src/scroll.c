/********************************************************************
* Scroll System
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
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

void scroll_update(tScroll *scroll, tVector *targetPos, uint8_t mode)
{
    MY_ASSERT(scroll);
    MY_ASSERT(targetPos);

    //updates scroll position following target position
    if (mode == E_SCROLL_X || mode == E_SCROLL_X_Y)
    {
        if (targetPos->x > (scroll->window.x >> 1) + scroll->pos.x + scrollOffsetX)
            scroll->pos.x = targetPos->x - (scroll->window.x >> 1) - scrollOffsetX;
        else if (targetPos->x < (scroll->window.x >> 1) + scroll->pos.x - scrollOffsetX)
            scroll->pos.x = targetPos->x - (scroll->window.x >> 1) + scrollOffsetX;
    }

    //limit scroll position
    scroll->pos.x = (int16_t)clamp(scroll->pos.x, 0, scroll->limit.x);      

    //updates scroll position following target position
    if (mode == E_SCROLL_Y || mode == E_SCROLL_X_Y)
    {
        if (targetPos->y > (scroll->window.y >> 1) + scroll->pos.y + scrollOffsetY)
            scroll->pos.y = targetPos->y - (scroll->window.y >> 1) - scrollOffsetY;
        else if (targetPos->y < (scroll->window.y >> 1) + scroll->pos.y - scrollOffsetY)
            scroll->pos.y = targetPos->y - (scroll->window.y >> 1) + scrollOffsetY;
    }
    
    //limit scroll position
    scroll->pos.y = (int16_t)clamp(scroll->pos.y, 0, scroll->limit.y);
}

bool scroll_rect_on_region(tRectangle rect, tScroll *scroll)
{	
    return  ((rect.pos.x + rect.size.x) >= scroll->pos.x - SCROLL_IN_REGION_OFFSET_X) && (rect.pos.x < ((scroll->pos.x + scroll->window.x) + SCROLL_IN_REGION_OFFSET_X)) &&
            ((rect.pos.y + rect.size.y) >= scroll->pos.y - SCROLL_IN_REGION_OFFSET_Y) && (rect.pos.y < ((scroll->pos.y + scroll->window.y) + SCROLL_IN_REGION_OFFSET_Y));
            
}
