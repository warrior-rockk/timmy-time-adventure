/********************************************************************
* Interface system
*
* 11/04/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include <string.h>

#include "allegro.h"
#include "interface.h"

BITMAP *interfaceSkin;                              //interface skin loaded on init
FONT *interfaceFont;                                //interface font loaded on init
BITMAP *dialogTiles[E_DIALOG_SKIN_NUM_TILES];       //tiles from skin to compose the dialog

void interface_init(BITMAP *_interfaceSkin, FONT *_interfaceFont)
{
    //sets the bitmap skin of interface
    interfaceSkin = _interfaceSkin;
    interfaceFont = _interfaceFont;

    //obtain dialog tiles
    for (uint8_t i = 0; i < E_DIALOG_SKIN_NUM_TILES; i++)
        dialogTiles[i] = create_sub_bitmap(interfaceSkin, i * interfaceSkin->h, 0, interfaceSkin->h, interfaceSkin->h);
}

void interface_destroy()
{
    //free interface tiles
    for (uint8_t i = 0; i < E_DIALOG_SKIN_NUM_TILES; i++)
        free(dialogTiles[i]);
    //destroy skin bitmap
    if (interfaceSkin)
        destroy_bitmap(interfaceSkin);
    //destroy skin font
    interfaceFont = NULL;        
    
    //TODO:free allocated memory: do this on dialog_destroy of each object

}

tDialog dialog_create(tRectangle dialogRect, int16_t backgroundColor, bool autoSize)
{
    //creates dialog object
    tDialog dialog;

    //sets dialog data
    dialog.optionSelected = 0;
    dialog.numOptions = 0;
    dialog.autoSize = autoSize;
    dialog.option = NULL;
    dialog.backgroundColor = backgroundColor;

    //sets size
    dialog.rect = dialogRect;
    if (autoSize)
    {
        //TODO: autoSize of x?
        //dialog.rect.size.x = interfaceSkin->h * 3;
        dialog.rect.size.y = interfaceSkin->h * 4;  //4 Y tiles minimun size
    }    

    //set drawing buffer for dialog oontainer
    dialog.drawContainer = create_bitmap(dialog.rect.size.x, dialog.rect.size.y);
    
    //returns dialog object
    return dialog;
}

void dialog_destroy(tDialog *dialog)
{
    if (dialog->drawContainer)
    {
        destroy_bitmap(dialog->drawContainer);        
    }
    if (dialog->option)
        free(dialog->option);

    free(dialog);
}

void dialog_add_option(tDialog *dialog, const char *optionText, uint8_t textColor)
{
    ASSERT(strlen(optionText) <= DIALOG_MAX_OPTION_LENGTH);
    
    //allocates option memory
    dialog->option  = realloc(dialog->option, (dialog->numOptions + 1) * sizeof(tDialogOption));
    //copies the data
    strcpy(dialog->option[dialog->numOptions].text, optionText);
    
    //init data
    dialog->option[dialog->numOptions].textColor    = textColor;
    dialog->option[dialog->numOptions].value        = NULL;
    dialog->option[dialog->numOptions].type         = E_OPTION_TYPE_NAVIGATION;
    dialog->option[dialog->numOptions].minValue     = 0;
    dialog->option[dialog->numOptions].maxValue     = 0;
    
    //increases num options
    dialog->numOptions++;   

    //resize container if autoSize flag
    if (dialog->autoSize)
    {
        //set new size
        //dialog->rect.size.x += interfaceSkin->h;
        dialog->rect.size.y += interfaceSkin->h;
        
        //set drawing buffer for dialog oontainer
        destroy_bitmap(dialog->drawContainer);
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
    
    //init data
    dialog->option[dialog->numOptions].textColor    = textColor;
    dialog->option[dialog->numOptions].value        = value;
    dialog->option[dialog->numOptions].type         = E_OPTION_TYPE_TEXTLIST;
    dialog->option[dialog->numOptions].minValue     = 0;
    dialog->option[dialog->numOptions].maxValue     = 0;
    
    //calculate max value
    for (uint8_t i = 0; strValues[i] != '\0'; i++)
    {
        if (strValues[i] == *(DIALOG_OPTIONS_DELIMITER)) 
            dialog->option[dialog->numOptions].maxValue++;
    }

    //increases num options
    dialog->numOptions++;   

    //resize container if resize active
    if (dialog->autoSize)
    {
        //set new size
        //dialog->rect.size.x += interfaceSkin->h;
        dialog->rect.size.y += interfaceSkin->h;

        //set drawing buffer for dialog oontainer
        destroy_bitmap(dialog->drawContainer);
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

    //init data
    dialog->option[dialog->numOptions].textColor    = textColor;
    dialog->option[dialog->numOptions].value        = value;
    dialog->option[dialog->numOptions].type         = E_OPTION_TYPE_NUMERIC;
    dialog->option[dialog->numOptions].minValue     = minValue;
    dialog->option[dialog->numOptions].maxValue     = maxValue;
    
    //increases num options
    dialog->numOptions++;   

    //resize container if resize active
    if (dialog->autoSize)
    {
        //set new size
        //dialog->rect.size.x += interfaceSkin->h;
        dialog->rect.size.y += interfaceSkin->h;
        
        //set drawing buffer for dialog container
        destroy_bitmap(dialog->drawContainer);
        dialog->drawContainer = create_bitmap(dialog->rect.size.x, dialog->rect.size.y);    
    }
}

void dialog_draw_container(tDialog *dialog)
{
    //clear container bitmap
    clear_to_color(dialog->drawContainer, dialog->backgroundColor);
    
    //draw corners
    draw_sprite(dialog->drawContainer, dialogTiles[E_DIALOG_SKIN_TILE_CORNER_LEFT_UP], 0, 0);                                       
    draw_sprite(dialog->drawContainer, dialogTiles[E_DIALOG_SKIN_TILE_CORNER_LEFT_DOWN], 0, dialog->rect.size.y - interfaceSkin->h);
    draw_sprite(dialog->drawContainer, dialogTiles[E_DIALOG_SKIN_TILE_CORNER_RIGHT_UP], dialog->rect.size.x - interfaceSkin->h, 0);
    draw_sprite(dialog->drawContainer, dialogTiles[E_DIALOG_SKIN_TILE_CORNER_RIGHT_DOWN], dialog->rect.size.x - interfaceSkin->h, dialog->rect.size.y - interfaceSkin->h);
    
    //draw horizontal lines
    for (uint8_t i = 1; i < (dialog->rect.size.x / interfaceSkin->h) - 1; i++)
    {
        draw_sprite(dialog->drawContainer, dialogTiles[E_DIALOG_SKIN_TILE_H_LINE_UP], (i * interfaceSkin->h), 0);
        draw_sprite(dialog->drawContainer, dialogTiles[E_DIALOG_SKIN_TILE_H_LINE_DOWN], (i * interfaceSkin->h), dialog->rect.size.y - interfaceSkin->h);       
    }
    
    //draw vertical lines
    for (uint8_t i = 1; i < (dialog->rect.size.y / interfaceSkin->h) - 1; i++)
    {
        draw_sprite(dialog->drawContainer, dialogTiles[E_DIALOG_SKIN_TILE_V_LINE_UP], 0, (i * interfaceSkin->h));
        draw_sprite(dialog->drawContainer, dialogTiles[E_DIALOG_SKIN_TILE_V_LINE_DOWN], dialog->rect.size.x - interfaceSkin->h, (i * interfaceSkin->h));       
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
                textprintf_ex(dialog->drawContainer, interfaceFont, (interfaceSkin->h << 1) + DIALOG_SPACING_X, posY + DIALOG_SPACING_Y, i == dialog->optionSelected ? dialog->option[i].textColor : 20, -1, "%s", dialog->option[i].text);
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
                textprintf_ex(dialog->drawContainer, interfaceFont, (interfaceSkin->h << 1) + DIALOG_SPACING_X, posY + DIALOG_SPACING_Y, i == dialog->optionSelected ? dialog->option[i].textColor : 20, -1, "%s %s", dialog->option[i].text, valueStrSelected);
            break;
            case E_OPTION_TYPE_NUMERIC:
                //print text option and numeric value
                textprintf_ex(dialog->drawContainer, interfaceFont, (interfaceSkin->h << 1) + DIALOG_SPACING_X, posY + DIALOG_SPACING_Y, i == dialog->optionSelected ? dialog->option[i].textColor : 20, -1, "%s %i", dialog->option[i].text, *(dialog->option[i].value));
            break;
        }

        //increment line position
        posY += text_height(interfaceFont);
    }
    
    //draw cursor
    draw_sprite(dialog->drawContainer, dialogTiles[E_DIALOG_SKIN_TILE_CURSOR], interfaceSkin->h, interfaceSkin->h + CURSOR_TEXT_OFFSET_Y + (text_height(interfaceFont) * dialog->optionSelected) + DIALOG_SPACING_Y);
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