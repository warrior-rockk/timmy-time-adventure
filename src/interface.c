/********************************************************************
* Interface system
*
* 11/04/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include "allegro.h"

#include "interface.h"

tDialog dialog_create(tRectangle dialogRect, BITMAP *tileSetStyle)
{
    //create dialog structure
    tDialog dialog;
    dialog.rect = dialogRect;
    dialog.dialogBuffer = create_bitmap(dialog.rect.size.x, dialog.rect.size.y);
    clear_bitmap(dialog.dialogBuffer);
    clear_to_color(dialog.dialogBuffer, 3);
    dialog.optionSelected = 0;

    BITMAP *dialogTiles[9];

    for (uint8_t i = 0; i < 9; i++)
        dialogTiles[i] = create_sub_bitmap(tileSetStyle, i * 8, 0, 8, 8);
    
    //corners
    draw_sprite(dialog.dialogBuffer, dialogTiles[0], 0, 0);                                             //leftUp
    draw_sprite(dialog.dialogBuffer, dialogTiles[1], 0, dialog.rect.size.y - 8);                        //LeftDown
    draw_sprite(dialog.dialogBuffer, dialogTiles[2], dialog.rect.size.x - 8, 0);                        //RightUp
    draw_sprite(dialog.dialogBuffer, dialogTiles[3], dialog.rect.size.x - 8, dialog.rect.size.y - 8);   //RightDown
    
    //lines
    for (uint8_t i = 1; i < (dialog.rect.size.x / 8) - 1; i++)
    {
        //up
        draw_sprite(dialog.dialogBuffer, dialogTiles[4], (i * 8), 0);
        //bottom
        draw_sprite(dialog.dialogBuffer, dialogTiles[5], (i * 8), dialog.rect.size.y - 8);       
    }
    
    //lines
    for (uint8_t i = 1; i < (dialog.rect.size.y / 8) - 1; i++)
    {
        //up
        draw_sprite(dialog.dialogBuffer, dialogTiles[6], 0, (i * 8));
        //bottom
        draw_sprite(dialog.dialogBuffer, dialogTiles[7], dialog.rect.size.x - 8, (i * 8));       
    }

    //return dialog object
    return dialog;
}

void dialog_add_options(tDialog dialog, FONT *textFont, const char *options, uint8_t selectedOption)
{
    //textout_centre_ex(buffer, gameFont, "DOS PLATFORM GAME", SCREEN_W>>1, 20, WHITE_COLOR, BLACK_COLOR);
    textout_ex(dialog.dialogBuffer, textFont, options, 2, 2, 10, -1);
}

void dialog_draw(BITMAP *drawBuffer, tDialog dialog)
{
    draw_sprite(drawBuffer, dialog.dialogBuffer, dialog.rect.pos.x, dialog.rect.pos.y);
}