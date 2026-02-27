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
#define PLAYER_IMG_W        41
#define PLAYER_IMG_H        41

//player velocities    
    //horizontal
#define PLAYER_MAX_VEL_X            1.3     //max velocity x of player
#define PLAYER_MIN_VEL_X_TO_RESET   0.01    //min x velocity for reset to 0 
#define PLAYER_ACCEL_X              0.30    //0.06 without friction on move
#define PLAYER_ACCEL_X_AIR          0.36    //more acceleration on air
#define PLAYER_FRICTION             0.92    //more friction, more sloppy (0.94-0.96 is like ice) 0.86 testing
#define PLAYER_AIR_FRICTION         0.98    //more than floor friction
#define PLAYER_HURT_VEL_X           1.4     //velocity x when gets hurt
    //vertical
#define PLAYER_ACCEL_Y              0.7     //jump acceleration
#define PLAYER_JUMP_VEL_Y           3.4     //max jump velocity
#define PLAYER_HURT_VEL_Y           -3.6    //velocity y when gets hurt
#define PLAYER_ATTACK_BOUNCE_VEL    -4      //velocity when bounces by attacking 

//player times
#define PLAYER_INVINCIBLE_TIME      200
#define PLAYER_PICKING_TIME         30

//player animations
#define ANIM_PLY_IDLE                0,  0, 10, ANIM_ONCE
#define ANIM_PLY_BREATH             24, 27, 30, ANIM_PING_PONG
#define ANIM_PLY_RUN                 1,  6, 10, ANIM_LOOP
#define ANIM_PLY_JUMP_UP            10, 11, 10, ANIM_ONCE
#define ANIM_PLY_JUMP_DOWN          12, 13, 10, ANIM_ONCE
#define ANIM_PLY_JUMP_RUN_UP        17, 18, 10, ANIM_ONCE
#define ANIM_PLY_JUMP_RUN_DOWN      19, 21, 30, ANIM_ONCE
#define ANIM_PLY_CROUCH             28, 31, 2,  ANIM_ONCE
#define ANIM_PLY_ATTACK             37, 37, 10, ANIM_ONCE 
#define ANIM_PLY_LAND               33, 33, 15, ANIM_ONCE  
#define ANIM_PLY_HURT               39, 41, 10, ANIM_ONCE
#define ANIM_PLY_FALL_DEAD          42, 48, 10, ANIM_ONCE
#define ANIM_PLY_PICKING            49, 49, 10, ANIM_LOOP
#define ANIM_PLY_PICKED             50, 50, 10, ANIM_ONCE
#define ANIM_PLY_THROW              51, 53, 10, ANIM_ONCE
#define ANIM_PLY_RUN_PICKED         54, 59, 10, ANIM_LOOP
#define ANIM_PLY_JUMP_UP_PICKED     60, 61, 10, ANIM_ONCE
#define ANIM_PLY_JUMP_DOWN_PICKED   62, 63, 10, ANIM_ONCE
#define ANIM_PLY_THROW_AIR          64, 64, 10, ANIM_ONCE

enum ePlayerState
{
    ST_PLAYER_IDLE,
    ST_PLAYER_RUN,
    ST_PLAYER_JUMP,
    ST_PLAYER_CROUCHED,
    ST_PLAYER_ATTACK,
    ST_PLAYER_LAND,
    ST_PLAYER_HURT,
    ST_PLAYER_DEAD,
    ST_PLAYER_PICKING,
    ST_PLAYER_PICKED,
    ST_PLAYER_THROWING,
};

enum E_PLAYER_SFX
{
    SFX_PLAYER_JUMP,
    SFX_PLAYER_HURT,
    SFX_PLAYER_THROW,
    SFX_PLAYER_NUM,
};

void player_create();
void player_init();
void player_update();
void player_destroy();
#endif