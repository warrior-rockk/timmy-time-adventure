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
    E_OPTION_TYPE_NAVIGATION,
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
    uint8_t backgroundColor;                     //background color for dialog
    bool autoSize;                              //flag for adjusts the height automatically based on options
} tDialog;

//initializates the interface system
void interface_init(BITMAP *_interfaceSkin, FONT *_interfaceFont);
//destroys the interface system
void interface_destroy();

//creates a interface dialog with specified dimensions and position, background color and flag to autosize the height based on number of options
tDialog dialog_create(tRectangle dialogRect, int16_t backgroundColor, bool autoSize);
//adds option to specified dialog
void dialog_add_option(tDialog *dialog, const char *optionText, uint8_t textColor);
//adds a text list option to specified dialog
void dialog_add_text_option(tDialog *dialog, const char *textOptions, const char *strValues, uint8_t textColor, int16_t *value);
//adds a numeric option to specified dialog
void dialog_add_num_option(tDialog *dialog, const char *textOptions, int16_t minValue, int16_t maxValue, uint8_t textColor, int16_t *value);

//increases selected option. Return true if not the end of options
bool dialog_next_option(tDialog *dialog);
//decreases selected option. Return true if not the start of options
bool dialog_prev_option(tDialog *dialog);
//increases the value of selected option. Return true if not the end of value
bool dialog_inc_option_value(tDialog *dialog);
//decreases the value of selected option. Return true if not the start of value
bool dialog_dec_option_value(tDialog *dialog);

//draws the dialog on specifed draw buffer
void dialog_draw(tDialog *dialog, BITMAP *drawBuffer);

#endif