/********************************************************************
* Text system
*
* 02/07/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include "text.h"

void text_multiline_draw(BITMAP *buffer, FONT *font, char *text, uint16_t x, uint16_t y, uint8_t fontColor, uint8_t backColor)
{
    uint16_t posY;
    char s[strlen(text)];    
    char *ch;

    //make a copy of the string for tokenizer
    strcpy(s, text);
    //first token
    ch = strtok(s, "\n");
    //sets the initial Y text position
    posY = y;

    //while ch != NULL (tokens left)
    while (ch)
    {
        //print text
        textout_centre_ex(buffer, font, ch, x, posY, fontColor, backColor);

        //increment line position
        posY += text_height(font);
        
        //get next token
        ch = strtok(NULL, "\n");
    }
}
