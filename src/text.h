/********************************************************************
* Text system definitions
*
* 02/07/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#ifndef _H_TEXT_
#define _H_TEXT_

#include "allegro.h"

//draws multiline text separated by \n
void text_multiline_draw(BITMAP *buffer, FONT *font, char *text, uint16_t x, uint16_t y, uint8_t fontColor, uint8_t backColor);

void text_marquee_init(char *text, uint16_t x, uint16_t y);
void text_marquee_draw(BITMAP *buffer, FONT *font, uint8_t fontColor, uint8_t backColor, uint16_t speed);
#endif