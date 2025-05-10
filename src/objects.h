/********************************************************************
* Objects entities (test)
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#ifndef _H_OBJECTS_
#define _H_OBJECTS_

#include "entity.h"

enum E_OBJECT_INSTANCES
{
    E_GEM_OBJECT_INSTANCE,
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

void object_system_init();
void object_create(tEntity *entity);
void object_update(tEntity *entity);
void object_gem_update(tEntity *this, tGemObjectData *objData);

#endif