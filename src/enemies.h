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

#define ENEMY_DEFAULT_DEAD_TIME     20      //time to display enemy death animation

//types of enemies
enum E_ENEMY_TYPE
{
    E_PTERO_ENEMY_TYPE,
    E_RAPTOR_ENEMY_TYPE,
    E_SPIDER_ENEMY_TYPE,
    E_PIRANHA_ENEMY_TYPE,
    E_COWBOY_ENEMY_TYPE,
    E_EAGLE_ENEMY_TYPE,
    E_BULLET_ENEMY_TYPE,
    E_TUMBLE_ENEMY_TYPE,
    E_TRICE_ENEMY_TYPE,
    E_ENEMIES_TYPE_NUM,
};

enum E_ENEMY_SFX
{
    E_SFX_ENEMY_DEAD,    
    E_SFX_ENEMY_SHOOT,
    E_SFX_ENEMY_NUM,
};

//global enemy local data struct
typedef struct 
{
    uint16_t flag;
    uint16_t timer;
} tEnemyLocalData;

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
//destroys enemy instance of list
void enemy_destroy(tEntity *entity);

//custom update function for particular entity
void enemy_ptero_update(tEntity *this, tEnemyLocalData *local);
void enemy_raptor_update(tEntity *this, tEnemyLocalData *local);
void enemy_spider_update(tEntity *this, tEnemyLocalData *local);
void enemy_piranha_update(tEntity *this, tEnemyLocalData *local);
void enemy_trice_update(tEntity *this, tEnemyLocalData *local);

void enemy_cowboy_update(tEntity *this, tEnemyLocalData *local);
void enemy_eagle_update(tEntity *this, tEnemyLocalData *local);
void enemy_bullet_update(tEntity *this, tEnemyLocalData *local);
void enemy_tumble_update(tEntity *this, tEnemyLocalData *local);
#endif