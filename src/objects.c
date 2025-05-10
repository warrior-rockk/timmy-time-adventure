/********************************************************************
* Object entities (test)
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#include "allegro.h"
#include "globals.h"
#include "objects.h"

uint16_t numObjectInstances;
static void *objectDataList;

void object_system_init()
{
    //allocate memory for one entity
    objectDataList = (tGemObjectData*)malloc(sizeof(tGemObjectData));
    //test memory allocation
    MY_ASSERT(objectDataList);
    //set number of entities
    numObjectInstances = 0;
}

void object_create(tEntity *entity)
{
    switch (entity->entType)
    {
        case E_GEM_OBJECT_INSTANCE:
            entity->entInstance = numObjectInstances;                
            //add gem objects counter
            numObjectInstances++;
            //allocate memory for next gem Object
            objectDataList = realloc(objectDataList, (numObjectInstances + 1) * sizeof(tGemObjectData));
        break;
        case E_STONE_OBJECT_INSTANCE:
            entity->entInstance = numObjectInstances;                
            //add gem objects counter
            numObjectInstances++;
            //allocate memory for next gem Object
            objectDataList = realloc(objectDataList, (numObjectInstances + 1) * sizeof(tStoneObjectData));
        break;
        default:
        break;
    }
};

void object_update(tEntity *entity)
{
    switch (entity->entType)
    {
        case E_GEM_OBJECT_INSTANCE:
            object_gem_update(entity, &objectDataList[entity->entInstance]);
        break;
        case E_STONE_OBJECT_INSTANCE:
            object_stone_update(entity, &objectDataList[entity->entInstance]);
        break;
        default:
        break;
    }
}

void object_gem_update(tEntity *this, tGemObjectData *objData)
{
    switch (this->state)
    {
        case E_GEM_IDLE_STATE:
            this->state = E_GEM_MOVE_RIGHT_STATE;
        break;
        case E_GEM_MOVE_RIGHT_STATE:
            if (this->pos.x > 130)
                this->state = E_GEM_MOVE_LEFT_STATE;
            else   
                this->fixVel.x = ftofix(0.4);
        break;
        case E_GEM_MOVE_LEFT_STATE:
        if (this->pos.x < 70)
                this->state = E_GEM_MOVE_RIGHT_STATE;
            else   
                this->fixVel.x = ftofix(-0.4);
        break;
        default:
            this->state = E_GEM_IDLE_STATE;
    }

    //apply velocity
    this->fixPos.x += fixmul(this->fixVel.x, ftofix(deltaTime));
    this->fixPos.y += fixmul(this->fixVel.y, ftofix(deltaTime));

    //update position
    this->pos.x = fixtoi(this->fixPos.x);
    this->pos.y = fixtoi(this->fixPos.y);

    objData->health++;
    trace_object(this);
}

void object_stone_update(tEntity *this, tStoneObjectData *objData)
{
    trace_object(this);
}

void trace_object(tEntity *this)
{
    MY_TRACE("Object Instance: %d\n\tObj Type:%d\n", this->entInstance, this->entType);
}