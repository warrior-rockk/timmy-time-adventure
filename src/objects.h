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
};

enum E_GEM_STATE
{
    E_GEM_IDLE_STATE,
    E_GEM_MOVE_RIGHT_STATE,
    E_GEM_MOVE_LEFT_STATE
};

typedef struct tGemLocalData
{
    uint16_t health;
    uint16_t timer;
} tGemLocalData;

typedef struct tStoneLocalData
{
    uint8_t flags;
    bool solid;
} tStoneLocalData;

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

//custom update function for gems objects
void object_gem_update(tEntity *this, tGemLocalData *objData);
//custom update function for stone objects
void object_stone_update(tEntity *this, tStoneLocalData *objData);

#endif