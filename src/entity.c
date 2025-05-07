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

static void entity_init()
{

}

static void entity_draw(BITMAP *buffer, tEntity *entity, tScroll *scroll)
{
    if (entity->img)
        draw_sprite(buffer, entity->img, entity->pos.x - scroll->pos.x, entity->pos.y - scroll->pos.y);
}

//public functions
//================

void entity_system_init()
{
    //allocate memory for one entity
    entityList = (tEntity*)malloc(sizeof(tEntity));
    //test memory allocation
    ASSERT(entityList);
    //set number of entities
    numEntities = 0;
}

void entity_system_destroy()
{
    free(entityList);
    numEntities = 0;
}

void entity_add(tEntity entity)
{
    entityList[numEntities] = entity;
    numEntities++;
    TRACE("add entity\n");
    TRACE("Data: %d\n", entityList[numEntities-1].size.x);
}

int16_t create_entity(tVector pos, BITMAP *img, void (*entity_update)(tEntity *entity))
{
    //assign entity data
    entityList[numEntities].pos = pos;
    entityList[numEntities].fixPos.x = itofix(pos.x);
    entityList[numEntities].fixPos.y = itofix(pos.y);
    entityList[numEntities].fixVel.x = 0;
    entityList[numEntities].fixVel.y = 0;
    entityList[numEntities].img = img;
    entityList[numEntities].size.x = img->w;
    entityList[numEntities].size.y = img->h;

    entityList[numEntities].entity_update = entity_update;

    //add entity counter
    numEntities++;
    
    //allocate memory for next entity
    entityList = realloc(entityList, (numEntities + 1) * sizeof(tEntity));

    return numEntities-1;
}

tEntity* get_entity(uint16_t numEntity)
{
    return &entityList[numEntity];
}

void entities_update()
{
    for (int i=0; i < numEntities; i++)
    {
        if (entityList[i].entity_update)
            entityList[i].entity_update(&entityList[i]);
    }   
}
void entities_draw(BITMAP *buffer, tScroll *scroll)
{
    for (int i=0; i < numEntities; i++)
    {
        entity_draw(buffer, &entityList[i], scroll);
    }    
}
