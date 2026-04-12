/********************************************************************
* Interface definitions
*
* 11/04/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/

#ifndef _H_INTERFACE_
#define _H_INTERFACE_

#define DIALOG_SPACING_X            2
#define DIALOG_SPACING_Y            2

#define DIALOG_OPTIONS_DELIMITER    ";"
#define DIALOG_MAX_OPTION_LENGTH    20

#include "utils.h"

typedef struct
{
    tRectangle rect;
    BITMAP *dialogBuffer;
    uint8_t optionSelected;
    uint8_t styleSize;
} tDialog;

tDialog dialog_create(tRectangle dialogRect, BITMAP *tileSetStyle, int16_t backgroundColor);
void dialog_add_options(tDialog dialog, FONT *textFont, const char *options, uint8_t textColor, BITMAP *cursor);

void dialog_draw(BITMAP *drawBuffer, tDialog dialog);
#endif