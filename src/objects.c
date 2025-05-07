#include "allegro.h"
#include "objects.h"

void object_gem_update(tEntity *entity)
{
    TRACE("Aqui estoy!\n");
    entity->pos.x++;
}