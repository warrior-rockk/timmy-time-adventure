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
    E_SCORPION_ENEMY_TYPE,
    E_INDIAN_AXE_ENEMY_TYPE,
    E_AXE_ENEMY_TYPE,
    E_BAT_ENEMY_TYPE,
    E_INDIAN_BOW_ENEMY_TYPE,
    E_ARROW_ENEMY_TYPE,
    E_EGYPTIAN_ENEMY_TYPE,
    E_TRAP_ARROW_ENEMY_TYPE,
    E_BEETLE_ENEMY_TYPE,
    E_MUMMY_ENEMY_TYPE,
    E_HITBOX_ENEMY_TYPE,
    E_VULTURE_ENEMY_TYPE,
    E_ANUBIS_ENEMY_TYPE,
    E_TRAP_FIRE_ENEMY_TYPE,
    E_GHOST_ENEMY_TYPE,
    E_KNIGHT_ENEMY_TYPE,

    E_ENEMIES_TYPE_NUM,
};

enum E_ENEMY_SFX
{
    E_SFX_ENEMY_DEAD,    
    E_SFX_ENEMY_SHOOT,
    E_SFX_ENEMY_AXE,
    E_SFX_ENEMY_ARROW,
    E_SFX_ENEMY_WIP,
    E_SFX_ENEMY_SPLASH,

    E_SFX_ENEMY_NUM,
};

//types of local data structures
enum E_ENEMY_LOCAL_DATA_TYPE
{
    E_ENEMY_DEFAULT_LOCAL_DATA_TYPE,
    E_ENEMY_AXE_LOCAL_DATA_TYPE,
};

//default enemy local data struct
typedef struct 
{
    uint16_t flag;
    uint16_t timer;
} tDefaultEnemyLocalData;

//axe enemy local data struct
typedef struct {
    fixed cx;       //center axis x position
    fixed cy;       //center axis y position    
    int angle;      //angle of axe rotation       
} tAxeLocalData;

//local metadata structure
typedef struct {
    uint8_t structureType;  //stores the type of the structure (need for realloc)
    void *data;             //pointer to the local data structure
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
void enemy_hitbox_update(tEntity *this, tDefaultEnemyLocalData *local);

void enemy_ptero_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_raptor_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_spider_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_piranha_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_trice_update(tEntity *this, tDefaultEnemyLocalData *local);

void enemy_cowboy_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_eagle_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_bullet_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_tumble_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_scorpion_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_indian_axe_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_indian_bow_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_axe_update(tEntity *this, tAxeLocalData *local);
void enemy_arrow_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_bat_update(tEntity *this, tDefaultEnemyLocalData *local);

void enemy_egyptian_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_beetle_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_mummy_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_vulture_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_anubis_update(tEntity *this, tDefaultEnemyLocalData *local);

void enemy_ghost_update(tEntity *this, tDefaultEnemyLocalData *local);
void enemy_knight_update(tEntity *this, tDefaultEnemyLocalData *local);
#endif