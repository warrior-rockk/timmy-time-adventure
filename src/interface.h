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
#define DIALOG_TEXT_MARGIN_Y        2

#define DIALOG_OPTIONS_DELIMITER    ";"
#define DIALOG_MAX_OPTION_LENGTH    50

#include "utils.h"

enum E_DIALOG_OPTION_TYPE
{
    E_OPTION_TYPE_TEXTLIST,
    E_OPTION_TYPE_NUMERIC,
};

typedef struct 
{
    uint8_t type;                               //type of the option
    char text[DIALOG_MAX_OPTION_LENGTH];        //text of the option
    char strValues[DIALOG_MAX_OPTION_LENGTH];   //list of values for the option separated by delimiter
    int16_t minValue;                           //minimun value
    int16_t maxValue;                           //maximum value
    uint8_t textColor;                          //text color for option
    int16_t *value;                             //pointer to value data
} tDialogOption;

typedef struct
{
    BITMAP *drawContainer;                      //dialog draw backbuffer
    tRectangle rect;                            //dimensions and position
    tDialogOption *option;                      //array of options objects
    uint8_t numOptions;                         //num of options of the dialog
    uint8_t optionSelected;                     //current selected option
    uint8_t backgrounColor;                     //background color for dialog
    bool autoSize;                              //flag for adjusts the height automatically based on options
} tDialog;

void interface_init(BITMAP *_interfaceSkin, FONT *_interfaceFont);
void interface_destroy();

tDialog dialog_create(tRectangle dialogRect, int16_t backgroundColor, bool autoSize);
void dialog_add_option(tDialog *dialog, const char *textOptions, const char *strValues, uint8_t textColor, int16_t *value);
void dialog_add_num_option(tDialog *dialog, const char *textOptions, int16_t minValue, int16_t maxValue, uint8_t textColor, int16_t *value);

bool dialog_inc_option_value(tDialog *dialog);
bool dialog_dec_option_value(tDialog *dialog);
bool dialog_next_option(tDialog *dialog);
bool dialog_prev_option(tDialog *dialog);

void dialog_draw(tDialog *dialog, BITMAP *drawBuffer);

//void dialog_add_values(const char *values, uint8_t value);

//void dialog_select_option(uint8_t option);
//uint8_t dialog_option_selected();

#endif