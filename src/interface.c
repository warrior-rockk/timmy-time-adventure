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

void interface_init(BITMAP *_interfaceSkin, FONT *_interfaceFont)
{
    //sets the bitmap skin of interface
    interfaceSkin = _interfaceSkin;
    interfaceFont = _interfaceFont;
}

tDialog dialog_create(tRectangle dialogRect, int16_t backgroundColor)
{
    //creates dialog object
    tDialog dialog;

    //reset dialog data
    dialog.optionSelected = 0;
    dialog.numOptions = 0;

    //set size
    dialog.rect = dialogRect;
    //set drawing buffer for dialog oontainer
    dialog.drawContainer = create_bitmap(dialog.rect.size.x, dialog.rect.size.y);
    //set background color
    if (backgroundColor > 0)
        clear_to_color(dialog.drawContainer, backgroundColor);
    else
        clear_bitmap(dialog.drawContainer);

    dialog.option = NULL;
    
    //returns dialog object
    return dialog;
}

void dialog_add_option(tDialog *dialog, const char *textOptions, const char *strValues, uint8_t textColor)
{
    //allocates option memory
    dialog->option  = realloc(dialog->option, (dialog->numOptions + 1) * sizeof(tDialogOption));
    //copies the data
    strcpy(dialog->option[dialog->numOptions].text, textOptions);
    strcpy(dialog->option[dialog->numOptions].strValues, strValues);
    
    //init data
    dialog->option[dialog->numOptions].value = 0;
    dialog->option[dialog->numOptions].textColor = textColor;

    //increases num options
    dialog->numOptions++;   
}

/*void dialog_select_option(uint8_t option)
{
    dialog->optionSelected = option;
}

uint8_t dialog_option_selected()
{
    return dialog->optionSelected;
}
*/

void dialog_draw_container(tDialog *dialog)
{
    //obtain dialog tiles
    BITMAP *dialogTiles[9];
    for (uint8_t i = 0; i < 9; i++)
        dialogTiles[i] = create_sub_bitmap(interfaceSkin, i * interfaceSkin->h, 0, interfaceSkin->h, interfaceSkin->h);
    
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
    char *valueStrSelected;
    int optionLine = 0;

    //sets the initial Y text position
    posY = interfaceSkin->h;
    
    for (uint8_t i = 0; i < dialog->numOptions; i++)
    {
        //make a copy of the string for tokenizer
        strcpy(s, dialog->option[i].strValues);
        //first token
        ch = strtok(s, DIALOG_OPTIONS_DELIMITER);
        if (dialog->option[i].value == 0)
            strcpy(valueStrSelected, ch);
        else
        {
            //while ch != NULL (tokens left)
            while (ch)
            {
                optionLine++;

                if (optionLine == dialog->option[i].value)
                    strcpy(valueStrSelected, ch);
                
                //get next token
                ch = strtok(NULL, DIALOG_OPTIONS_DELIMITER);
            }
        }

        //print text option and value
        textprintf_ex(dialog->drawContainer, interfaceFont, interfaceSkin->h + DIALOG_SPACING_X, posY + DIALOG_SPACING_Y, dialog->option[i].textColor, -1, "%s %s", dialog->option[i].text, valueStrSelected);
        //increment line position
        posY += text_height(interfaceFont);
    }
    
    //draw cursor
    //masked_blit(cursor, dialog->drawContainer, 0, 0, interfaceSkin->h, ((cursor->h) * dialog->optionSelected) + interfaceSkin->h + DIALOG_SPACING_Y, cursor->w, cursor->h);
    //masked_blit(cursor, dialog->drawContainer, 0, 0, interfaceSkin->h + cursor->w + DIALOG_SPACING_X, (text_height(textFont) * dialog->optionSelected) + DIALOG_SPACING_Y, cursor->w, cursor->h);
}

void dialog_draw(tDialog *dialog, BITMAP *drawBuffer)
{
    dialog_draw_container(dialog);
    dialog_draw_options(dialog);

    draw_sprite(drawBuffer, dialog->drawContainer, dialog->rect.pos.x, dialog->rect.pos.y);
}