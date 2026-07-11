/********************************************************************
* Text system
*
* 02/07/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include <stdio.h>
#include <string.h>

#include "text.h"
#include "timer.h"

char marqueeTxt[300];
uint16_t marqueeX, marqueeY;
uint16_t marqueeActualPos = 0;
int16_t marqueeActualX = 0;
uint16_t marqueeTime = 0;

void text_multiline_draw(BITMAP *buffer, FONT *font, char *text, uint16_t x, uint16_t y, uint8_t fontColor, uint8_t backColor)
{
    uint16_t posY;
    char *s;    
    char *ch;

    //make a copy of the string for tokenizer
    s = strdup(text);
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

void text_marquee_init(char *text, uint16_t x, uint16_t y)
{
    ASSERT(strlen(text) < 300);

    strcpy(marqueeTxt, text);
    marqueeX = x;
    marqueeY = y;
    marqueeActualPos = 0;
    marqueeActualX   = 0;
    marqueeTime = 0;
}

uint8_t text_marquee_draw(BITMAP *buffer, FONT *font, uint8_t fontColor, uint8_t backColor, uint16_t speed)
{
    if (marqueeActualPos < strlen(marqueeTxt))
    {
        if (marqueeTime >= speed)
        {
            char letter[2];
            sprintf(letter, "%c", marqueeTxt[marqueeActualPos]);    
            if (letter[0] == '\n')
            {
                marqueeY += text_height(font);
                marqueeActualX = 0;
            }

            text_multiline_draw(buffer, font, letter, marqueeX + (marqueeActualX * 8), marqueeY, fontColor, backColor);
            marqueeActualPos++;
            marqueeActualX++;
            marqueeTime = 0;
        }
        else
            marqueeTime += clock_tick_get();

        return 0;
    }
    else
        return 1;
}
