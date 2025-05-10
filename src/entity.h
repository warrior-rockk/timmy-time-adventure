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

#define ENTITY_MAX_NUM      255     //Max. number of entities

//entity object
typedef struct tEntity
{
    tVector pos;
    tVector size;
    tFixVector fixPos;
    tFixVector fixVel;
    BITMAP *img;
    uint8_t state;
    uint8_t prevState;
    uint8_t entType;
    uint8_t entInstance;
    bool ground;
    bool jump;
    void (*entity_create)(struct tEntity *entity);
    void (*entity_update)(struct tEntity *entity);
} tEntity;

//inits entity system
void entity_system_init();
//adds entity object to entity list
void entity_add(tEntity entity);
//creates a new entity. Returns entity number
int16_t entity_create(tVector pos, BITMAP *img, uint8_t entType, void (*entity_create)(tEntity *entity), void (*entity_update)(tEntity *entity));
//returns entity based on entity number
tEntity* get_entity(uint16_t numEntity);
//calls the update function on all entities
void entities_update();
//draws all entities on passed buffer
void entities_draw(BITMAP *buffer, tScroll *scroll);


#endif
