/********************************************************************
* Scroll System
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#include <math.h>

#include "scroll.h"

#define TRACE_FLAG      "[SCROLL]"

tScroll scroll;     //game scroll object

static void scroll_update_x(tVector *cameraTarget, bool init);
static void scroll_update_y(tVector *cameraTarget, bool init);

void scroll_create(tVector window, tVector limit, uint8_t mode)
{
    scroll.pos              = (tVector){0, 0};        
    scroll.target           = (tVector){0, 0};
    scroll.fixPos           = (tFixVector){0, 0};    
    scroll.fixVel           = (tFixVector){0, 0}; 
    scroll.moving           = E_SCROLL_MOVE_NONE;
    scroll.stopScroll.left  = 0;
    scroll.stopScroll.right = 0;
    scroll.stopScroll.down  = 0;
    scroll.stopScroll.up    = 0;
    
    scroll.window           = window;
    scroll.limit            = limit;
    scroll.mode             = mode;

    MY_TRACE_FLAG("Created scroll with window x:%i y:%i and limit x:%i y:%i\n", scroll.window.x, scroll.window.y, scroll.limit.x, scroll.limit.y);
}

void scroll_init(tVector *initPos)
{
    
    scroll.moving  = E_SCROLL_MOVE_NONE;

    scroll_update_x(initPos, true);
    
    scroll_update_y(initPos, true);

    MY_TRACE_FLAG("Scroll initialized on position x: %i position y:%i\n", scroll.pos.x, scroll.pos.y);
}

void scroll_update(tVector *cameraTarget)
{
    scroll_update_x(cameraTarget, false);
    
    scroll_update_y(cameraTarget, false);
    
    //show_debug("scFy: %.2f scPY: %i scTY: %i", fixtof(scroll.fixPos.y), scroll.pos.y, scroll.target.y);
    //show_debug("scVY: %.2f", fixtof(scroll.fixVel.y));
    //show_debug("wX: %i, wY: %i, lX:%i lY:%i", scroll.window.x, scroll.window.y, scroll.limit.x, scroll.limit.y);    
}

bool scroll_rect_on_region(tRectangle rect)
{	
    return  ((rect.pos.x + rect.size.x) >= scroll.pos.x - SCROLL_IN_REGION_OFFSET_X) && (rect.pos.x < ((scroll.pos.x + scroll.window.x) + SCROLL_IN_REGION_OFFSET_X)) &&
            ((rect.pos.y + rect.size.y) >= scroll.pos.y - SCROLL_IN_REGION_OFFSET_Y) && (rect.pos.y < ((scroll.pos.y + scroll.window.y) + SCROLL_IN_REGION_OFFSET_Y));
            
}

bool scroll_on_region(tVector checkPosition)
{
    return  checkPosition.y < (scroll.pos.y + scroll.window.y) && checkPosition.y > (scroll.pos.y) &&
            checkPosition.x < (scroll.pos.x + scroll.window.x) && checkPosition.x > (scroll.pos.x);
}

static void scroll_update_x(tVector *cameraTarget, bool init)
{
    //updates X scroll position
    switch (scroll.mode)    
    {
        //continuous follow camera
        case E_SCROLL_NORMAL_MODE:
        case E_SCROLL_BY_WINDOW_Y_MODE:
            if (cameraTarget->x > (scroll.window.x >> 1) + scroll.pos.x + SCROLL_OFFSET_X)
                scroll.pos.x = cameraTarget->x - (scroll.window.x >> 1) - SCROLL_OFFSET_X;
            else if ((cameraTarget->x < (scroll.window.x >> 1) + scroll.pos.x - SCROLL_OFFSET_X))
                scroll.pos.x = cameraTarget->x - (scroll.window.x >> 1) + SCROLL_OFFSET_X;
        break;
    }
    
    if (scroll.pos.x > scroll.stopScroll.right - scroll.window.x && scroll.stopScroll.right)
        scroll.pos.x = scroll.stopScroll.right - scroll.window.x;
    
    if (scroll.pos.x < scroll.stopScroll.left && scroll.stopScroll.left)
        scroll.pos.x = scroll.stopScroll.left;

    //MY_TRACE_FLAG("update x pos.x %i stopScroLeft %i\n", scroll.pos.x, scroll.stopScroll.left);

    //scroll.pos.x = (int16_t)clamp(scroll.pos.x, game.stopScrollLeft, game.stopScrollRight - scroll.window.x);
    //show_debug("stopRight %i stopLeft %i", scroll.stopScroll.right, scroll.stopScroll.left);  

    //limit scroll position
    scroll.pos.x = (int16_t)clamp(scroll.pos.x, 0, scroll.limit.x);
}

static void scroll_update_y(tVector *cameraTarget, bool init)
{
    //updates Y scroll position
    switch (scroll.mode)
    {
        //continuous follow camera: sets scroll position y to center cameraTarget +/- offset
        case E_SCROLL_NORMAL_MODE:
            if (cameraTarget->y > (scroll.window.y >> 1) + scroll.pos.y + SCROLL_OFFSET_Y)
                scroll.pos.y = cameraTarget->y - (scroll.window.y >> 1) - SCROLL_OFFSET_Y;
            else if (cameraTarget->y < (scroll.window.y >> 1) + scroll.pos.y - SCROLL_OFFSET_Y)
                scroll.pos.y = cameraTarget->y - (scroll.window.y >> 1) + SCROLL_OFFSET_Y;        
        break;
        //moves the scroll only when change the size of scroll window +/- range
        case E_SCROLL_BY_WINDOW_MODE:
        case E_SCROLL_BY_WINDOW_Y_MODE:
            //calculate Y scroll target
            if (!scroll.moving)
            {
                //check camera target to move scroll down one scroll window position
                if ((cameraTarget->y > (scroll.pos.y + scroll.window.y - SCROLL_BY_WINDOW_RANGE) && scroll.pos.y < scroll.limit.y) && (!scroll.stopScroll.down || init))
                    scroll.target.y = (int16_t)(floor(cameraTarget->y / scroll.window.y)) * scroll.window.y;
                //check camera target to move scroll up one scroll window position
                if ((cameraTarget->y < (scroll.pos.y - SCROLL_BY_WINDOW_RANGE) && scroll.pos.y > 0) && (!scroll.stopScroll.up || init))   
                    scroll.target.y = (int16_t)(floor(cameraTarget->y / scroll.window.y)) * scroll.window.y;
            }
            //show_debug("Scroll target y:%i", scroll.target.y);
            //show_debug("Floor %i", (int16_t)(floor(cameraTarget->y / scroll.window.y)));

            //set scroll velocity
            if (scroll.moving == E_SCROLL_MOVE_NONE && !init)
            {
                if (scroll.pos.y < scroll.target.y)
                {
                    scroll.fixVel.y = itofix(SCROLL_BY_WINDOW_VEL_Y);
                    scroll.moving = E_SCROLL_MOVE_DOWN;
                }
                else if (scroll.pos.y > scroll.target.y)
                {
                    scroll.fixVel.y = itofix(-SCROLL_BY_WINDOW_VEL_Y);
                    scroll.moving = E_SCROLL_MOVE_UP;
                }            
            }
            else     
            {
                if ((scroll.pos.y >= scroll.target.y && scroll.moving == E_SCROLL_MOVE_DOWN) ||
                    (scroll.pos.y <= scroll.target.y && scroll.moving == E_SCROLL_MOVE_UP)   ||
                    init)
                {
                    scroll.fixVel.y = 0;  
                    scroll.fixPos.y = itofix(scroll.target.y);
                    scroll.moving = E_SCROLL_MOVE_NONE;
                }
            }

            //add velocity
            scroll.fixPos.y = scroll.fixPos.y + scroll.fixVel.y;
            //update position        
            scroll.pos.y = fixtoi(scroll.fixPos.y);
        break;
    }

    //show_debug("StopScrollDown %i, StopScrollUp %i", scroll.stopScroll.down, scroll.stopScroll.up);
    //limit scroll position
    scroll.pos.y = (int16_t)clamp(scroll.pos.y, 0, scroll.limit.y);    
}

tVector scroll_get_position()
{
    return (tVector){scroll.pos.x, scroll.pos.y};
}

void scroll_stop_scroll(uint8_t dir, int16_t value)
{
    switch (dir)
    {
        case E_STOP_SCROLL_LEFT:
            scroll.stopScroll.left = value;
        break;
        case E_STOP_SCROLL_RIGHT:
            scroll.stopScroll.right = value;
        break;
        case E_STOP_SCROLL_DOWN:
            scroll.stopScroll.down = value;
        break;
        case E_STOP_SCROLL_UP:
            scroll.stopScroll.up = value;
        break;    
    }
}

int16_t scroll_get_stop_scroll(uint8_t dir)
{
    switch (dir)
    {
        case E_STOP_SCROLL_LEFT:
            return scroll.stopScroll.left;
        break;
        case E_STOP_SCROLL_RIGHT:
            return scroll.stopScroll.right;
        break;
        case E_STOP_SCROLL_DOWN:
            return scroll.stopScroll.down;
        break;
        case E_STOP_SCROLL_UP:
            return scroll.stopScroll.up;
        break;    
        default:
            return 0;
        break;
    }
}