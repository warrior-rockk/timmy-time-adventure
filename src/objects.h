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

//object types
enum E_OBJECT_TYPE
{
    E_ROCK_OBJECT_TYPE,
    E_ROCK_2_OBJECT_TYPE,
    E_EGG_OBJECT_TYPE,
    E_END_OBJECT_TYPE,
    E_CHECKPOINT_OBJECT_TYPE,
    E_ITEM_OBJECT_TYPE,
    E_STOP_SCROLL_OBJECT_TYPE,
    E_OBJECTS_TYPE_NUM,
};

//item type uses spare to set type
enum E_ITEMS_TYPE
{
    E_ITEM_EXTRA_LIFE,
    E_ITEM_FULL_LIFE,
    E_ITEMS_NUM,
};

//stop scroll options
enum E_STOP_SCROLL_OPTIONS
{
    E_STOP_SCROLL_LEFT      = 0x01,
    E_STOP_SCROLL_RIGHT     = 0x02,
    E_STOP_SCROLL_DOWN      = 0x04,
    E_STOP_SCROLL_UP        = 0x08, 
};

typedef struct 
{
    uint16_t timer;  
    int16_t flag;  
} tSolidObjectLocalData;

enum E_OBJECT_SFX
{
    E_SFX_OBJECT_FULL_LIFE,
    E_SFX_OBJECT_EXTRA_LIVE,        
    E_SFX_OBJECT_NUM,
};

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

//custom update function for objects
void object_solid_update(tEntity *this, tSolidObjectLocalData *objData);
void object_item_update(tEntity *this, tSolidObjectLocalData *objData);
void object_trigger_update(tEntity *this, tSolidObjectLocalData *objData);
void object_end_update(tEntity *this, tSolidObjectLocalData *objData);
void object_checkpoint_update(tEntity *this, tSolidObjectLocalData *objData);
#endif