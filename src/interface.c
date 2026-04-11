/********************************************************************
* Interface system
*
* 11/04/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include "allegro.h"

#include "interface.h"

void dialog_draw(BITMAP *drawBitmapBuffer, tRectangle dialogRect, BITMAP *tileSetStyle)
{
    
    BITMAP *dialogTiles[9];

    for (uint8_t i = 0; i < 9; i++)
        dialogTiles[i] = create_sub_bitmap(tileSetStyle, i * 8, 0, 8, 8);
    
    //corners
    draw_sprite(drawBitmapBuffer, dialogTiles[0], dialogRect.pos.x, dialogRect.pos.y);                                          //leftUp
    draw_sprite(drawBitmapBuffer, dialogTiles[1], dialogRect.pos.x, dialogRect.pos.y + dialogRect.size.y);                      //LeftDown
    draw_sprite(drawBitmapBuffer, dialogTiles[2], dialogRect.pos.x + dialogRect.size.x, dialogRect.pos.y);                      //RightUp
    draw_sprite(drawBitmapBuffer, dialogTiles[3], dialogRect.pos.x + dialogRect.size.x, dialogRect.pos.y + dialogRect.size.y);  //RightDown
    
    //lines
    for (uint8_t i = 1; i < (dialogRect.size.x / 8); i++)
    {
        //up
        draw_sprite(drawBitmapBuffer, dialogTiles[4], dialogRect.pos.x + (i * 8), dialogRect.pos.y);
        //bottom
        draw_sprite(drawBitmapBuffer, dialogTiles[5], dialogRect.pos.x + (i * 8), dialogRect.pos.y + dialogRect.size.y);       
    }
    
    //lines
    for (uint8_t i = 1; i < (dialogRect.size.y / 8); i++)
    {
        //up
        draw_sprite(drawBitmapBuffer, dialogTiles[6], dialogRect.pos.x, dialogRect.pos.y + (i * 8));
        //bottom
        draw_sprite(drawBitmapBuffer, dialogTiles[7], dialogRect.pos.x + dialogRect.size.x, dialogRect.pos.y + (i * 8));       
    }
}