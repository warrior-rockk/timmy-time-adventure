/********************************************************************
* Enemies definitions
*
* 04/02/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#ifndef _H_ENEMIES_
#define _H_ENEMIES_

#include "entity.h"
#include "anim.h"

//enemy animations
#define ANIM_PTERO_FLY     0,   1,     20, ANIM_LOOP

enum E_ENEMY_TYPE
{
    E_PTERO_ENEMY_TYPE,   zz 
};

enum E_PTERO_ENEMY_STATE
{
    E_PTERO_IDLE_STATE,
    E_PTERO_MOVE_RIGHT_STATE,
    E_PTERO_MOVE_LEFT_STATE
};

typedef struct tPteroLocalData
{
    uint16_t health;
    uint16_t timer;    
} tPteroLocalData;


//debug trace entity enemy info
void enemy_trace(tEntity *this);

//initializes the enemy systen
void enemy_system_init();
//destroys the enemy system
void enemy_system_destroy();
//inits an entity passing his entity structure
void enemy_init(tEntity *entity);
//creates an entity passing his entity structure
void enemy_create(tEntity *entity);
//updates an entity passing his entity structure
void enemy_update(tEntity *entity);

//custom update function for each enemy
void enemy_ptero_update(tEntity *this, tPteroLocalData *objData);

#endif