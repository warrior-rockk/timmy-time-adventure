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
    BITMAP *skin;
    BITMAP *drawBuffer;
    tRectangle rect;
    uint8_t numOptions;
    uint8_t optionSelected;
} tDialog;

void interface_init(BITMAP *interfaceSkin);
void interface_destroy();

tDialog *dialog_set(tRectangle dialogRect, int16_t backgroundColor);
void dialog_add_options(FONT *textFont, const char *options, uint8_t textColor, BITMAP *cursor);
void dialog_select_option(uint8_t option);
uint8_t dialog_option_selected();
void dialog_draw(BITMAP *drawBuffer);
#endif