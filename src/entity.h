/********************************************************************
* Entity system
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#ifndef _H_ENTITY_
#define _H_ENTITY_
#include <stdbool.h>

#include "allegro.h"
#include "globals.h"
#include "scroll.h"

//entity object
typedef struct tEntity
{
    tVector pos;
    tVector size;
    tFixVector fixPos;
    tFixVector fixVel;
    BITMAP *img;
    bool ground;
    bool jump;
    void (*entity_update)(struct tEntity *entity);
} tEntity;


void entity_add(tEntity entity);
int16_t create_entity(tVector pos, BITMAP *img, void (*entity_update)(tEntity *entity));
tEntity* get_entity(uint16_t numEntity);
void entities_update();
void entities_draw(BITMAP *buffer, tScroll *scroll);


#endif
