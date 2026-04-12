/********************************************************************
* Interface system
*
* 11/04/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include "allegro.h"

#include "interface.h"

tDialog dialog_create(tRectangle dialogRect, BITMAP *tileSetStyle, int16_t backgroundColor)
{
    //create dialog structure
    tDialog dialog;
    dialog.rect = dialogRect;
    dialog.dialogBuffer = create_bitmap(dialog.rect.size.x, dialog.rect.size.y);
    
    if (backgroundColor > 0)
        clear_to_color(dialog.dialogBuffer, backgroundColor);
    else
        clear_bitmap(dialog.dialogBuffer);
    
    dialog.optionSelected = 0;

    dialog.styleSize = tileSetStyle->h;

    BITMAP *dialogTiles[9];
    for (uint8_t i = 0; i < 9; i++)
        dialogTiles[i] = create_sub_bitmap(tileSetStyle, i * dialog.styleSize, 0, dialog.styleSize, dialog.styleSize);
    
    //corners
    draw_sprite(dialog.dialogBuffer, dialogTiles[0], 0, 0);                                             //leftUp
    draw_sprite(dialog.dialogBuffer, dialogTiles[1], 0, dialog.rect.size.y - dialog.styleSize);                        //LeftDown
    draw_sprite(dialog.dialogBuffer, dialogTiles[2], dialog.rect.size.x - dialog.styleSize, 0);                        //RightUp
    draw_sprite(dialog.dialogBuffer, dialogTiles[3], dialog.rect.size.x - dialog.styleSize, dialog.rect.size.y - dialog.styleSize);   //RightDown
    
    //lines
    for (uint8_t i = 1; i < (dialog.rect.size.x / dialog.styleSize) - 1; i++)
    {
        //up
        draw_sprite(dialog.dialogBuffer, dialogTiles[4], (i * dialog.styleSize), 0);
        //bottom
        draw_sprite(dialog.dialogBuffer, dialogTiles[5], (i * dialog.styleSize), dialog.rect.size.y - dialog.styleSize);       
    }
    
    //lines
    for (uint8_t i = 1; i < (dialog.rect.size.y / dialog.styleSize) - 1; i++)
    {
        //up
        draw_sprite(dialog.dialogBuffer, dialogTiles[6], 0, (i * dialog.styleSize));
        //bottom
        draw_sprite(dialog.dialogBuffer, dialogTiles[7], dialog.rect.size.x - dialog.styleSize, (i * dialog.styleSize));       
    }

    //return dialog object
    return dialog;
}

void dialog_add_options(tDialog dialog, FONT *textFont, const char *options, uint8_t textColor, BITMAP *cursor)
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

    //while ch != NULL (tokens left)
    while (ch)
    {
        //print text
        textprintf_ex(dialog.dialogBuffer, textFont, dialog.styleSize + cursor->w + DIALOG_SPACING_X, posY, textColor, -1, "%s", ch);

        //increment line position
        posY += text_height(textFont);
        
        //get next token
        ch = strtok(NULL, DIALOG_OPTIONS_DELIMITER);
    }

    //draw cursor
    masked_blit(cursor, dialog.dialogBuffer, 0, 0, dialog.styleSize, (cursor->h * dialog.optionSelected) + dialog.styleSize, cursor->w, cursor->h);
}

void dialog_draw(BITMAP *drawBuffer, tDialog dialog)
{
    draw_sprite(drawBuffer, dialog.dialogBuffer, dialog.rect.pos.x, dialog.rect.pos.y);
}