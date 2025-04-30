#ifndef _H_ENTITY
#define _H_ENTITY

#include <stdbool.h>
#include "allegro.h"
#include "globals.h"

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

#endif
