/********************************************************************
* Player system
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#include <stdbool.h>
#include <math.h>
#include "allegro.h"
#include "player.h"
#include "globals.h"
#include "entity.h"

#define PLAYER_MAX_FRAMES   50
#define PLAYER_W            28
#define PLAYER_H            43

BITMAP *playerSpriteSheet;
BITMAP *playerFrames[PLAYER_MAX_FRAMES];
tAnimation playerAnim;

void player_init(tEntity *player)
{
    //load player spriteSheet
    playerSpriteSheet = load_bmp("res/player/player.bmp", NULL);
    for (uint8_t i = 0; i < PLAYER_MAX_FRAMES; i++)
    {
        playerFrames[i] = create_sub_bitmap(playerSpriteSheet, i * PLAYER_W, 0, PLAYER_W, PLAYER_H);
    }

    player->state = ST_PLAYER_IDLE;
    player->size.x = PLAYER_W;
    player->size.y = PLAYER_H;
    player->dir = E_ENT_DIR_RIGHT;
}

void player_update(tEntity *player)
{
    fixed accel_x = ftofix(0.06);
    fixed friction = ftofix(0.86);       //more friction, more sloppy (0.94-0.96 is like ice)
    fixed air_friction = ftofix(0.6);   //less than floor friction
    fixed gravity = ftofix(0.2);
    fixed accel_y = ftofix(4.0);        //jump acceleration
    fixed max_vel_x = ftofix(1.0);
    fixed max_vel_y = ftofix(6);
    
    const fixed cMinVelToIdle = ftofix(0.1);
    uint8_t playerCrouched = false;


    int16_t floor = 160;
    
    fixed localFriction = player->ground ? friction: air_friction;

    //update controls
    if (key[KEY_RIGHT] && player->fixVel.x < max_vel_x)
    {
        //player->vX+= fixmul(accel_x, (itofix(1) - friction));
        player->fixVel.x+= fixmul(accel_x, ftofix(deltaTime));
        player->dir = E_ENT_DIR_RIGHT;
    }

    if (key[KEY_LEFT] && player->fixVel.x > -max_vel_x)
    {
        player->fixVel.x-= fixmul(accel_x, ftofix(deltaTime));
        player->dir = E_ENT_DIR_LEFT;
    }

    if (key[KEY_Z] && player->ground)
    {
        player->fixVel.y = -accel_y;
        player->jump = true;
        player->ground = false;
    }

    if (key[KEY_DOWN])
    {
        playerCrouched = true;
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

    //update state
    player->prevState = player->state;    
    if (!player->ground == true)
    {
        player->state = ST_PLAYER_JUMP;
    }
    else if (abs(player->fixVel.x) > cMinVelToIdle)
    {
        player->state = ST_PLAYER_RUN;
    }
    else if (playerCrouched)
        player->state = ST_PLAYER_CROUCHED;
    else
    {
        player->state = ST_PLAYER_IDLE;
    }

    //update frame
    switch (player->state)
    {
        case ST_PLAYER_IDLE:
            play_animation(&playerAnim, ANIM_PLY_BREATH);            
        break;
        case ST_PLAYER_RUN:
            play_animation(&playerAnim, ANIM_PLY_RUN);
        break;
        case ST_PLAYER_JUMP:
            if (player->fixVel.y < 0)
                if (abs(player->fixVel.x) > cMinVelToIdle)
                    play_animation(&playerAnim, ANIM_PLY_JUMP_RUN_UP);
                else
                    play_animation(&playerAnim, ANIM_PLY_JUMP_UP);
            else
                if (abs(player->fixVel.x) > cMinVelToIdle)
                    play_animation(&playerAnim, ANIM_PLY_JUMP_RUN_DOWN);
                else
                    play_animation(&playerAnim, ANIM_PLY_JUMP_DOWN);
        break;
        case ST_PLAYER_CROUCHED:
            play_animation(&playerAnim, ANIM_PLY_CROUCH);
        break;
    }
    player->img = playerFrames[playerAnim.frame];    
}