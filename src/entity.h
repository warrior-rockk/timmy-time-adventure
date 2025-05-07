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

//inits entity system
void entity_system_init();
//adds entity object to entity list
void entity_add(tEntity entity);
//creates a new entity. Returns entity number
int16_t create_entity(tVector pos, BITMAP *img, void (*entity_update)(tEntity *entity));
//returns entity based on entity number
tEntity* get_entity(uint16_t numEntity);
//calls the update function on all entities
void entities_update();
//draws all entities on passed buffer
void entities_draw(BITMAP *buffer, tScroll *scroll);


#endif
