/********************************************************************
* Special FX definitions
*
* 22/08/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#ifndef _H_SFX_
#define _H_SFX_

#include "allegro.h"
#include "utils.h"

#define MAX_SPIDERS_FX      2   //num max of spider web sfx
#define SPIDER_WEB_COLOR    46  //from jurassic palette    

//initializes sfx system
void fx_init();
//draws special effects
void fx_draw(BITMAP *bufer);

//adds spider web sfx to draw queue
void fx_add_spider_web(tVector iniPos, tVector endPos);

#endif