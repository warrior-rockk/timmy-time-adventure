/********************************************************************
* Object entities (test)
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#include "allegro.h"
#include "objects.h"

uint16_t numGemObjects= 0;
static tGemObjectData *gemObjDataList;

void object_system_init()
{
    //allocate memory for one entity
    gemObjDataList = (tGemObjectData*)malloc(sizeof(tGemObjectData));
    //test memory allocation
    ASSERT(gemObjDataList);
    //set number of entities
    numGemObjects = 0;
}

void object_create(tEntity *entity)
{
    switch (entity->entType)
    {
        case E_GEM_OBJECT_INSTANCE:
            entity->entInstance = numGemObjects;                
            //add gem objects counter
            numGemObjects++;
            //allocate memory for next gem Object
            gemObjDataList = realloc(gemObjDataList, (numGemObjects + 1) * sizeof(tGemObjectData));
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
            object_gem_update(entity, &gemObjDataList[entity->entInstance]);
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
    TRACE("Object Instance: %d, Object Health: %d\n", this->entInstance, objData->health);
}