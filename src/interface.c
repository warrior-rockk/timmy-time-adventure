/********************************************************************
* Interface system
*
* 11/04/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include "allegro.h"

#include "interface.h"

tDialog dialog;

void interface_init(BITMAP *interfaceSkin)
{
    //sets the bitmap skin of interface
    dialog.skin = interfaceSkin;   
    //reset dialog data
    memset(&dialog.rect, 0, sizeof(dialog.rect));
    dialog.optionSelected = 0;
    dialog.numOptions = 0;
    dialog.drawBuffer = NULL;
}

tDialog *dialog_set(tRectangle dialogRect, int16_t backgroundColor)
{
    //set size
    dialog.rect = dialogRect;
    //set drawing buffer
    if (dialog.drawBuffer)
        destroy_bitmap(dialog.drawBuffer);
    dialog.drawBuffer = create_bitmap(dialog.rect.size.x, dialog.rect.size.y);
    //set background color
    if (backgroundColor > 0)
        clear_to_color(dialog.drawBuffer, backgroundColor);
    else
        clear_bitmap(dialog.drawBuffer);
    
    //obtain dialog tiles
    BITMAP *dialogTiles[9];
    for (uint8_t i = 0; i < 9; i++)
        dialogTiles[i] = create_sub_bitmap(dialog.skin, i * dialog.skin->h, 0, dialog.skin->h, dialog.skin->h);
    
    //draw corners
    draw_sprite(dialog.drawBuffer, dialogTiles[0], 0, 0);                                                           //leftUp
    draw_sprite(dialog.drawBuffer, dialogTiles[1], 0, dialog.rect.size.y - dialog.skin->h);                        //LeftDown
    draw_sprite(dialog.drawBuffer, dialogTiles[2], dialog.rect.size.x - dialog.skin->h, 0);                        //RightUp
    draw_sprite(dialog.drawBuffer, dialogTiles[3], dialog.rect.size.x - dialog.skin->h, dialog.rect.size.y - dialog.skin->h);   //RightDown
    
    //draw horizontal lines
    for (uint8_t i = 1; i < (dialog.rect.size.x / dialog.skin->h) - 1; i++)
    {
        //up
        draw_sprite(dialog.drawBuffer, dialogTiles[4], (i * dialog.skin->h), 0);
        //bottom
        draw_sprite(dialog.drawBuffer, dialogTiles[5], (i * dialog.skin->h), dialog.rect.size.y - dialog.skin->h);       
    }
    
    //draw vertical lines
    for (uint8_t i = 1; i < (dialog.rect.size.y / dialog.skin->h) - 1; i++)
    {
        //up
        draw_sprite(dialog.drawBuffer, dialogTiles[6], 0, (i * dialog.skin->h));
        //bottom
        draw_sprite(dialog.drawBuffer, dialogTiles[7], dialog.rect.size.x - dialog.skin->h, (i * dialog.skin->h));       
    }

    return &dialog;
}

void dialog_add_options(FONT *textFont, const char *options, uint8_t textColor, BITMAP *cursor)
{
    int posY;
    char s[DIALOG_MAX_OPTION_LENGTH];
    char *ch;
    
    //make a copy of the string for tokenizer
    strcpy(s, options);
    //first token
    ch = strtok(s, DIALOG_OPTIONS_DELIMITER);
    //sets the initial Y text position
    posY = cursor->h;

    dialog.numOptions = 0;

    //while ch != NULL (tokens left)
    while (ch)
    {
        //print text
        textprintf_ex(dialog.drawBuffer, textFont, dialog.skin->h + cursor->w + DIALOG_SPACING_X, posY + DIALOG_SPACING_Y, textColor, -1, "%s", ch);

        //increment line position
        posY += text_height(textFont);
        
        //get next token
        ch = strtok(NULL, DIALOG_OPTIONS_DELIMITER);

        dialog.numOptions++;
    }

    //draw cursor
    //masked_blit(cursor, dialog.drawBuffer, 0, 0, dialog.skin->h, ((cursor->h) * dialog.optionSelected) + dialog.skin->h + DIALOG_SPACING_Y, cursor->w, cursor->h);
    masked_blit(cursor, dialog.drawBuffer, 0, 0, dialog.skin->h + cursor->w + DIALOG_SPACING_X, (text_height(textFont) * dialog.optionSelected) + DIALOG_SPACING_Y, cursor->w, cursor->h);
}

void dialog_select_option(uint8_t option)
{
    dialog.optionSelected = option;
}

uint8_t dialog_option_selected()
{
    return dialog.optionSelected;
}

void dialog_draw(BITMAP *drawBuffer)
{
    draw_sprite(drawBuffer, dialog.drawBuffer, dialog.rect.pos.x, dialog.rect.pos.y);
}