/********************************************************************
* Player system
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#include <stdbool.h>
#include "allegro.h"
#include "player.h"
#include "globals.h"
#include "entity.h"

void player_update(tEntity *player)
{
    fixed accel_x = ftofix(0.06);
    fixed friction = ftofix(0.86);       //more friction, more sloppy (0.94-0.96 is like ice)
    fixed air_friction = ftofix(0.6);   //less than floor friction
    fixed gravity = ftofix(0.2);
    fixed accel_y = ftofix(4.0);        //jump acceleration
    fixed max_vel_x = ftofix(1.0);
    fixed max_vel_y = ftofix(6);

    int16_t floor = 160;
    
    fixed localFriction = player->ground ? friction: air_friction;

    //update controls
    if (key[KEY_RIGHT] && player->vX < max_vel_x)
        //player->vX+= fixmul(accel_x, (itofix(1) - friction));
        player->vX+= fixmul(accel_x, ftofix(deltaTime));

    if (key[KEY_LEFT] && player->vX > -max_vel_x)
        player->vX-= fixmul(accel_x, ftofix(deltaTime));

    if (key[KEY_Z] && player->ground)
    {
        player->vY = -accel_y;
        player->jump = true;
        player->ground = false;
    }

    //update vels
    if (!key[KEY_RIGHT] && !key[KEY_LEFT])
        player->vX = fixmul(player->vX, ftofix(pow(fixtof(friction), (deltaTime * fixtof(friction))))); //this the equivalent formula for vX *= friction with deltaTime
    
    if (player->pos.y >= floor && !player->jump)
    { 
        player->vY = 0;
        player->fY = itofix(floor);
        player->ground = true;
    }
    else
    {
        player->vY += player->vY >= max_vel_y ? 0 : fixmul(gravity, ftofix(deltaTime));
        //player->vY += fixmul(gravity, ftofix(deltaTime));
        player->jump = false;
    }   
    
    //apply velocity
    player->fX += fixmul(player->vX, ftofix(deltaTime));
    player->fY += fixmul(player->vY, ftofix(deltaTime));

    //update position
    player->pos.x = fixtoi(player->fX);
    player->pos.y = fixtoi(player->fY);

    //check collisions
}