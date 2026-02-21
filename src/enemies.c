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
#include "collisions.h"

#define TRACE_FLAG  "[ENEMY]"

uint16_t numEnemyInstances;        //num of enemy instances
static void *enemyDataList;        //list of enemy local data
BITMAP *enemyResources[E_ENEMIES_TYPE_NUM];

void enemy_system_init()
{
    //empty enemy list
    free(enemyDataList);
    enemyDataList = NULL;
    //set number of entities
    numEnemyInstances = 0;     

    MY_TRACE_FLAG("Initialized enemy system\n");
}

void enemy_system_destroy()
{
    //empty enemy list
    free(enemyDataList);
    enemyDataList = NULL;
    //set number of entities
    numEnemyInstances = 0;    
    //free resources
    for (uint8_t i = 0; i < E_ENEMIES_TYPE_NUM; i++)
    {
        if (enemyResources[i])
        {
            destroy_bitmap(enemyResources[i]);
            enemyResources[i] = NULL;
        }
    }

    MY_TRACE_FLAG("Destroyed enemy system\n");
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
            enemyDataList = realloc(enemyDataList, numEnemyInstances * sizeof(tEnemyLocalData));            
            //load enemy resources
            if (!enemyResources[E_PTERO_ENEMY_TYPE])
                enemyResources[E_PTERO_ENEMY_TYPE] = load_bmp("res/enemies/ptero.bmp", NULL);
            
            entity->img = enemyResources[E_PTERO_ENEMY_TYPE]; 
            entity->spriteSize = (tVector){52, 48};
            entity->size.x = 50;
            entity->size.y = 20;                          
            entity->properties = E_ENT_PROP_PERSISTENT;
        break;
        case E_RAPTOR_ENEMY_TYPE:
            //allocate memory for enemy
            enemyDataList = realloc(enemyDataList, numEnemyInstances * sizeof(tEnemyLocalData));
            if (!enemyResources[E_RAPTOR_ENEMY_TYPE])
                enemyResources[E_RAPTOR_ENEMY_TYPE] = load_bmp("res/enemies/raptor2.bmp", NULL);
            
            entity->img = enemyResources[E_RAPTOR_ENEMY_TYPE]; 
            entity->spriteSize = (tVector){72, 44};                          
            entity->size = (tVector){50, 30};
            entity->axis = E_ENT_AXIS_DOWN;
            entity->properties = 0x00; //E_ENT_PROP_PHYSICS_ON;
            collision_create_entity_points(entity);
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
            enemy_ptero_update(entity, &((tEnemyLocalData*)enemyDataList)[entity->entInstance]);
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
            ((tEnemyLocalData*)enemyDataList)[numEnemyInstances - 1].timer = 0;            
            
        break;        
        case E_RAPTOR_ENEMY_TYPE:            
            ((tEnemyLocalData*)enemyDataList)[numEnemyInstances - 1].timer = 0;                              
        break;        
        default:
        break;
    }
}

void enemy_ptero_update(tEntity *this, tEnemyLocalData *local)
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
}

void enemy_raptor_update(tEntity *this, tEnemyLocalData *local)
{              
    #define RAPTOR_RANGE_PATROL     50
    #define RAPTOR_PLAYER_RANGE     20
    
    //enemy animations
    #define ANIM_RAPTOR_WALK   4,   6,  10, ANIM_PING_PONG
    #define ANIM_RAPTOR_ATACK  0,   3,  10, ANIM_PING_PONG
    #define ANIM_RAPTOR_DEAD   7,   9,  15, ANIM_ONCE

    //enemy states
    enum E_RAPTOR_ENEMY_STATES{E_RAPTOR_ST_IDLE, E_RAPTOR_ST_MOVING, E_RAPTOR_ATTACK, E_RAPTOR_HURT};   

    tEntity *player;

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_RAPTOR_HURT;

    switch (this->state)
    {
        case E_RAPTOR_ST_IDLE:
            this->fixVel.x = itofix(1);
            this->state++;
        break;
        case E_RAPTOR_ST_MOVING:
            //uint8_t colDir = 0;
            //this->ground = false;
            //fixed movement
            this->fixVel.x = this->dir == E_ENT_DIR_LEFT ? itofix(-1) : itofix(1);
            
            //check all the entity collision points    
            /*for (uint8_t i = 0; i < NUM_COL_POINTS; i++)
            {                
                //check collision tile for collision point
                colDir = collision_check_tile(this, i);        
                //apply collision direction
                collision_apply_dir(this, colDir);        
                //change direction on collision
                if (colDir == E_COLLISION_LEFT)
                    this->dir = E_ENT_DIR_RIGHT;
                if (colDir == E_COLLISION_RIGHT)
                    this->dir = E_ENT_DIR_LEFT;

                if (!colDir)
                {
                    if (this->pos.x > this->initPos.x + RAPTOR_RANGE_PATROL || this->pos.x < this->initPos.x - RAPTOR_RANGE_PATROL)
                        colDir = colDir ? 0 : 1;
                }
            }*/
            
            if ((this->dir && this->pos.x > (this->initPos.x + RAPTOR_RANGE_PATROL)) || (!this->dir && this->pos.x < (this->initPos.x - RAPTOR_RANGE_PATROL)))
                this->dir = this->dir ? 0 : 1;

            player = entity_get(PLAYER_ENTITY_ID);
            if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, RAPTOR_PLAYER_RANGE)) //this->pos.x < player->pos.x + 5 && this->pos.x < player->pos.x - 5)
                this->state = E_RAPTOR_ATTACK;

            play_animation(&this->anim, ANIM_RAPTOR_WALK);
        break;     
        case E_RAPTOR_ATTACK:
            //this->fixVel.x = 0;
            player = entity_get(PLAYER_ENTITY_ID);
            if (!in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, RAPTOR_PLAYER_RANGE))
                this->state = E_RAPTOR_ST_MOVING;

            play_animation(&this->anim, ANIM_RAPTOR_ATACK); 
        break;   
        case E_RAPTOR_HURT:
            this->fixVel.x = 0;
            
            if (play_animation(&this->anim, ANIM_RAPTOR_DEAD))
            {
                this->dead = true;    
                this->signal = E_ENT_SIGNAL_NONE;
            }
        break;
    }       
}

void enemy_trace(tEntity *this)
{
    MY_TRACE_FLAG("Enemy Instance: %d\n\tObj Type:%d\n", this->entInstance, this->entType);
}