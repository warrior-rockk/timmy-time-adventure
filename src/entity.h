#ifndef _H_ENTITY
#define _H_ENTITY
#include <stdbool.h>

#include "allegro.h"
#include "globals.h"
#include "scroll.h"

typedef struct tEntity
{
    tVector pos;
    tVector size;
    fixed fX;
    fixed fY;
    fixed vX;
    fixed vY;
    BITMAP *img;
    bool ground;
    bool jump;
    void (*entity_update)(struct tEntity *entity);
} tEntity;


void entity_add(tEntity entity);
void create_entity(tVector pos, BITMAP *img, void (*entity_update)(tEntity *entity));
tEntity* get_entity(uint16_t numEntity);
void entities_update();
void entities_draw(BITMAP *buffer, tScroll *scroll);


#endif
