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

//player dimensions
#define PLAYER_SIZE_W               12      //player collision size width
#define PLAYER_SIZE_H               32      //player collision size height
#define PLAYER_SIZE_H_CROUCH        16      //player collision size height when crouched
#define PLAYER_IMG_W                41      //player sprite width
#define PLAYER_IMG_H                44      //player sprite height
#define PLAYER_CROUCH_CHECK_Y       16      //player y offset to check above tile when crouched

//player velocities    
    //horizontal
#define PLAYER_MAX_VEL_X            1.3     //max velocity x of player
#define PLAYER_MIN_VEL_X_TO_RESET   0.01    //min x velocity for reset to 0 
#define PLAYER_ACCEL_X              0.30    //0.06 without friction on move
#define PLAYER_ACCEL_X_AIR          0.36    //more acceleration on air
#define PLAYER_FRICTION             0.88    //more friction, more sloppy (0.94-0.96 is like ice) 0.92 was too much
#define PLAYER_AIR_FRICTION         0.98    //more than floor friction
#define PLAYER_HURT_VEL_X           1.4     //velocity x when gets hurt
    //vertical
#define PLAYER_ACCEL_Y              0.7     //jump acceleration
#define PLAYER_JUMP_VEL_Y           3.5     //max jump velocity
#define PLAYER_HURT_VEL_Y           -3.6    //velocity y when gets hurt
#define PLAYER_ATTACK_BOUNCE_VEL    -4      //velocity when bounces by attacking 
#define PLAYER_STAIRS_VEL_Y         0.6     //velocity on stairs

//player times
#define PLAYER_INVINCIBLE_TIME      200     //clock times
#define PLAYER_PICKING_TIME         10      //clock times
#define PLAYER_IDLE_WAIT_TIME       5       //seconds

//player animations
#define ANIM_PLY_IDLE                0,  0, 10, ANIM_ONCE
#define ANIM_PLY_BREATH             24, 27, 30, ANIM_PING_PONG
#define ANIM_PLY_RUN                 1,  6, 10, ANIM_LOOP
#define ANIM_PLY_JUMP_UP            10, 11, 10, ANIM_ONCE
#define ANIM_PLY_JUMP_DOWN          12, 13, 10, ANIM_ONCE
#define ANIM_PLY_JUMP_RUN_UP        17, 18, 10, ANIM_ONCE
#define ANIM_PLY_JUMP_RUN_DOWN      19, 21, 30, ANIM_ONCE
#define ANIM_PLY_GET_CROUCH         28, 31, 2,  ANIM_ONCE
#define ANIM_PLY_CROUCH             31, 31, 2,  ANIM_LOOP
#define ANIM_PLY_WALK_CROUCH        65, 68, 20, ANIM_LOOP
#define ANIM_PLY_ATTACK             37, 37, 10, ANIM_ONCE 
#define ANIM_PLY_LAND               93, 94, 10, ANIM_ONCE   //33, 33, 15, ANIM_ONCE  
#define ANIM_PLY_HURT               39, 41, 10, ANIM_ONCE
#define ANIM_PLY_FALL_DEAD          42, 48, 10, ANIM_ONCE
#define ANIM_PLY_PICKING            49, 49, 10, ANIM_LOOP
#define ANIM_PLY_PICKED             50, 50, 10, ANIM_ONCE
#define ANIM_PLY_THROW              51, 53, 10, ANIM_ONCE
#define ANIM_PLY_RUN_PICKED         54, 59, 10, ANIM_LOOP
#define ANIM_PLY_JUMP_UP_PICKED     60, 61, 10, ANIM_ONCE
#define ANIM_PLY_JUMP_DOWN_PICKED   62, 63, 10, ANIM_ONCE
#define ANIM_PLY_THROW_AIR          64, 64, 10, ANIM_ONCE
#define ANIM_PLY_ON_TOP_STAIRS      69, 69, 10, ANIM_LOOP
#define ANIM_PLY_ON_STAIRS          70, 70, 10, ANIM_LOOP
#define ANIM_PLY_MOVE_STAIRS        70, 71, 20, ANIM_LOOP
#define ANIM_PLY_IDLE_WAIT          72, 83, 10, ANIM_ONCE
#define ANIM_PLY_JUMP_ATTACK        85, 88, 5, ANIM_ONCE
#define ANIM_PLY_ON_EDGE            95, 97, 10, ANIM_PING_PONG
#define ANIM_PLY_TIMEOUT            98, 105, 10, ANIM_ONCE

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
    ST_PLAYER_ON_STAIRS,
    ST_PLAYER_MOVING_ON_STAIRS,  
    ST_PLAYER_TIMEOUT,  
};

enum E_PLAYER_SFX
{
    SFX_PLAYER_JUMP,
    SFX_PLAYER_HURT,
    SFX_PLAYER_THROW,
    SFX_PLAYER_BOUNCE,
    SFX_PLAYER_PICK,
    SFX_PLAYER_DEAD,
    SFX_PLAYER_STAIR,
    SFX_PLAYER_NUM,
};

void player_create(tEntity *player);
void player_init(tEntity *player);
void player_update(tEntity *player);
void player_destroy(tEntity *player);
#endif