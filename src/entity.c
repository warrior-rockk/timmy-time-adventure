#include "entity.h"

tEntity entityList[10];
uint16_t numEntities;

static void entity_init()
{

}

static void entity_update()
{

}

static void entity_draw(BITMAP *buffer, tEntity *entity, tScroll *scroll)
{
    if (entity->img)
        draw_sprite(buffer, entity->img, entity->pos.x - scroll->pos.x, entity->pos.y - scroll->pos.y);
}

//public functions
//================

void entity_add(tEntity entity)
{
    entityList[numEntities] = entity;
    numEntities++;
    TRACE("add entity\n");
    TRACE("Data: %d\n", entityList[numEntities-1].pos.x);
}

void entities_update()
{
    
}
void entities_draw(BITMAP *buffer, tScroll *scroll)
{
    
}

