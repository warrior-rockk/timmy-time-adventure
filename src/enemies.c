/********************************************************************
* Enemies system
*
* 04/02/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include "allegro.h"
#include "utils.h"
#include "game.h"
#include "enemies.h"
#include "timer.h"

uint16_t numEnemyInstances;        //num of enemy instances
static void *enemyDataList;        //list of enemy local data

void enemy_system_init()
{
    //empty enemy list
    free(enemyDataList);
    //set number of entities
    numEnemyInstances = 0;    
}

void enemy_system_destroy()
{
    //empty enemy list
    free(enemyDataList);
    //set number of entities
    numEnemyInstances = 0;    
}

//check enemy entity type to add the local data structure to local data list and increases instances number
void enemy_create(tEntity *entity)
{
    //inc num instances
    numEnemyInstances++;

    //alloc memory for specified enemy type local data
    switch (entity->entType)
    {
        case E_PTERO_ENEMY_TYPE:
            //allocate memory for enemy
            enemyDataList = realloc(enemyDataList, numEnemyInstances * sizeof(tPteroLocalData));
            entity->img = load_bmp("res/enemies/ptero.bmp",NULL); 
            entity->spriteSize = (tVector){52, 48};
            entity->size.x = 50;
            entity->size.y = 20;                          
        break;
        case E_RAPTOR_ENEMY_TYPE:
            //allocate memory for enemy
            enemyDataList = realloc(enemyDataList, numEnemyInstances * sizeof(tEnemyLocalData));
            entity->img = load_bmp("res/enemies/raptor.bmp",NULL); 
            entity->spriteSize = (tVector){47, 31};                          
            entity->size = entity->spriteSize;
        break;        
        default:
            abort_on_error("Tipo de entidad enemigo no reconocida");
        break;
    }

    //test memory allocation
    MY_ASSERT(enemyDataList);

    //set actual instance num
    entity->entInstance = numEnemyInstances - 1;                
};

//calls specified enemy type update function
void enemy_update(tEntity *entity)
{   
    switch (entity->entType)
    {
        case E_PTERO_ENEMY_TYPE:            
            enemy_ptero_update(entity, &((tPteroLocalData*)enemyDataList)[entity->entInstance]);
        break;        
        case E_RAPTOR_ENEMY_TYPE:            
            enemy_raptor_update(entity, &((tEnemyLocalData*)enemyDataList)[entity->entInstance]);
        break;        
        default:
        break;
    }
}

//calls specified enemy type init function
void enemy_init(tEntity *entity)
{   
    switch (entity->entType)
    {
        case E_PTERO_ENEMY_TYPE:            
            ((tPteroLocalData*)enemyDataList)[numEnemyInstances - 1].health = 0;
            ((tPteroLocalData*)enemyDataList)[numEnemyInstances - 1].timer = 0;            
            
        break;        
        case E_RAPTOR_ENEMY_TYPE:            
            ((tEnemyLocalData*)enemyDataList)[numEnemyInstances - 1].timer = 0;                              
        break;        
        default:
        break;
    }
}

void enemy_ptero_update(tEntity *this, tPteroLocalData *local)
{
    //enemy animations
    #define ANIM_PTERO_FLY     0,   1,  20, ANIM_LOOP

    //enemy states
    enum E_PTERO_ENEMY_STATE
    {
        E_PTERO_IDLE_STATE,
        E_PTERO_MOVE_RIGHT_STATE,
        E_PTERO_MOVE_LEFT_STATE
    };

    //check state
    switch (this->state)
    {
        case E_PTERO_IDLE_STATE:
                this->state = E_PTERO_MOVE_RIGHT_STATE;
                local->timer = 0;
        break;
        case E_PTERO_MOVE_RIGHT_STATE:
            if (this->pos.x > this->initPos.x + 20)
                this->state = E_PTERO_MOVE_LEFT_STATE;
            else   
                this->fixVel.x = ftofix(0.4);
            
            play_animation(&this->anim, ANIM_PTERO_FLY);
            this->dir = E_ENT_DIR_RIGHT;
        break;
        case E_PTERO_MOVE_LEFT_STATE:
            if (this->pos.x < this->initPos.x - 20)
                this->state = E_PTERO_MOVE_RIGHT_STATE;
            else   
                this->fixVel.x = -ftofix(0.4);
            
            play_animation(&this->anim, ANIM_PTERO_FLY);
            this->dir = E_ENT_DIR_LEFT;
        break;
        default:
            this->state = E_PTERO_IDLE_STATE;
    }

    //apply velocity
    this->fixPos.x += fixmul(this->fixVel.x, ftofix(deltaTime));
    this->fixPos.y += fixmul(this->fixVel.y, ftofix(deltaTime));

    //update position
    this->pos.x = fixtoi(this->fixPos.x);
    this->pos.y = fixtoi(this->fixPos.y);

    local->health = this->pos.x;      
}

void enemy_raptor_update(tEntity *this, tEnemyLocalData *local)
{              
    //enemy animations
    #define ANIM_RAPTOR_WALK   0,   1,  20, ANIM_LOOP

    play_animation(&this->anim, ANIM_RAPTOR_WALK);
}

void enemy_trace(tEntity *this)
{
    MY_TRACE("enemy Instance: %d\n\tObj Type:%d\n", this->entInstance, this->entType);
}