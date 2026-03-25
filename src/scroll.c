/********************************************************************
* Scroll System
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#include <math.h>

#include "scroll.h"
#include "game.h"

#define TRACE_FLAG      "[SCROLL]"


static void scroll_update_x(tScroll *scroll, tVector *cameraTarget, bool init);
static void scroll_update_y(tScroll *scroll, tVector *cameraTarget, bool init);

tScroll scroll_create(tVector window, tVector limit, uint8_t mode)
{
    tScroll scroll;

    scroll.pos     = (tVector){0, 0};        
    scroll.target  = (tVector){0, 0};
    scroll.fixPos  = (tFixVector){0, 0};    
    scroll.fixVel  = (tFixVector){0, 0}; 
    scroll.moving  = E_SCROLL_MOVE_NONE;
    
    scroll.window   = window;
    scroll.limit    = limit;
    scroll.mode     = mode;

    MY_TRACE_FLAG("Created scroll with window x:%i y:%i and limit x:%i y:%i\n", scroll.window.x, scroll.window.y, scroll.limit.x, scroll.limit.y);
    return scroll;
}

void scroll_init(tScroll *scroll, tVector *initPos)
{
    MY_ASSERT(scroll);
    
    scroll->moving  = E_SCROLL_MOVE_NONE;

    scroll_update_x(scroll, initPos, true);
    
    scroll_update_y(scroll, initPos, true);

    MY_TRACE_FLAG("Scroll initialized on position x: %i position y:%i\n", scroll->pos.x, scroll->pos.y);
}

void scroll_update(tScroll *scroll, tVector *cameraTarget)
{
    MY_ASSERT(scroll);
    MY_ASSERT(cameraTarget);

    scroll_update_x(scroll, cameraTarget, false);
    
    scroll_update_y(scroll, cameraTarget, false);
    
    //show_debug("scFy: %.2f scPY: %i scTY: %i", fixtof(scroll->fixPos.y), scroll->pos.y, scroll->target.y);
    //show_debug("scVY: %.2f", fixtof(scroll->fixVel.y));
    //show_debug("wX: %i, wY: %i, lX:%i lY:%i", scroll->window.x, scroll->window.y, scroll->limit.x, scroll->limit.y);    
}

bool scroll_rect_on_region(tRectangle rect, tScroll *scroll)
{	
    return  ((rect.pos.x + rect.size.x) >= scroll->pos.x - SCROLL_IN_REGION_OFFSET_X) && (rect.pos.x < ((scroll->pos.x + scroll->window.x) + SCROLL_IN_REGION_OFFSET_X)) &&
            ((rect.pos.y + rect.size.y) >= scroll->pos.y - SCROLL_IN_REGION_OFFSET_Y) && (rect.pos.y < ((scroll->pos.y + scroll->window.y) + SCROLL_IN_REGION_OFFSET_Y));
            
}

static void scroll_update_x(tScroll *scroll, tVector *cameraTarget, bool init)
{
    //updates X scroll position
    switch (scroll->mode)    
    {
        //continuous follow camera
        case E_SCROLL_NORMAL_MODE:
        case E_SCROLL_BY_WINDOW_Y_MODE:
            if (cameraTarget->x > (scroll->window.x >> 1) + scroll->pos.x + SCROLL_OFFSET_X)
                scroll->pos.x = cameraTarget->x - (scroll->window.x >> 1) - SCROLL_OFFSET_X;
            else if ((cameraTarget->x < (scroll->window.x >> 1) + scroll->pos.x - SCROLL_OFFSET_X))
                scroll->pos.x = cameraTarget->x - (scroll->window.x >> 1) + SCROLL_OFFSET_X;
        break;
    }
    
    if (scroll->pos.x > game.stopScrollRight - scroll->window.x && game.stopScrollRight)
        scroll->pos.x = game.stopScrollRight - scroll->window.x;
    
    if (scroll->pos.x < game.stopScrollLeft && game.stopScrollLeft)
        scroll->pos.x = game.stopScrollLeft;

    MY_TRACE_FLAG("update x pos.x %i stopScroLeft %i\n", scroll->pos.x, game.stopScrollLeft);

    //scroll->pos.x = (int16_t)clamp(scroll->pos.x, game.stopScrollLeft, game.stopScrollRight - scroll->window.x);
    show_debug("stopRight %i stopLeft %i", game.stopScrollRight, game.stopScrollLeft);  

    //limit scroll position
    scroll->pos.x = (int16_t)clamp(scroll->pos.x, 0, scroll->limit.x);
}

static void scroll_update_y(tScroll *scroll, tVector *cameraTarget, bool init)
{
    //updates Y scroll position
    switch (scroll->mode)
    {
        //continuous follow camera: sets scroll position y to center cameraTarget +/- offset
        case E_SCROLL_NORMAL_MODE:
            if (cameraTarget->y > (scroll->window.y >> 1) + scroll->pos.y + SCROLL_OFFSET_Y)
                scroll->pos.y = cameraTarget->y - (scroll->window.y >> 1) - SCROLL_OFFSET_Y;
            else if (cameraTarget->y < (scroll->window.y >> 1) + scroll->pos.y - SCROLL_OFFSET_Y)
                scroll->pos.y = cameraTarget->y - (scroll->window.y >> 1) + SCROLL_OFFSET_Y;        
        break;
        //moves the scroll only when change the size of scroll window +/- range
        case E_SCROLL_BY_WINDOW_MODE:
        case E_SCROLL_BY_WINDOW_Y_MODE:
            //calculate Y scroll target
            if (!scroll->moving)
            {
                //check camera target to move scroll down one scroll window position
                if ((cameraTarget->y > (scroll->pos.y + scroll->window.y - SCROLL_BY_WINDOW_RANGE) && scroll->pos.y < scroll->limit.y) && (!game.stopScrollDown || init))
                    scroll->target.y = (int16_t)(floor(cameraTarget->y / scroll->window.y)) * scroll->window.y;
                //check camera target to move scroll up one scroll window position
                if ((cameraTarget->y < (scroll->pos.y - SCROLL_BY_WINDOW_RANGE) && scroll->pos.y > 0) && (!game.stopScrollUp || init))   
                    scroll->target.y = (int16_t)(floor(cameraTarget->y / scroll->window.y)) * scroll->window.y;
            }
            show_debug("Scroll target y:%i", scroll->target.y);
            show_debug("Floor %i", (int16_t)(floor(cameraTarget->y / scroll->window.y)));

            //set scroll velocity
            if (scroll->moving == E_SCROLL_MOVE_NONE && !init)
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
                    (scroll->pos.y <= scroll->target.y && scroll->moving == E_SCROLL_MOVE_UP)   ||
                    init)
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

    show_debug("StopScrollDown %i, StopScrollUp %i", game.stopScrollDown, game.stopScrollUp);
    //limit scroll position
    scroll->pos.y = (int16_t)clamp(scroll->pos.y, 0, scroll->limit.y);    
}