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
    tVector pos;            //entity position
    tVector size;           //entity size
    tFixVector fixPos;      //entity fixed position
    tFixVector fixVel;      //entity fixed velocities
    tVector initPos;        //entity initial position
    BITMAP *img;            //actual entity frame image
    uint8_t state;          //actual entity state
    uint8_t prevState;      //previous entity state
    uint8_t entType;        //entity type
    uint8_t entInstance;    //entity instance of type
    bool dead;              //dead flag
    bool ground;            //ground flag
    bool jump;              //jump flag
    //function pointers
    void (*entity_init)(struct tEntity *entity);
    void (*entity_create)(struct tEntity *entity);
    void (*entity_update)(struct tEntity *entity);
} tEntity;

//inits entity system
void entity_system_init();
//adds entity object to entity list
void entity_add(tEntity entity);
//creates a new entity. Returns entity number
int16_t entity_create(tVector initPos, BITMAP *img, uint8_t entType, void (*entity_init)(tEntity *entity), void (*entity_create)(tEntity *entity), void (*entity_update)(tEntity *entity));
//returns entity based on entity number
tEntity* get_entity(uint16_t numEntity);
//calls the init function on all entitites
void entities_init();
//calls the update function on all entities
void entities_update();
//draws all entities on passed buffer
void entities_draw(BITMAP *buffer, tScroll *scroll);


#endif
