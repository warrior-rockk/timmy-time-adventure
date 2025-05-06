#include "entity.h"

tEntity entityList[10];
uint16_t numEntities = 0;

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
    TRACE("Data: %d\n", entityList[numEntities-1].size.x);
}

void create_entity(tVector pos, BITMAP *img)
{
    entityList[numEntities].pos = pos;
    entityList[numEntities].fX = itofix(pos.x);
    entityList[numEntities].fY = itofix(pos.y);
    entityList[numEntities].vX = 0;
    entityList[numEntities].vY = 0;
    entityList[numEntities].img = img;
    entityList[numEntities].size.x = img->w;
    entityList[numEntities].size.y = img->h;

    numEntities++;
}

void entities_update()
{
    
}
void entities_draw(BITMAP *buffer, tScroll *scroll)
{
    for (int i=0; i < numEntities; i++)
    {
        entity_draw(buffer, &entityList[i], scroll);
    }    
}

