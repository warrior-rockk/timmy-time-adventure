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
#include "sound.h"

#define TRACE_FLAG  "[ENEMY]"

uint16_t numEnemyInstances;        //num of enemy instances
static void *enemyDataList;        //list of enemy local data
BITMAP *enemyResources[E_ENEMIES_TYPE_NUM];
SAMPLE *enemySfx[E_SFX_ENEMY_NUM];

void enemy_system_init()
{
    //empty enemy list
    free(enemyDataList);
    enemyDataList = NULL;
    //set number of entities
    numEnemyInstances = 0;     

    //load enemy sfx
    enemySfx[E_SFX_ENEMY_DEAD]  = load_wav("res/enemies/dead.wav");

    MY_TRACE_FLAG("Initialized enemy system\n");
}

void enemy_system_destroy()
{
    //empty enemy list
    free(enemyDataList);
    enemyDataList = NULL;
    //set number of entities
    numEnemyInstances = 0;   
    //free samples
    for (uint8_t i = 0; i < E_SFX_ENEMY_NUM; i++)
        destroy_sample(enemySfx[i]);
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

    //allocate memory for enemy
    enemyDataList = realloc(enemyDataList, numEnemyInstances * sizeof(tEnemyLocalData));            

    //set enemy type properties
    switch (entity->entType)
    {
        case E_PTERO_ENEMY_TYPE:            
            //load enemy resources
            if (!enemyResources[E_PTERO_ENEMY_TYPE])
                enemyResources[E_PTERO_ENEMY_TYPE] = load_bmp("res/enemies/ptero.bmp", NULL);
            
            entity->img = enemyResources[E_PTERO_ENEMY_TYPE]; 
            entity->spriteSize = (tVector){71, 64};
            entity->size.x = 50;
            entity->size.y = 20;                                      
        break;
        case E_RAPTOR_ENEMY_TYPE:
            if (!enemyResources[E_RAPTOR_ENEMY_TYPE])
                enemyResources[E_RAPTOR_ENEMY_TYPE] = load_bmp("res/enemies/raptor2.bmp", NULL);
            
            entity->img = enemyResources[E_RAPTOR_ENEMY_TYPE]; 
            entity->spriteSize = (tVector){72, 44};                          
            entity->size = (tVector){50, 30};
            entity->axis = E_ENT_AXIS_DOWN;            
        break;
        case E_SPIDER_ENEMY_TYPE:
            if (!enemyResources[E_SPIDER_ENEMY_TYPE])
                enemyResources[E_SPIDER_ENEMY_TYPE] = load_bmp("res/enemies/spider.bmp", NULL);
            
            entity->img = enemyResources[E_SPIDER_ENEMY_TYPE]; 
            entity->spriteSize = (tVector){20, 29};                          
            entity->size = (tVector){16, 16};        
            collision_create_entity_points(entity);              
        break;        
        case E_PIRANHA_ENEMY_TYPE:
            if (!enemyResources[E_PIRANHA_ENEMY_TYPE])
                enemyResources[E_PIRANHA_ENEMY_TYPE] = load_bmp("res/enemies/piranha.bmp", NULL);
            
            entity->img = enemyResources[E_PIRANHA_ENEMY_TYPE]; 
            entity->spriteSize = (tVector){42, 33};                          
            entity->size = (tVector){32, 32};                    
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
        case E_SPIDER_ENEMY_TYPE:            
            enemy_spider_update(entity, &((tEnemyLocalData*)enemyDataList)[entity->entInstance]);
        break;
        case E_PIRANHA_ENEMY_TYPE:            
            enemy_piranha_update(entity, &((tEnemyLocalData*)enemyDataList)[entity->entInstance]);
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
        default:
            ((tEnemyLocalData*)enemyDataList)[entity->entInstance].flag = 0;
            ((tEnemyLocalData*)enemyDataList)[entity->entInstance].timer = 0;
        break;
    }
}


//ENEMY IA FUNCTIONS
//=========================================================================

//Enemy patrol: moves around a X range
void enemy_patrol_ia(tEntity *entity, fixed velocity, int16_t patrol_range)
{
    //linear movement
    entity->fixVel.x = entity->dir == E_ENT_DIR_LEFT ? -velocity : velocity;            
    
    //change direction on range patrol
    if ((entity->dir && entity->pos.x > (entity->initPos.x + patrol_range)) || (!entity->dir && entity->pos.x < (entity->initPos.x - patrol_range)))
        entity->dir = !entity->dir;
}

void enemy_dead(tEntity *entity, uint8_t deadStateNum, int startFrame, int endFrame, int speed, uint8_t mode)
{
    //stop enemy
    entity->fixVel.x = 0;
    entity->fixVel.y = 0;
    //blink
    entity_blink(entity);
    //play dead sfx
    if (entity->state != entity->prevState)
        sfx_play(enemySfx[E_SFX_ENEMY_DEAD], E_SFX_ENEMY_VOICE, false);
    //play dead animation
    if (play_animation(&entity->anim, startFrame, endFrame, speed, mode))
    {
        entity->dead = true;
        entity->signal = E_ENT_SIGNAL_NONE;
    }
}

//=========================================================================

void enemy_ptero_update(tEntity *this, tEnemyLocalData *local)
{
    //enemy defines
    #define PTERO_VELOCITY          0.4
    #define PTERO_RANGE_PATROL      20

    //enemy animations
    #define ANIM_PTERO_FLY     0,   1,  20, ANIM_LOOP
    #define ANIM_PTERO_DEAD    2,   5,  20, ANIM_ONCE
    #define ANIM_PTERO_HURT    6,   8,  20, ANIM_ONCE

    //enemy states
    enum E_PTERO_ENEMY_STATE{E_PTERO_ST_IDLE, E_PTERO_ST_MOVE, E_PTERO_ST_HURT};

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)       
        this->state = E_PTERO_ST_HURT;              

    //check state
    switch (this->state)
    {
        case E_PTERO_ST_IDLE:
            this->state = E_PTERO_ST_MOVE;                
        break;
        case E_PTERO_ST_MOVE:
            enemy_patrol_ia(this, ftofix(PTERO_VELOCITY), PTERO_RANGE_PATROL);
                                    
            play_animation(&this->anim, ANIM_PTERO_FLY);            
        break;
        case E_PTERO_ST_HURT:
            enemy_dead(this, E_PTERO_ST_HURT, ANIM_PTERO_HURT);            
        break;
        default:
            this->state = E_PTERO_ST_IDLE;
    }
}

void enemy_raptor_update(tEntity *this, tEnemyLocalData *local)
{              
    #define RAPTOR_VELOCITY         0.8
    #define RAPTOR_RANGE_PATROL     50
    #define RAPTOR_PLAYER_RANGE     20
    
    //enemy animations
    #define ANIM_RAPTOR_WALK   4,   6,  10, ANIM_PING_PONG
    #define ANIM_RAPTOR_ATACK  0,   3,  10, ANIM_PING_PONG
    #define ANIM_RAPTOR_DEAD   7,   9,  15, ANIM_ONCE

    //enemy states
    enum E_RAPTOR_ENEMY_STATES{E_RAPTOR_ST_IDLE, E_RAPTOR_ST_MOVING, E_RAPTOR_ST_ATTACK, E_RAPTOR_ST_HURT};   

    tEntity *player;

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_RAPTOR_ST_HURT;
    
    switch (this->state)
    {
        case E_RAPTOR_ST_IDLE:            
            this->state++;
        break;
        case E_RAPTOR_ST_MOVING:            
            enemy_patrol_ia(this, ftofix(RAPTOR_VELOCITY), RAPTOR_PLAYER_RANGE);
            
            //check range of player
            player = entity_get(PLAYER_ENTITY_ID);
            if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, RAPTOR_PLAYER_RANGE))
                this->state = E_RAPTOR_ST_ATTACK;

            play_animation(&this->anim, ANIM_RAPTOR_WALK);
        break;     
        case E_RAPTOR_ST_ATTACK:
            //check range of player
            player = entity_get(PLAYER_ENTITY_ID);
            if (!in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, RAPTOR_PLAYER_RANGE))
                this->state = E_RAPTOR_ST_MOVING;

            play_animation(&this->anim, ANIM_RAPTOR_ATACK); 
        break;   
        case E_RAPTOR_ST_HURT:
            enemy_dead(this, E_RAPTOR_ST_HURT, ANIM_RAPTOR_DEAD);            
        break;
    }       
}
void enemy_piranha_update(tEntity *this, tEnemyLocalData *local)
{              
    //enemy animations
    #define ANIM_PIRANHA_JUMP   1,   9,  5, ANIM_ONCE
    
    //enemy states
    enum E_PIRANHA_ENEMY_STATES{E_PIRANHA_ST_IDLE, E_PIRANHA_ST_JUMP, E_PIRANHA_ST_HURT};   
    
    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_PIRANHA_ST_HURT;
    
    switch (this->state)
    {
        case E_PIRANHA_ST_IDLE:            
            CLEAR_FLAG(this->properties, E_ENT_PROP_PHYSICS_ON);
            SET_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            this->anim.frame = 0;
            this->visible = false;
            this->fixPos.x = itofix(this->initPos.x);
            this->fixPos.y = itofix(this->initPos.y);

            if (local->timer >= 2)
            {
                this->state++;
                SET_FLAG(this->properties, E_ENT_PROP_PHYSICS_ON);
                CLEAR_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
                this->fixVel.y = ftofix(-4);
                this->fixVel.x = ftofix(0.8);
                this->ground = false;
            }
            else
                local->timer += get_clock_tick_1sec();
        break;
        case E_PIRANHA_ST_JUMP:            
            local->timer = 0;
            this->visible = true;            
            if (play_animation(&this->anim, ANIM_PIRANHA_JUMP))
                this->state--;
        break;             
        case E_PIRANHA_ST_HURT:
            enemy_dead(this, E_PIRANHA_ST_HURT, ANIM_RAPTOR_DEAD);            
        break;
    }       
}


void enemy_spider_update(tEntity *this, tEnemyLocalData *local)
{              
    //enemy defines
    #define SPIDER_VELOCITY     0.6

    //enemy animations
    #define ANIM_SPIDER_IDLE   0,   0,  20, ANIM_LOOP
    #define ANIM_SPIDER_TURN   0,   7,  20, ANIM_LOOP

    //enemy states
    enum E_SPIDER_ENEMY_STATES{E_SPIDER_ST_IDLE, E_SPIDER_ST_MOVING_1, E_SPIDER_ST_MOVING_2, E_SPIDER_ST_MOVING_3, E_SPIDER_ST_MOVING_4, E_SPIDER_HURT};   

    uint8_t colDir = 0;

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)    
        this->state = E_SPIDER_HURT;
    
    switch (this->state)
    {
        case E_SPIDER_ST_IDLE:
            this->fixVel.y = 0;
            play_animation(&this->anim, ANIM_SPIDER_IDLE);
            this->state++;
        break;
        case E_SPIDER_ST_MOVING_1:
            this->fixVel.y = ftofix(SPIDER_VELOCITY);            
            
            play_animation(&this->anim, ANIM_SPIDER_TURN);
            
            //check collision tile for collision point
            colDir = collision_check_tile(this, E_COLPOINT_DOWN_L);        
            //apply collision direction
            collision_apply_dir(this, colDir, E_COLLISION_NO_BOUNCE);        
            //change direction on collision
            if (colDir == E_COLLISION_DIR_DOWN)
            {
                this->state++;
                local->flag = this->pos.y;
            }
        break;      
        case E_SPIDER_ST_MOVING_2:
            this->fixVel.y = ftofix(-SPIDER_VELOCITY);            
            play_animation(&this->anim, ANIM_SPIDER_TURN);

            if (this->pos.y < (local->flag - (local->flag >> 1)))
                this->state++;
        break;
        case E_SPIDER_ST_MOVING_3:
            this->fixVel.y = ftofix(SPIDER_VELOCITY);            
            play_animation(&this->anim, ANIM_SPIDER_TURN);

            //check collision tile for collision point
            colDir = collision_check_tile(this, E_COLPOINT_DOWN_L);        
            //apply collision direction
            collision_apply_dir(this, colDir, E_COLLISION_NO_BOUNCE);        
            //change direction on collision
            if (colDir == E_COLLISION_DIR_DOWN)
            {
                this->state++;
                local->flag = this->pos.y;
            }
        break;
        case E_SPIDER_ST_MOVING_4:
            this->fixVel.y = ftofix(-SPIDER_VELOCITY);            
            play_animation(&this->anim, ANIM_SPIDER_TURN);
            
            if (this->pos.y < this->initPos.y)
                this->state = E_SPIDER_ST_IDLE;
        break;
        case E_SPIDER_HURT:
           enemy_dead(this, E_SPIDER_HURT, ANIM_PTERO_HURT);
        break;
    }       
}

void enemy_trace(tEntity *this)
{
    MY_TRACE_FLAG("Enemy Instance: %d\n\tObj Type:%d\n", this->entInstance, this->entType);
}

//TEMPLATE FOR ENEMY
/*
void enemy_template_update(tEntity *this, tEnemyLocalData *local)
{              
    //enemy defines
    #define RAPTOR_RANGE_PATROL     50
    #define RAPTOR_PLAYER_RANGE     20
    
    //enemy animations
    #define ANIM_RAPTOR_WALK   4,   6,  10, ANIM_PING_PONG
    #define ANIM_RAPTOR_ATACK  0,   3,  10, ANIM_PING_PONG
    #define ANIM_RAPTOR_DEAD   7,   9,  15, ANIM_ONCE

    //enemy states
    enum E_RAPTOR_ENEMY_STATES{E_RAPTOR_ST_IDLE, E_RAPTOR_ST_MOVING, E_RAPTOR_ATTACK, E_RAPTOR_HURT};   

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_RAPTOR_HURT;

    switch (this->state)
    {
        case E_RAPTOR_ST_IDLE:
            this->fixVel.x = itofix(1);
            this->state++;
        break;
        case E_RAPTOR_ST_MOVING:    //basic terrain collision
            uint8_t colDir = 0;
            this->ground = false;
            //fixed movement
            this->fixVel.x = this->dir == E_ENT_DIR_LEFT ? itofix(-1) : itofix(1);
            
            //check all the entity collision points    
            for (uint8_t i = 0; i < E_NUM_COL_POINTS; i++)
            {                
                //check collision tile for collision point
                colDir = collision_check_tile(this, i);        
                //apply collision direction
                collision_apply_dir(this, colDir);        
                //change direction on collision
                if (colDir == E_COLLISION_DIR_LEFT)
                    this->dir = E_ENT_DIR_RIGHT;
                if (colDir == E_COLLISION_DIR_RIGHT)
                    this->dir = E_ENT_DIR_LEFT;

                if (!colDir)
                {
                    if (this->pos.x > this->initPos.x + RAPTOR_RANGE_PATROL || this->pos.x < this->initPos.x - RAPTOR_RANGE_PATROL)
                        colDir = colDir ? 0 : 1;
                }
            }            
            
            play_animation(&this->anim, ANIM_RAPTOR_WALK);
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
}*/