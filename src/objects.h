/********************************************************************
* Objects entities (test)
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#ifndef _H_OBJECTS_
#define _H_OBJECTS_

#include "entity.h"

#define ANIM_OBJECT_BREAK                1,  2, 10, ANIM_ONCE

enum E_OBJECT_TYPE
{
    E_GEM_OBJECT_TYPE,
    E_STONE_OBJECT_TYPE,
    E_ROCK_OBJECT_TYPE,
    E_ROCK_2_OBJECT_TYPE,
    E_EGG_OBJECT_TYPE,
    E_END_OBJECT_TYPE,
    E_CHECKPOINT_OBJECT_TYPE,
    E_ITEM_OBJECT_TYPE,
    E_OBJECTS_TYPE_NUM,
};

enum E_ITEMS_TYPE
{
    E_ITEM_EXTRA_LIFE,
    E_ITEM_FULL_LIFE,
    E_ITEMS_NUM,
};

typedef struct 
{
    uint16_t timer;    
} tSolidObjectLocalData;

//debug trace entity object info
void object_trace(tEntity *this);

//initializes the object systen
void object_system_init();
//destroys the object system
void object_system_destroy();
//inits an entity passing his entity structure
void object_init(tEntity *entity);
//creates an entity passing his entity structure
void object_create(tEntity *entity);
//updates an entity passing his entity structure
void object_update(tEntity *entity);
//destroy object instance of list
void object_destroy(tEntity *entity);

//custom update function for gems objects
void object_gem_update(tEntity *this, tSolidObjectLocalData *objData);
//custom update function for stone objects
void object_solid_update(tEntity *this, tSolidObjectLocalData *objData);
void object_item_update(tEntity *this, tSolidObjectLocalData *objData);

void object_end_update(tEntity *this, tSolidObjectLocalData *objData);
void object_checkpoint_update(tEntity *this, tSolidObjectLocalData *objData);
#endif