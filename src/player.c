/********************************************************************
* Player system
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#include <stdbool.h>
#include <math.h>
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
    if (key[KEY_RIGHT] && player->fixVel.x < max_vel_x)
        //player->vX+= fixmul(accel_x, (itofix(1) - friction));
        player->fixVel.x+= fixmul(accel_x, ftofix(deltaTime));

    if (key[KEY_LEFT] && player->fixVel.x > -max_vel_x)
        player->fixVel.x-= fixmul(accel_x, ftofix(deltaTime));

    if (key[KEY_Z] && player->ground)
    {
        player->fixVel.y = -accel_y;
        player->jump = true;
        player->ground = false;
    }

    //update vels
    if (!key[KEY_RIGHT] && !key[KEY_LEFT])
        player->fixVel.x = fixmul(player->fixVel.x, ftofix(pow(fixtof(friction), (deltaTime * fixtof(friction))))); //this the equivalent formula for vX *= friction with deltaTime
    
    if (player->pos.y >= floor && !player->jump)
    { 
        player->fixVel.y = 0;
        player->fixPos.y = itofix(floor);
        player->ground = true;
    }
    else
    {
        player->fixVel.y += player->fixVel.y >= max_vel_y ? 0 : fixmul(gravity, ftofix(deltaTime));
        //player->vY += fixmul(gravity, ftofix(deltaTime));
        player->jump = false;
    }   
    
    //apply velocity
    player->fixPos.x += fixmul(player->fixVel.x, ftofix(deltaTime));
    player->fixPos.y += fixmul(player->fixVel.y, ftofix(deltaTime));

    //update position
    player->pos.x = fixtoi(player->fixPos.x);
    player->pos.y = fixtoi(player->fixPos.y);

    //check collisions
}