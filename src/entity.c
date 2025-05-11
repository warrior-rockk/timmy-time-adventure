/********************************************************************
* Entity system
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#include <stdlib.h>
#include "entity.h"

//static tEntity entityList[10];
static tEntity *entityList;
static uint16_t numEntities;

static void entity_draw(BITMAP *buffer, tEntity *entity, tScroll *scroll)
{
    if (entity->img)
        draw_sprite(buffer, entity->img, entity->pos.x - scroll->pos.x, entity->pos.y - scroll->pos.y);
}

//public functions
//================

void entity_system_init()
{
    //free entity memory allocation
    free(entityList);
    //clear num entities
    numEntities = 0;
}

void entity_system_destroy()
{
    //free entity memory allocation
    free(entityList);
    //clear num entities
    numEntities = 0;
}

int16_t entity_create(tVector initPos, BITMAP *img, uint8_t entType, void (*entity_init)(tEntity *entity), void (*entity_create)(tEntity *entity), void (*entity_update)(tEntity *entity))
{
    //inc num of entities
    numEntities++;

    if (numEntities <= ENTITY_MAX_NUM)
    {
        //allocate memory for entity
        entityList = realloc(entityList, numEntities * sizeof(tEntity));
        
        //test memory allocation
        ASSERT(entityList);

        //assign entity data
        uint16_t newEntity = numEntities - 1;

        entityList[newEntity].initPos         = initPos;
        entityList[newEntity].pos             = initPos;
        entityList[newEntity].fixPos.x        = itofix(initPos.x);
        entityList[newEntity].fixPos.y        = itofix(initPos.y);
        entityList[newEntity].fixVel.x        = 0;
        entityList[newEntity].fixVel.y        = 0;
        entityList[newEntity].img             = img;
        entityList[newEntity].size.x          = img->w;
        entityList[newEntity].size.y          = img->h;
        entityList[newEntity].state           = 0;
        entityList[newEntity].prevState       = 0;
        entityList[newEntity].entType         = entType;
        entityList[newEntity].entInstance     = 0;
        entityList[newEntity].dead            = false;
        entityList[newEntity].visible         = true;
        entityList[newEntity].entity_create   = entity_create;
        entityList[newEntity].entity_init     = entity_init;
        entityList[newEntity].entity_update   = entity_update;

        //call create function pointer of entity
        if (entityList[newEntity].entity_create) 
            entityList[newEntity].entity_create(&entityList[newEntity]);
        
        return newEntity;
    }
    else
        abort_on_error("ERROR: Reached max. number of entities\n");
        return -1;
}

void entity_add(tEntity entity)
{
    entityList[numEntities] = entity;
    numEntities++;
    TRACE("add entity\n");
    TRACE("Data: %d\n", entityList[numEntities-1].size.x);
}

tEntity* get_entity(uint16_t numEntity)
{
    return &entityList[numEntity];
}

//destroys entity index number
void entity_destroy(uint16_t entityIndex)
{
    //copies last entity to deleted entity position
    entityList[entityIndex] = entityList[numEntities - 1];
    //decrement entity number
    numEntities--;
    //reallocates the array with decremented entity number    
    entityList = realloc(entityList, numEntities * sizeof(tEntity));
}

void entities_init()
{
    for (int i=0; i < numEntities; i++)
    {
        entityList[i].pos       = entityList[i].initPos;
        entityList[i].fixPos.x  = itofix(entityList[i].pos.x);
        entityList[i].fixPos.y  = itofix(entityList[i].pos.y);
        entityList[i].fixVel    = (tFixVector){0, 0};
        entityList[i].state     = 0;
        entityList[i].visible   = true;

        if (entityList[i].entity_init)
        {
            entityList[i].entity_init(&entityList[i]);          
        }
    }   
}

void entities_update()
{
    for (int i=0; i < numEntities; i++)
    {
        if (entityList[i].dead)
            entity_destroy(i);
        else if (entityList[i].visible)
        {
            if (entityList[i].entity_update)
                entityList[i].entity_update(&entityList[i]);          
        }
    }   

    show_debug("Num entities: %d", numEntities);
}

void entities_draw(BITMAP *buffer, tScroll *scroll)
{
    for (int i=numEntities - 1; i >= 0; i--)
    {
        if (entityList[i].visible)
            entity_draw(buffer, &entityList[i], scroll);
    }    
}
