/********************************************************************
* Scroll System
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#include <math.h>

#include "scroll.h"

tScroll scroll_create(tVector window, tVector limit, uint8_t mode)
{
    tScroll scroll;

    scroll.pos.x    = 0;
    scroll.pos.y    = 0;
    scroll.window   = window;
    scroll.limit    = limit;
    scroll.mode     = mode;

    return scroll;
}

void scroll_init(tScroll *scroll)
{
    MY_ASSERT(scroll);
    
    scroll->pos     = (tVector){0, 0};        
    scroll->target  = (tVector){0, 0};
    scroll->fixPos  = (tFixVector){0, 0};    
    scroll->fixVel  = (tFixVector){0, 0}; 
    scroll->moving  = E_SCROLL_MOVE_NONE;
}

void scroll_update(tScroll *scroll, tVector *cameraTarget)
{
    MY_ASSERT(scroll);
    MY_ASSERT(cameraTarget);

    //updates X scroll position
    switch (scroll->mode)    
    {
        //continuous follow camera
        case E_SCROLL_NORMAL_MODE:
        case E_SCROLL_BY_WINDOW_Y_MODE:
            if (cameraTarget->x > (scroll->window.x >> 1) + scroll->pos.x + scrollOffsetX)
                scroll->pos.x = cameraTarget->x - (scroll->window.x >> 1) - scrollOffsetX;
            else if (cameraTarget->x < (scroll->window.x >> 1) + scroll->pos.x - scrollOffsetX)
                scroll->pos.x = cameraTarget->x - (scroll->window.x >> 1) + scrollOffsetX;
        break;
    }
    //limit scroll position
    scroll->pos.x = (int16_t)clamp(scroll->pos.x, 0, scroll->limit.x);      

    //updates Y scroll position
    switch (scroll->mode)
    {
        //continuous follow camera
        case E_SCROLL_NORMAL_MODE:
            if (cameraTarget->y > (scroll->window.y >> 1) + scroll->pos.y + scrollOffsetY)
                scroll->pos.y = cameraTarget->y - (scroll->window.y >> 1) - scrollOffsetY;
            else if (cameraTarget->y < (scroll->window.y >> 1) + scroll->pos.y - scrollOffsetY)
                scroll->pos.y = cameraTarget->y - (scroll->window.y >> 1) + scrollOffsetY;        
        break;
        //continuous follow camera adjusting to room screen
        case E_SCROLL_BY_WINDOW_MODE:
        case E_SCROLL_BY_WINDOW_Y_MODE:
            //calculate Y scroll target
            scroll->target.y = (int16_t)(cameraTarget->y / (scroll->window.y - SCROLL_BY_WINDOW_RANGE)) * scroll->window.y;

            //set scroll velocity
            if (scroll->moving == E_SCROLL_MOVE_NONE)
            {
                if (scroll->pos.y < scroll->target.y)
                {
                    scroll->fixVel.y = itofix(SCROLL_BY_WINDOW_VEL_Y);
                    scroll->moving = E_SCROLL_MOVE_DOWN;
                }
                else if (scroll->pos.y > scroll->target.y)
                {
                    scroll->fixVel.y = itofix(-SCROLL_BY_WINDOW_VEL_Y);
                    scroll->moving = E_SCROLL_MOVE_UP;
                }            
            }
            else     
            {
                if ((scroll->pos.y >= scroll->target.y && scroll->moving == E_SCROLL_MOVE_DOWN) ||
                    (scroll->pos.y <= scroll->target.y && scroll->moving == E_SCROLL_MOVE_UP))
                {
                    scroll->fixVel.y = 0;  
                    scroll->fixPos.y = itofix(scroll->target.y);
                    scroll->moving = E_SCROLL_MOVE_NONE;
                }
            }

            //add velocity
            scroll->fixPos.y = scroll->fixPos.y + scroll->fixVel.y;
            //update position        
            scroll->pos.y = fixtoi(scroll->fixPos.y);
        break;
    }
    //limit scroll position
    scroll->pos.y = (int16_t)clamp(scroll->pos.y, 0, scroll->limit.y);

    //show_debug("scFy: %.2f scPY: %i scTY: %i", fixtof(scroll->fixPos.y), scroll->pos.y, scroll->target.y);
    //show_debug("scVY: %.2f", fixtof(scroll->fixVel.y));
    //show_debug("wX: %i, wY: %i, lX:%i lY:%i", scroll->window.x, scroll->window.y, scroll->limit.x, scroll->limit.y);    
}

bool scroll_rect_on_region(tRectangle rect, tScroll *scroll)
{	
    return  ((rect.pos.x + rect.size.x) >= scroll->pos.x - SCROLL_IN_REGION_OFFSET_X) && (rect.pos.x < ((scroll->pos.x + scroll->window.x) + SCROLL_IN_REGION_OFFSET_X)) &&
            ((rect.pos.y + rect.size.y) >= scroll->pos.y - SCROLL_IN_REGION_OFFSET_Y) && (rect.pos.y < ((scroll->pos.y + scroll->window.y) + SCROLL_IN_REGION_OFFSET_Y));
            
}
