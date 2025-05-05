#ifndef _H_ENTITY
#define _H_ENTITY
#include <stdbool.h>

#include "allegro.h"
#include "globals.h"

#define PLAYER_ENTITY_ID    0

typedef struct tEntity
{
    tVector pos;
    tVector size;
    fixed fX;
    fixed fY;
    fixed vX;
    fixed vY;
    bool ground;
    bool jump;
} tEntity;


void entity_add(tEntity entity);
void entities_update();


#endif
