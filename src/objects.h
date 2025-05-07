/********************************************************************
* Objects entities (test)
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#ifndef _H_OBJECTS_
#define _H_OBJECTS_

#include "entity.h"

enum E_GEM_STATE{
    GEM_IDLE_STATE,
    GEM_MOVE_RIGHT_STATE,
    GEM_MOVE_LEFT_STATE
};

void object_gem_update(tEntity *this);

#endif