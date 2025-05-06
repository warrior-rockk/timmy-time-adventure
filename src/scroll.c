#include "scroll.h"

tScroll create_scroll(tVector window, tVector limit)
{
    tScroll scroll;
    scroll.pos.x = 0;
    scroll.pos.y = 0;
    scroll.window = window;
    scroll.limit = limit;

    return scroll;
}

void scroll_init(tScroll *scroll)
{
    scroll->pos.x = 0;
    scroll->pos.y = 0;
}

void scroll_update(tScroll *scroll, tVector *targetPos)
{
    //test: follow player 
    //note: in sms games, the scroll moves when the player is offset to center in the direction of movement. A few pixels to right when move to right and viceversa
    //if the player is in the offset zone, the scroll doesn't move allowing a tiny zone with not scroll movement
    const uint16_t scrollOffset = 8;

    if (targetPos->x > (scroll->window.x >> 1) + scroll->pos.x + scrollOffset)
        scroll->pos.x = targetPos->x - (scroll->window.x >> 1) - scrollOffset;
    else if (targetPos->x < (scroll->window.x >> 1) + scroll->pos.x - scrollOffset)
        scroll->pos.x = targetPos->x - (scroll->window.x >> 1) + scrollOffset;
    
    //scroll limits
    if (scroll->pos.x > scroll->limit.x - 1)
        scroll->pos.x = scroll->limit.x - 1;
              
    if (scroll->pos.x < 0)
        scroll->pos.x = 0;
}