/********************************************************************
* Interface system
*
* 11/04/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include <string.h>

#include "allegro.h"

#include "interface.h"

//inteface skin loaded on init
BITMAP *interfaceSkin;
FONT *interfaceFont;
BITMAP *dialogTiles[9];

void interface_init(BITMAP *_interfaceSkin, FONT *_interfaceFont)
{
    //sets the bitmap skin of interface
    interfaceSkin = _interfaceSkin;
    interfaceFont = _interfaceFont;

    //obtain dialog tiles
    for (uint8_t i = 0; i < 9; i++)
        dialogTiles[i] = create_sub_bitmap(interfaceSkin, i * interfaceSkin->h, 0, interfaceSkin->h, interfaceSkin->h);
}

tDialog dialog_create(tRectangle dialogRect, int16_t backgroundColor, bool autoSize)
{
    //creates dialog object
    tDialog dialog;

    //reset dialog data
    dialog.optionSelected = 0;
    dialog.numOptions = 0;
    dialog.autoSize = autoSize;

    //set size
    dialog.rect = dialogRect;
    if (autoSize)
    {
        //TODO: autoSize of x?
        //dialog.rect.size.x = interfaceSkin->h * 3;
        dialog.rect.size.y = interfaceSkin->h * 4;
    }    

    //set drawing buffer for dialog oontainer
    dialog.drawContainer = create_bitmap(dialog.rect.size.x, dialog.rect.size.y);
    //set background color
    dialog.backgrounColor = backgroundColor;
    dialog.option = NULL;
    
    //returns dialog object
    return dialog;
}

void dialog_add_option(tDialog *dialog, const char *optionText, uint8_t textColor)
{
    ASSERT(strlen(optionText) <= DIALOG_MAX_OPTION_LENGTH);
    
    //allocates option memory
    dialog->option  = realloc(dialog->option, (dialog->numOptions + 1) * sizeof(tDialogOption));
    //copies the data
    strcpy(dialog->option[dialog->numOptions].text, optionText);
    
    //count values
    dialog->option[dialog->numOptions].minValue = 0;
    dialog->option[dialog->numOptions].maxValue = 0;
    
    //init data
    dialog->option[dialog->numOptions].textColor    = textColor;
    dialog->option[dialog->numOptions].value        = NULL;
    dialog->option[dialog->numOptions].type         = E_OPTION_TYPE_NAVIGATION;
    //increases num options
    dialog->numOptions++;   

    //resize container if resize active
    if (dialog->autoSize)
    {
        //set new size
        //dialog->rect.size.x += interfaceSkin->h;
        dialog->rect.size.y += interfaceSkin->h;
        destroy_bitmap(dialog->drawContainer);
        //set drawing buffer for dialog oontainer
        dialog->drawContainer = create_bitmap(dialog->rect.size.x, dialog->rect.size.y);    
    }
}

void dialog_add_text_option(tDialog *dialog, const char *textOptions, const char *strValues, uint8_t textColor, int16_t *value)
{
    ASSERT(strlen(textOptions) <= DIALOG_MAX_OPTION_LENGTH);
    ASSERT(strlen(strValues) <= DIALOG_MAX_OPTION_LENGTH);

    //allocates option memory
    dialog->option  = realloc(dialog->option, (dialog->numOptions + 1) * sizeof(tDialogOption));
    //copies the data
    strcpy(dialog->option[dialog->numOptions].text, textOptions);
    strcpy(dialog->option[dialog->numOptions].strValues, strValues);
    
    //count values
    dialog->option[dialog->numOptions].minValue = 0;
    dialog->option[dialog->numOptions].maxValue = 0;
    for (uint8_t i = 0; strValues[i] != '\0'; i++)
    {
        if (strValues[i] == ';') 
            dialog->option[dialog->numOptions].maxValue++;
    }

    //init data
    dialog->option[dialog->numOptions].textColor    = textColor;
    dialog->option[dialog->numOptions].value        = value;
    dialog->option[dialog->numOptions].type         = E_OPTION_TYPE_TEXTLIST;
    //increases num options
    dialog->numOptions++;   

    //resize container if resize active
    if (dialog->autoSize)
    {
        //set new size
        //dialog->rect.size.x += interfaceSkin->h;
        dialog->rect.size.y += interfaceSkin->h;
        destroy_bitmap(dialog->drawContainer);
        //set drawing buffer for dialog oontainer
        dialog->drawContainer = create_bitmap(dialog->rect.size.x, dialog->rect.size.y);    
    }
}

void dialog_add_num_option(tDialog *dialog, const char *textOptions, int16_t minValue, int16_t maxValue, uint8_t textColor, int16_t *value)
{
    ASSERT(strlen(textOptions) <= DIALOG_MAX_OPTION_LENGTH);
    
    //allocates option memory
    dialog->option  = realloc(dialog->option, (dialog->numOptions + 1) * sizeof(tDialogOption));
    //copies the data
    strcpy(dialog->option[dialog->numOptions].text, textOptions);
    strcpy(dialog->option[dialog->numOptions].strValues, "");

    //set value limits
    dialog->option[dialog->numOptions].minValue = minValue;
    dialog->option[dialog->numOptions].maxValue = maxValue;
    
    //init data
    dialog->option[dialog->numOptions].textColor = textColor;
    dialog->option[dialog->numOptions].value     = value;
    dialog->option[dialog->numOptions].type      = E_OPTION_TYPE_NUMERIC;
    //increases num options
    dialog->numOptions++;   

    //resize container if resize active
    if (dialog->autoSize)
    {
        //set new size
        //dialog->rect.size.x += interfaceSkin->h;
        dialog->rect.size.y += interfaceSkin->h;
        destroy_bitmap(dialog->drawContainer);
        //set drawing buffer for dialog container
        dialog->drawContainer = create_bitmap(dialog->rect.size.x, dialog->rect.size.y);    
    }
}

void dialog_draw_container(tDialog *dialog)
{
    //clear container bitmap
    clear_to_color(dialog->drawContainer, dialog->backgrounColor);
    
    //draw corners
    draw_sprite(dialog->drawContainer, dialogTiles[0], 0, 0);                                                           //leftUp
    draw_sprite(dialog->drawContainer, dialogTiles[1], 0, dialog->rect.size.y - interfaceSkin->h);                        //LeftDown
    draw_sprite(dialog->drawContainer, dialogTiles[2], dialog->rect.size.x - interfaceSkin->h, 0);                        //RightUp
    draw_sprite(dialog->drawContainer, dialogTiles[3], dialog->rect.size.x - interfaceSkin->h, dialog->rect.size.y - interfaceSkin->h);   //RightDown
    
    //draw horizontal lines
    for (uint8_t i = 1; i < (dialog->rect.size.x / interfaceSkin->h) - 1; i++)
    {
        //up
        draw_sprite(dialog->drawContainer, dialogTiles[4], (i * interfaceSkin->h), 0);
        //bottom
        draw_sprite(dialog->drawContainer, dialogTiles[5], (i * interfaceSkin->h), dialog->rect.size.y - interfaceSkin->h);       
    }
    
    //draw vertical lines
    for (uint8_t i = 1; i < (dialog->rect.size.y / interfaceSkin->h) - 1; i++)
    {
        //up
        draw_sprite(dialog->drawContainer, dialogTiles[6], 0, (i * interfaceSkin->h));
        //bottom
        draw_sprite(dialog->drawContainer, dialogTiles[7], dialog->rect.size.x - interfaceSkin->h, (i * interfaceSkin->h));       
    }
}

void dialog_draw_options(tDialog *dialog)
{
    int posY;
    char s[DIALOG_MAX_OPTION_LENGTH];
    char *ch;
    char valueStrSelected[DIALOG_MAX_OPTION_LENGTH];
    int optionLine = 0;

    //sets the initial Y text position
    posY = interfaceSkin->h;

    for (uint8_t i = 0; i < dialog->numOptions; i++)
    {
        //check option type
        switch (dialog->option[i].type)
        {
            case E_OPTION_TYPE_NAVIGATION:
                //print text option
                textprintf_ex(dialog->drawContainer, interfaceFont, (interfaceSkin->h << 1) + DIALOG_SPACING_X, posY + DIALOG_SPACING_Y, dialog->option[i].textColor, -1, "%s", dialog->option[i].text);
            break;
            case E_OPTION_TYPE_TEXTLIST:
                optionLine = 0;
                strcpy(valueStrSelected, "");
                //make a copy of the string for tokenizer
                strcpy(s, dialog->option[i].strValues);
                //first token
                ch = strtok(s, DIALOG_OPTIONS_DELIMITER);
                //while tokens left
                while (ch)
                {
                    if (optionLine == *(dialog->option[i].value))
                    {
                        strcpy(valueStrSelected, ch);
                        break;
                    }

                    //get next token
                    ch = strtok(NULL, DIALOG_OPTIONS_DELIMITER);
                    optionLine++;
                }

                //print text option and value
                textprintf_ex(dialog->drawContainer, interfaceFont, (interfaceSkin->h << 1) + DIALOG_SPACING_X, posY + DIALOG_SPACING_Y, dialog->option[i].textColor, -1, "%s %s", dialog->option[i].text, valueStrSelected);
            break;
            case E_OPTION_TYPE_NUMERIC:
                //print text option and numeric value
                textprintf_ex(dialog->drawContainer, interfaceFont, (interfaceSkin->h << 1) + DIALOG_SPACING_X, posY + DIALOG_SPACING_Y, dialog->option[i].textColor, -1, "%s %i", dialog->option[i].text, *(dialog->option[i].value));
            break;
        }

        //increment line position
        posY += text_height(interfaceFont);
    }
    
    //draw cursor
    draw_sprite(dialog->drawContainer, dialogTiles[8], interfaceSkin->h, interfaceSkin->h + DIALOG_TEXT_MARGIN_Y + (text_height(interfaceFont) * dialog->optionSelected) + DIALOG_SPACING_Y);
}

void dialog_draw(tDialog *dialog, BITMAP *drawBuffer)
{
    dialog_draw_container(dialog);
    dialog_draw_options(dialog);

    draw_sprite(drawBuffer, dialog->drawContainer, dialog->rect.pos.x, dialog->rect.pos.y);
}

bool dialog_inc_option_value(tDialog *dialog)
{
    if (dialog->option[dialog->optionSelected].value)
    {
        if (*(dialog->option[dialog->optionSelected].value) < dialog->option[dialog->optionSelected].maxValue)
        {
            *(dialog->option[dialog->optionSelected].value) += 1;   
            return true;
        }
        else
            return false;
        }
    else
        return false;
}

bool dialog_dec_option_value(tDialog *dialog)
{
    if (dialog->option[dialog->optionSelected].value)
    {
        if (*(dialog->option[dialog->optionSelected].value) > dialog->option[dialog->optionSelected].minValue)
        {
            *(dialog->option[dialog->optionSelected].value) -= 1;
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

bool dialog_next_option(tDialog *dialog)
{
    if (dialog->optionSelected < dialog->numOptions - 1)
    {
        dialog->optionSelected++;
        return true;
    }
    else
        return false;
}

bool dialog_prev_option(tDialog *dialog)
{
    if (dialog->optionSelected > 0)
    {
        dialog->optionSelected--;
        return true;
    }
    else
        return false;
}