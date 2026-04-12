/********************************************************************
* Interface definitions
*
* 11/04/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/

#ifndef _H_INTERFACE_
#define _H_INTERFACE_

#include "utils.h"

typedef struct
{
    tRectangle rect;
    BITMAP *dialogBuffer;
    uint8_t optionSelected;
} tDialog;

tDialog dialog_create(tRectangle dialogRect, BITMAP *tileSetStyle);
void dialog_add_options(tDialog dialog, FONT *textFont, const char *options, uint8_t selectedOption);

void dialog_draw(BITMAP *drawBuffer, tDialog dialog);
#endif