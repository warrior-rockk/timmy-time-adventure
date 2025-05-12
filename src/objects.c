/********************************************************************
* Object entities (test)
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#include "allegro.h"
#include "globals.h"
#include "objects.h"

uint16_t numObjectInstances;        //num of object instances
static void *objectDataList;        //list of object local data

void object_system_init()
{
    //empty object list
    free(objectDataList);
    //set number of entities
    numObjectInstances = 0;    
}

void object_system_destroy()
{
    //empty object list
    free(objectDataList);
    //set number of entities
    numObjectInstances = 0;    
}

//check object entity type to add the local data structure to local data list and increases instances number
void object_create(tEntity *entity)
{
    //inc num instances
    numObjectInstances++;

    //alloc memory for specified object type local data
    switch (entity->entType)
    {
        case E_GEM_OBJECT_TYPE:
            //allocate memory for gem Object
            objectDataList = realloc(objectDataList, numObjectInstances * sizeof(tGemLocalData));
        break;
        case E_STONE_OBJECT_TYPE:
            //allocate memory for next stone Object
            objectDataList = realloc(objectDataList, numObjectInstances * sizeof(tStoneLocalData));
        break;
        default:
        break;
    }

    //test memory allocation
    MY_ASSERT(objectDataList);

    //set actual instance num
    entity->entInstance = numObjectInstances - 1;                
};

//calls specified object type update function
void object_update(tEntity *entity)
{   
    switch (entity->entType)
    {
        case E_GEM_OBJECT_TYPE:            
            object_gem_update(entity, &((tGemLocalData*)objectDataList)[entity->entInstance]);
        break;
        case E_STONE_OBJECT_TYPE:
            object_stone_update(entity, &((tStoneLocalData*)objectDataList)[entity->entInstance]);
        break;
        default:
        break;
    }
}

//calls specified object type init function
void object_init(tEntity *entity)
{   
    switch (entity->entType)
    {
        case E_GEM_OBJECT_TYPE:            
            ((tGemLocalData*)objectDataList)[numObjectInstances - 1].health = 0;
            ((tGemLocalData*)objectDataList)[numObjectInstances - 1].timer = 0;
        break;
        case E_STONE_OBJECT_TYPE:
            ((tStoneLocalData*)objectDataList)[numObjectInstances - 1].solid = false;
        break;
        default:
        break;
    }
}

void object_gem_update(tEntity *this, tGemLocalData *local)
{
    switch (this->state)
    {
        case E_GEM_IDLE_STATE:
            if (local->timer >= this->pos.y)
            {
                this->state = E_GEM_MOVE_RIGHT_STATE;
                local->timer = 0;
            }
            else
                local->timer++;
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

    local->health = this->pos.x;
    object_trace(this);
    TRACE("\tHealth:%d\n", local->health);
}

void object_stone_update(tEntity *this, tStoneLocalData *local)
{
    object_trace(this);
    local->solid = true;
    TRACE("\tSolid:%d\n", local->solid);
}

void object_trace(tEntity *this)
{
    MY_TRACE("Object Instance: %d\n\tObj Type:%d\n", this->entInstance, this->entType);
}