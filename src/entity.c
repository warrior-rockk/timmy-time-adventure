#include "entity.h"

tEntity entityList[10];
uint16_t numEntities;

void entities_update()
{
    
}

void entity_add(tEntity entity)
{
    entityList[numEntities] = entity;
    numEntities++;
    TRACE("add entity\n");
    TRACE("Data: %d\n", entityList[numEntities-1].pos.x);
}

static void entity_init()
{

}

static void entity_update()
{

}

static void entity_draw()
{

}
