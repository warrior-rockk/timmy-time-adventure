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
#define PLAYER_ACCEL_X              0.20    //0.06 without friction on move
#define PLAYER_ACCEL_X_AIR          0.40    //more acceleration on air
#define PLAYER_FRICTION             0.92    //more friction, more sloppy (0.94-0.96 is like ice) 0.86 testing
#define PLAYER_AIR_FRICTION         0.6     //less than floor friction
#define PLAYER_GRAVITY              0.2     
#define PLAYER_ACCEL_Y              4.0     //jump acceleration
#define PLAYER_MAX_VEL_X            1.0
#define PLAYER_MAX_VEL_Y            6
#define PLAYER_MIN_VEL_TO_IDLE      0.1     
#define PLAYER_ENEMY_BOUNCE_VEL     -4
#define PLAYER_HURT_VEL_Y           -3.6
#define PLAYER_HURT_VEL_X           1.4

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

void player_init();
void player_update();
#endif