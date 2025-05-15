/********************************************************************
* Entity system
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#ifndef _H_ENTITY_
#define _H_ENTITY_
#include <stdbool.h>

#include "allegro.h"
#include "globals.h"
#include "scroll.h"

#define ENTITY_MAX_NUM      255     //Max. number of entities

enum E_ENTITY_PROPERTIES
{
    E_COLLISIONS_ON_PROPERTY = 0x01,
};

//entity object
typedef struct tEntity
{
    uint16_t id;            //entity id number
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
    uint16_t properties;    //entity properties
    bool dead;              //dead flag
    bool visible;           //visible flag
    bool ground;            //ground flag
    bool jump;              //jump flag
    //function pointers
    void (*entity_init)(struct tEntity *entity);
    void (*entity_create)(struct tEntity *entity);
    void (*entity_update)(struct tEntity *entity);
} tEntity;

//inits entity system
void entity_system_init();
//destroys entity system
void entity_system_destroy();
//adds entity object to entity list
void entity_add(tEntity entity);
//creates a new entity. Returns entity number
int16_t entity_create(tVector initPos, BITMAP *img, uint8_t entType, uint16_t properties, void (*entity_init)(tEntity *entity), void (*entity_create)(tEntity *entity), void (*entity_update)(tEntity *entity));
//returns entity based on entity number
tEntity* get_entity(uint16_t numEntity);
//calls the init function on all entitites
void entities_init();
//calls the update function on all entities
void entities_update(tScroll *scroll);
//draws all entities on passed buffer
void entities_draw(BITMAP *buffer, tScroll *scroll);
void entity_destroy_all();

#endif
