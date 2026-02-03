/********************************************************************
* Player system
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#ifndef _H_PLAYER_
#define _H_PLAYER_

#include "anim.h"
#include "entity.h"

#define PLAYER_W            12
#define PLAYER_H            30 //40
#define PLAYER_IMG_W        28
#define PLAYER_IMG_H        41

//player animations
#define ANIM_PLY_IDLE                0,   0,     10, ANIM_ONCE
#define ANIM_PLY_BREATH             24,  27,     30, ANIM_PING_PONG
#define ANIM_PLY_RUN                 1,   6,     10, ANIM_LOOP
#define ANIM_PLY_JUMP_UP            10,  11,     10, ANIM_ONCE
#define ANIM_PLY_JUMP_DOWN          12,  13,     10, ANIM_ONCE
#define ANIM_PLY_JUMP_RUN_UP        17,  18,     10, ANIM_ONCE
#define ANIM_PLY_JUMP_RUN_DOWN      19,  21,     30, ANIM_ONCE
#define ANIM_PLY_CROUCH              8,   9,     5, ANIM_ONCE

enum ePlayerState
{
    ST_PLAYER_IDLE,
    ST_PLAYER_RUN,
    ST_PLAYER_JUMP,
    ST_PLAYER_CROUCHED,
};

static void player_update_controls(tEntity *player);
static void player_update_collisions(tEntity *player);
static void player_update_state(tEntity *player);
static void player_update_animations(tEntity *player);

void player_init();
void player_update();
#endif