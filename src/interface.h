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
#define DIALOG_MAX_OPTION_LENGTH    50

#include "utils.h"

typedef struct 
{
    char text[DIALOG_MAX_OPTION_LENGTH];
    char strValues[DIALOG_MAX_OPTION_LENGTH];
    int16_t value;
    uint8_t textColor;
} tDialogOption;

typedef struct
{
    BITMAP *drawContainer;
    tRectangle rect;
    tDialogOption *option;
    uint8_t numOptions;
    uint8_t optionSelected;
    uint8_t backgrounColor;
} tDialog;

void interface_init(BITMAP *_interfaceSkin, FONT *_interfaceFont);
void interface_destroy();

tDialog dialog_create(tRectangle dialogRect, int16_t backgroundColor);
void dialog_add_option(tDialog *dialog, const char *textOptions, const char *strValues, uint8_t textColor);

bool dialog_inc_option_value(tDialog *dialog);
bool dialog_dec_option_value(tDialog *dialog);

void dialog_draw(tDialog *dialog, BITMAP *drawBuffer);

//void dialog_add_values(const char *values, uint8_t value);

//void dialog_select_option(uint8_t option);
//uint8_t dialog_option_selected();

#endif