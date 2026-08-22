/********************************************************************
* Special FX code
*
* 22/08/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/

#include "fx.h"
#include "game.h"

typedef struct 
{
    tVector iniPos;
    tVector endPos;
} tSpiderFx;

tSpiderFx spiderFx[MAX_SPIDERS_FX];
uint8_t numSpiderFx;

static void fx_draw_spider_web(BITMAP *buffer, tSpiderFx spiderFxPos)
{
    line(buffer, spiderFxPos.iniPos.x, spiderFxPos.iniPos.y, spiderFxPos.endPos.x, spiderFxPos.endPos.y, SPIDER_WEB_COLOR);
}

void fx_init()
{
    numSpiderFx = 0;
    for (uint8_t i = 0; i < MAX_SPIDERS_FX; i++)
    {
        spiderFx[i].iniPos = (tVector){0, 0};
        spiderFx[i].endPos = (tVector){0, 0};
    }
}

void fx_draw(BITMAP *buffer)
{
    bool existsFx = false;

    switch (game.actualLevel)
    {
        case E_GAME_LEVEL_JURASSIC:
            for (uint8_t i = 0; i < numSpiderFx; i++)
            {
                fx_draw_spider_web(buffer, spiderFx[i]);
                existsFx = true;        
            }          
        break;
    }

    //after draw, reset all effects for be assigned by entities
    if (existsFx)
        fx_init(); 
}

void fx_add_spider_web(tVector iniPos, tVector endPos)
{
    if (numSpiderFx < MAX_SPIDERS_FX)
    {
        spiderFx[numSpiderFx].iniPos = iniPos;
        spiderFx[numSpiderFx].endPos = endPos;
        numSpiderFx++;
    }
}

