/********************************************************************
* Objects entities (test)
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#ifndef _H_OBJECTS_
#define _H_OBJECTS_

#include "entity.h"

enum E_OBJECT_TYPE
{
    E_GEM_OBJECT_TYPE,
    E_STONE_OBJECT_TYPE,
};

enum E_GEM_STATE{
    E_GEM_IDLE_STATE,
    E_GEM_MOVE_RIGHT_STATE,
    E_GEM_MOVE_LEFT_STATE
};

typedef struct tGemObjectData
{
    uint16_t health;
} tGemObjectData;

typedef struct tStonebjectData
{
    uint8_t flags;
    bool solid;
} tStoneObjectData;

void object_trace(tEntity *this);
void object_system_init();
void object_create(tEntity *entity);
void object_update(tEntity *entity);
void object_gem_update(tEntity *this, tGemObjectData *objData);
void object_stone_update(tEntity *this, tStoneObjectData *objData);

#endif