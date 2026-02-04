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
            entity->img = load_bmp("res/enemy/ptero.bmp",NULL);            
        break;        
        default:
            abort_on_error("Tipo de entidad enemigo no reconocida");
        break;
    }
    //set entity size
    entity->size = (tVector){entity->img->w, entity->img->h};

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
        default:
        break;
    }
}

void enemy_ptero_update(tEntity *this, tPteroLocalData *local)
{
    switch (this->state)
    {
        case E_PTERO_ENEMY_TYPE:
            if (local->timer >= this->pos.y)
            {
                this->state = E_PTERO_MOVE_RIGHT_STATE;
                local->timer = 0;
            }
            else
                local->timer+= (1 * deltaTime);
        break;
        case E_PTERO_MOVE_RIGHT_STATE:
            if (this->pos.x > 130)
                this->state = E_PTERO_MOVE_LEFT_STATE;
            else   
                this->fixVel.x = ftofix(0.4);
        break;
        case E_PTERO_MOVE_LEFT_STATE:
            if (this->pos.x < 70)
                this->state = E_PTERO_MOVE_RIGHT_STATE;
            else   
                this->fixVel.x = ftofix(-0.4);
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

void enemy_trace(tEntity *this)
{
    MY_TRACE("enemy Instance: %d\n\tObj Type:%d\n", this->entInstance, this->entType);
}