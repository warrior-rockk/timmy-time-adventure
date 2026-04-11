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
#include "resources.h"

#include "data/edata.h"

#define TRACE_FLAG  "[ENEMY]"

uint16_t numEnemyInstances;        //num of enemy instances
static void *enemyDataList;        //list of enemy local data
BITMAP *enemyResources[E_ENEMIES_TYPE_NUM];
SAMPLE *enemySfx[E_SFX_ENEMY_NUM];
DATAFILE_INDEX *enemyDataFileIndex;

void enemy_system_init()
{
    //empty enemy list
    free(enemyDataList);
    enemyDataList = NULL;
    //set number of entities
    numEnemyInstances = 0;     

    //create data file index
    enemyDataFileIndex = create_dat_index("enemies.dat");

    //load enemy sfx
    enemySfx[E_SFX_ENEMY_DEAD]  = load_dat_wav_indexed(enemyDataFileIndex, DEAD_WAV);

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
    {
        if (enemySfx[i])
        {
            destroy_sample(enemySfx[i]);
            enemySfx[i] = NULL;
        }
    }
    //free resources
    for (uint8_t i = 0; i < E_ENEMIES_TYPE_NUM; i++)
    {
        if (enemyResources[i])
        {
            destroy_bitmap(enemyResources[i]);
            enemyResources[i] = NULL;
        }
    }

    destroy_dat_index(enemyDataFileIndex);

    MY_TRACE_FLAG("Destroyed enemy system\n");
}

void enemy_destroy(tEntity *entity)
{
    uint16_t enemyIndex;
    enemyIndex = entity->entInstance;
    
    //copies last enemy to deleted enemy position
    ((tEnemyLocalData*)enemyDataList)[enemyIndex] = ((tEnemyLocalData*)enemyDataList)[numEnemyInstances - 1];
    
    //decrement entity number
    numEnemyInstances--;
    if (numEnemyInstances == 0)
    {
        //free entity list
        free(enemyDataList);
        enemyDataList = NULL;
    }
    else
        //reallocates the array with decremented entity number    
        //TODO: this is incorrect when entities has different local data structure
        enemyDataList = realloc(enemyDataList, numEnemyInstances * sizeof(tEnemyLocalData));     
    
    MY_TRACE_FLAG("Destroyed enemy instance:%i\n", enemyIndex);
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
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, PTERO_BMP);
            entity->img = enemyResources[E_PTERO_ENEMY_TYPE]; 
            entity->spriteSize = (tVector){71, 64};
            entity->size.x = 30;
            entity->size.y = 20;                                      
        break;
        case E_RAPTOR_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, RAPTOR2_BMP);
            entity->img = enemyResources[E_RAPTOR_ENEMY_TYPE]; 
            entity->spriteSize = (tVector){72, 44};                          
            entity->size = (tVector){46, 32};
            entity->axis = E_ENT_AXIS_DOWN;  
            SET_FLAG(entity->properties, E_ENT_PROP_PHYSICS_ON);     
            collision_create_entity_points(entity);     
        break;
        case E_TRICE_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, TRICE_BMP);
            entity->img = enemyResources[E_TRICE_ENEMY_TYPE]; 
            entity->spriteSize = (tVector){48, 34};                          
            entity->size = (tVector){40, 32};
            entity->axis = E_ENT_AXIS_DOWN;  
            SET_FLAG(entity->properties, E_ENT_PROP_PHYSICS_ON);     
            collision_create_entity_points(entity);     
        break;
        case E_SPIDER_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, SPIDER_BMP);
            entity->img = enemyResources[E_SPIDER_ENEMY_TYPE]; 
            entity->spriteSize = (tVector){20, 29};                          
            entity->size = (tVector){16, 16};        
            collision_create_entity_points(entity);              
        break;        
        case E_PIRANHA_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, PIRANHA_BMP);
            entity->img = enemyResources[E_PIRANHA_ENEMY_TYPE]; 
            entity->spriteSize = (tVector){42, 33};                          
            entity->size = (tVector){32, 32};                    
        break;
        case E_COWBOY_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, COWBOY_BMP);
            load_entity_wav_resources(&enemySfx[E_SFX_ENEMY_SHOOT], enemyDataFileIndex, SHOOT_WAV);
            entity->img = enemyResources[E_COWBOY_ENEMY_TYPE]; 
            entity->spriteSize = (tVector){50, 45};                          
            entity->size = (tVector){18, 40};        
            entity->axis = E_ENT_AXIS_DOWN;                        
        break;
        case E_BULLET_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, BULLET_BMP);
            entity->img = enemyResources[E_BULLET_ENEMY_TYPE]; 
            entity->spriteSize = (tVector){2, 2};                          
            entity->size = (tVector){2, 2};  
            entity->properties = E_ENT_PROP_AUTO_DESTROY | E_ENT_PROP_NO_HURT;                  
        break;
        case E_EAGLE_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, EAGLE_BMP);
            entity->img = enemyResources[E_EAGLE_ENEMY_TYPE]; 
            entity->spriteSize = (tVector){32, 34};                          
            entity->size = (tVector){20, 18};      
            collision_create_entity_points(entity);              
        break;        
        case E_TUMBLE_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, TUMBLE_BMP);
            entity->img = enemyResources[E_TUMBLE_ENEMY_TYPE]; 
            entity->spriteSize = (tVector){30, 29};                          
            entity->size = (tVector){30, 29};                  
        break;
        case E_SCORPION_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, SCORPION_BMP);
            entity->img = enemyResources[E_SCORPION_ENEMY_TYPE]; 
            entity->spriteSize = (tVector){30, 23};                          
            entity->size = (tVector){24, 16};      
            entity->axis = E_ENT_AXIS_DOWN;
            collision_create_entity_points(entity);              
        break;        
        default:
            abort_on_error("Enemy type entity not valid");
        break;
    }

    //test memory allocation
    MY_ASSERT(enemyDataList);

    //set actual instance num
    entity->entInstance = numEnemyInstances - 1;

    #if DEBUG_TRACE_CREATE_ENTITIES
        MY_TRACE_FLAG("Created enemy instance:%i\n", entity->entInstance);
    #endif
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
        case E_COWBOY_ENEMY_TYPE:            
            enemy_cowboy_update(entity, &((tEnemyLocalData*)enemyDataList)[entity->entInstance]);
        break;
        case E_BULLET_ENEMY_TYPE:            
            enemy_bullet_update(entity, &((tEnemyLocalData*)enemyDataList)[entity->entInstance]);
        break;
        case E_EAGLE_ENEMY_TYPE:            
            enemy_eagle_update(entity, &((tEnemyLocalData*)enemyDataList)[entity->entInstance]);
        break;
        case E_TUMBLE_ENEMY_TYPE:            
            enemy_tumble_update(entity, &((tEnemyLocalData*)enemyDataList)[entity->entInstance]);
        break;
        case E_TRICE_ENEMY_TYPE:            
            enemy_trice_update(entity, &((tEnemyLocalData*)enemyDataList)[entity->entInstance]);
        break;
        case E_SCORPION_ENEMY_TYPE:            
            enemy_scorpion_update(entity, &((tEnemyLocalData*)enemyDataList)[entity->entInstance]);
        break;
        default:
        break;
    }

    show_debug("Num enemies:%i", numEnemyInstances);
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

void enemy_dead(tEntity *entity, int startFrame, int endFrame, int speed, uint8_t mode)
{
    //stop enemy
    entity->fixVel.x = 0;
    entity->fixVel.y = 0;
    //blink
    entity_blink(entity);
    //play dead sfx
    if (entity->state != entity->prevState)
    {
        sfx_play(enemySfx[E_SFX_ENEMY_DEAD], E_SFX_ENEMY_VOICE);
        game.score += SCORE_POINT_HURT_ENEMY;
    }
    
    //play dead animation
    if (play_animation(&entity->anim, startFrame, endFrame, speed, mode))
    {
        entity->dead = true;
        entity->signal = E_ENT_SIGNAL_NONE;
        MY_TRACE_FLAG("Entity %i set dead\n", entity->id);
    }
}

//=========================================================================

void enemy_ptero_update(tEntity *this, tEnemyLocalData *local)
{
    //enemy defines
    #define PTERO_VELOCITY                  0.4
    #define PTERO_DEFAULT_RANGE_PATROL      20

    //enemy animations
    #define ANIM_PTERO_FLY     0,   1,  20, ANIM_LOOP
    #define ANIM_PTERO_DEAD    2,   5,  ENEMY_DEFAULT_DEAD_TIME, ANIM_ONCE
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
            enemy_patrol_ia(this, ftofix(PTERO_VELOCITY), this->spare != 0 ? this->spare : PTERO_DEFAULT_RANGE_PATROL);
                                    
            play_animation(&this->anim, ANIM_PTERO_FLY);            
        break;
        case E_PTERO_ST_HURT:
            enemy_dead(this, ANIM_PTERO_HURT);            
        break;
        default:
            this->state = E_PTERO_ST_IDLE;
    }
}

void enemy_raptor_update(tEntity *this, tEnemyLocalData *local)
{              
    #define RAPTOR_VELOCITY         0.6
    #define RAPTOR_RANGE_PATROL     50
    #define RAPTOR_PLAYER_RANGE     20
    
    //enemy animations
    #define ANIM_RAPTOR_WALK   4,   6,  10, ANIM_PING_PONG
    #define ANIM_RAPTOR_ATACK  0,   3,  10, ANIM_PING_PONG
    #define ANIM_RAPTOR_DEAD   7,   9,  ENEMY_DEFAULT_DEAD_TIME, ANIM_ONCE

    //enemy states
    enum E_RAPTOR_ENEMY_STATES{E_RAPTOR_ST_IDLE, E_RAPTOR_ST_MOVING, E_RAPTOR_ST_ATTACK, E_RAPTOR_ST_HURT};   

    tEntity *player;

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_RAPTOR_ST_HURT;
    
    //terrain collisions
    uint8_t colDir = 0;
    this->ground = false;
    //check all the entity collision points    
    for (uint8_t i = 0; i < E_NUM_COL_POINTS; i++)
    {                
        //check collision tile for collision point
        colDir = collision_check_tile(this, i);        
        //apply collision direction
        collision_apply_dir(this, colDir, E_COLLISION_NO_BOUNCE);       
        
        //change direction if horizontal collision
        if (colDir == E_COLLISION_DIR_RIGHT || colDir == E_COLLISION_DIR_LEFT)
            this->dir = !this->dir;
    }

    switch (this->state)
    {
        case E_RAPTOR_ST_IDLE:            
            this->state++;
        break;
        case E_RAPTOR_ST_MOVING:            
            enemy_patrol_ia(this, ftofix(RAPTOR_VELOCITY), RAPTOR_RANGE_PATROL);
            
            //check range of player
            player = entity_get(entity_get_player_id());
            if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, RAPTOR_PLAYER_RANGE))
                this->state = E_RAPTOR_ST_ATTACK;

            play_animation(&this->anim, ANIM_RAPTOR_WALK);
        break;     
        case E_RAPTOR_ST_ATTACK:
            //check range of player
            player = entity_get(entity_get_player_id());
            if (!in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, RAPTOR_PLAYER_RANGE))
                this->state = E_RAPTOR_ST_MOVING;

            play_animation(&this->anim, ANIM_RAPTOR_ATACK); 
        break;   
        case E_RAPTOR_ST_HURT:
            enemy_dead(this, ANIM_RAPTOR_DEAD);            
        break;
    }       
}

void enemy_trice_update(tEntity *this, tEnemyLocalData *local)
{              
    #define TRICE_WALK_VELOCITY     0.4
    #define TRICE_RUN_VELOCITY      1.2
    #define TRICE_RANGE_PATROL      50
    #define TRICE_PLAYER_RANGE      90
    
    //enemy animations
    #define ANIM_TRICE_IDLE    1,   1,  10, ANIM_LOOP
    #define ANIM_TRICE_RUN     10,  18, 5, ANIM_LOOP
    #define ANIM_TRICE_DEAD    19,  22, ENEMY_DEFAULT_DEAD_TIME, ANIM_ONCE

    //enemy states
    enum E_TRICE_ENEMY_STATES{E_TRICE_ST_IDLE, E_TRICE_ST_MOVING, E_TRICE_ST_ATTACK, E_TRICE_ST_HURT};   

    tEntity *player;

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_TRICE_ST_HURT;
    
    switch (this->state)
    {
        case E_TRICE_ST_IDLE:            
            this->state++;
        break;
        case E_TRICE_ST_MOVING:            
            //enemy_patrol_ia(this, ftofix(TRICE_WALK_VELOCITY), TRICE_PLAYER_RANGE);
            

            //check range of player
            
            player = entity_get(entity_get_player_id());
            this->dir = player->pos.x < this->pos.x ? E_ENT_DIR_LEFT : E_ENT_DIR_RIGHT;
            if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, TRICE_PLAYER_RANGE))
            {
                this->state = E_TRICE_ST_ATTACK;
            }

            play_animation(&this->anim, ANIM_TRICE_IDLE);
        break;     
        case E_TRICE_ST_ATTACK:
            this->fixVel.x = this->dir == E_ENT_DIR_LEFT ? ftofix(-TRICE_RUN_VELOCITY) : ftofix(TRICE_RUN_VELOCITY); 
            
            //check range of player
            player = entity_get(entity_get_player_id());
            //this->dir = player->pos.x < this->pos.x ? E_ENT_DIR_LEFT : E_ENT_DIR_RIGHT;
            if (!in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, TRICE_PLAYER_RANGE))
                this->state = E_TRICE_ST_MOVING;

            play_animation(&this->anim, ANIM_TRICE_RUN); 
        break;   
        case E_TRICE_ST_HURT:
            enemy_dead(this, ANIM_TRICE_DEAD);            
        break;
    }       

    //terrain collisions
    uint8_t colDir = 0;
    this->ground = false;
    //check all the entity collision points    
    for (uint8_t i = 0; i < E_NUM_COL_POINTS; i++)
    {                
        //check collision tile for collision point
        colDir = collision_check_tile(this, i);        
        //apply collision direction
        collision_apply_dir(this, colDir, E_COLLISION_NO_BOUNCE);        
    }
}

void enemy_piranha_update(tEntity *this, tEnemyLocalData *local)
{              
    //enemy definitions
    #define PIRANHA_DEFAULT_WAIT_TIME   150
    #define PIRANHA_VEL_Y               -4
    #define PIRANHA_VEL_X               0.8

    //enemy animations    
    #define ANIM_PIRANHA_JUMP   1,   9,  5,  ANIM_ONCE
    #define ANIM_PIRANHA_DEAD   10,  10, ENEMY_DEFAULT_DEAD_TIME, ANIM_ONCE
    
    //enemy states
    enum E_PIRANHA_ENEMY_STATES{E_PIRANHA_ST_IDLE, E_PIRANHA_ST_DELAY, E_PIRANHA_ST_JUMP, E_PIRANHA_ST_HURT};   
    
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
            this->fixVel.y = 0;
            this->fixVel.x = 0;

            if (clock_counter_check(PIRANHA_DEFAULT_WAIT_TIME))
            {
                this->state = E_PIRANHA_ST_DELAY;    
            }
        break;
        case E_PIRANHA_ST_DELAY:
            //waits spare delay
            if (local->timer >= this->spare) 
            {                
                this->state = E_PIRANHA_ST_JUMP;                
                SET_FLAG(this->properties, E_ENT_PROP_PHYSICS_ON);
                CLEAR_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
                this->fixVel.y = ftofix(PIRANHA_VEL_Y);
                this->fixVel.x = ftofix(PIRANHA_VEL_X);
                this->ground = false;
            }
            else
                local->timer += clock_tick_get();
        break;
        case E_PIRANHA_ST_JUMP:            
            local->timer = 0;
            this->visible = true;            
            if (play_animation(&this->anim, ANIM_PIRANHA_JUMP))                
                this->state = E_PIRANHA_ST_IDLE;
        break;             
        case E_PIRANHA_ST_HURT:
            enemy_dead(this, ANIM_PIRANHA_DEAD);            
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
    #define ANIM_SPIDER_DEAD   0,   1,  ENEMY_DEFAULT_DEAD_TIME, ANIM_ONCE
    
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
                local->flag = this->pos.y - this->initPos.y;
                MY_TRACE_FLAG("flag: %i\n", local->flag);
            }
        break;      
        case E_SPIDER_ST_MOVING_2:
            this->fixVel.y = ftofix(-SPIDER_VELOCITY);            
            play_animation(&this->anim, ANIM_SPIDER_TURN);

            if (this->pos.y < this->initPos.y + (local->flag >> 1))
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
           enemy_dead(this, ANIM_SPIDER_DEAD);
        break;
    }
}

void enemy_cowboy_update(tEntity *this, tEnemyLocalData *local)
{              
    //enemy definitions
    #define COWBOY_PLAYER_RANGE     160
    #define COWBOY_WAIT_TIME        15

    //enemy animations
    #define ANIM_COWBOY_IDLE   0,   0, 10,  ANIM_LOOP
    #define ANIM_COWBOY_SHOOT  1,   16, 5,  ANIM_ONCE
    #define ANIM_COWBOY_DEAD   17,  24, ENEMY_DEFAULT_DEAD_TIME, ANIM_ONCE

    //enemy states
    enum E_COWBOY_ENEMY_STATES{E_COWBOY_ST_IDLE, E_COWBOY_ST_SHOOT, E_COWBOY_ST_WAIT, E_COWBOY_ST_HURT};   

    tEntity *player;

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_COWBOY_ST_HURT;

    switch (this->state)
    {
        case E_COWBOY_ST_IDLE:        
            local->flag = false;    
            //get player instance
            player = entity_get(entity_get_player_id());            
            
            //direction faces player
            this->dir = player->pos.x < this->pos.x ? E_ENT_DIR_LEFT : E_ENT_DIR_RIGHT;

            //shoot in player range
            if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, COWBOY_PLAYER_RANGE))
                this->state++;

            play_animation(&this->anim, ANIM_COWBOY_IDLE);
        break;
        case E_COWBOY_ST_SHOOT:            
            if (play_animation(&this->anim, ANIM_COWBOY_SHOOT))
            {
                this->state++;      
            }
            if (this->anim.frame == 11 && !local->flag)
            {
                entity_create(E_ENT_CLASS_ENEMY, E_BULLET_ENEMY_TYPE, (tVector){this->pos.x, this->pos.y + 10}, this->dir, this->spare);
                sfx_play(enemySfx[E_SFX_ENEMY_SHOOT], E_SFX_ENEMY_VOICE);
                local->flag = true;
            }
        break;     
        case E_COWBOY_ST_WAIT:
            if (local->timer > COWBOY_WAIT_TIME)
            {
                local->timer = 0;
                this->state = E_COWBOY_ST_IDLE;
            }
            else
                local->timer += clock_tick_get();

            play_animation(&this->anim, ANIM_COWBOY_IDLE); 
        break;   
        case E_COWBOY_ST_HURT:
            enemy_dead(this, ANIM_COWBOY_DEAD);            
        break;
    }       
}

void enemy_bullet_update(tEntity *this, tEnemyLocalData *local)
{              
    //enemy defines
    #define BULLET_VELOCITY     4.0

    //enemy animations
    #define ANIM_BULLET_IDLE   0,   0, 10,  ANIM_LOOP

    //enemy states
    enum E_BULLET_ENEMY_STATES{E_BULLET_ST_IDLE};   

    switch (this->state)
    {
        case E_BULLET_ST_IDLE:            
            this->fixVel.x = this->dir == E_ENT_DIR_LEFT ? ftofix(-BULLET_VELOCITY) : ftofix(BULLET_VELOCITY);

            play_animation(&this->anim, ANIM_BULLET_IDLE);
        break;
    }       
}

void enemy_eagle_update(tEntity *this, tEnemyLocalData *local)
{              
    //enemy definitions
    #define EAGLE_PATROL_VELOCITY     0.8
    #define EAGLE_PATROL_RANGE        50
    #define EAGLE_PLAYER_RANGE        45
    #define EAGLE_ATTACK_VEL_Y        1.6
    #define EAGLE_ATTACK_MAX_VEL_X    2
    
    //enemy animations
    #define ANIM_EAGLE_FLY     0,   7, 10,  ANIM_LOOP
    #define ANIM_EAGLE_ATTACK  9,   9, 10,  ANIM_LOOP
    #define ANIM_EAGLE_RETURN  10,  10, 10, ANIM_LOOP
    #define ANIM_EAGLE_HURT    11,  14, ENEMY_DEFAULT_DEAD_TIME, ANIM_ONCE

    //enemy states
    enum E_EAGLE_ENEMY_STATES{E_EAGLE_ST_FLY, E_EAGLE_ST_ATTACK, E_EAGLE_ST_RETURN, E_EAGLE_ST_HURT};   

    tEntity *player;

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_EAGLE_ST_HURT;

    switch (this->state)
    {
        case E_EAGLE_ST_FLY:        
            //get player instance
            player = entity_get(entity_get_player_id());
            
            enemy_patrol_ia(this, ftofix(EAGLE_PATROL_VELOCITY), EAGLE_PATROL_RANGE);
            
            //attack in player range
            if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, EAGLE_PLAYER_RANGE))
            {
                this->state++;
                //direction to attack player
                this->dir = player->pos.x < this->pos.x ? E_ENT_DIR_LEFT : E_ENT_DIR_RIGHT;
                
                //calculate x velocity
                fixed targetX = abs(itofix(player->pos.x - this->pos.x));
                fixed targetY = abs(itofix(player->pos.y - this->pos.y));
                fixed velX = fix_clamp(fixdiv(targetX, fixdiv(targetY, ftofix(EAGLE_ATTACK_VEL_Y))), 0, itofix(EAGLE_ATTACK_MAX_VEL_X));
                this->fixVel.x = this->dir == E_ENT_DIR_LEFT ? -velX : velX;                
            }

            play_animation(&this->anim, ANIM_EAGLE_FLY);
        break;        
        case E_EAGLE_ST_ATTACK:
            //get player instance
            player = entity_get(entity_get_player_id());
            //set attack velocities
            this->fixVel.y = ftofix(EAGLE_ATTACK_VEL_Y);
                        
            //end attack                        
            //check collision tile for collision point
            uint8_t colDir = collision_check_tile(this, E_COLPOINT_DOWN_L);        
            collision_apply_dir(this, colDir, E_COLLISION_NO_BOUNCE);
            if (colDir == E_COLLISION_DIR_DOWN)
            {
                this->state++;                
            }

            play_animation(&this->anim, ANIM_EAGLE_ATTACK);
        break;
        case E_EAGLE_ST_RETURN:
            //set return velocities
            this->fixVel.y = -itofix(EAGLE_ATTACK_VEL_Y);
            this->fixVel.x = this->dir == E_ENT_DIR_LEFT ? -ftofix(EAGLE_ATTACK_VEL_Y) : ftofix(EAGLE_ATTACK_VEL_Y);

            if (in_range(this->pos.y, this->initPos.y, 2))
            {
                this->pos.y = this->initPos.y;
                this->fixVel.x = 0;
                this->fixVel.y = 0;
                this->state = E_EAGLE_ST_FLY;
            }

            play_animation(&this->anim, ANIM_EAGLE_RETURN);
        break;
        case E_EAGLE_ST_HURT:
            enemy_dead(this, ANIM_EAGLE_HURT);            
        break;
    }       
}

void enemy_tumble_update(tEntity *this, tEnemyLocalData *local)
{              
    //enemy defines
    #define TUMBLE_VELOCITY     2.2

    //enemy animations
    #define ANIM_TUMBLE_ROLL   0,   7, 10,  ANIM_LOOP

    //enemy states
    enum E_TUMBLE_ENEMY_STATES{E_TUMBLE_ST_ROLL};
    
    if (this->signal == E_ENT_SIGNAL_AWAKE)
    {
        this->signal = E_ENT_SIGNAL_NONE;
        this->pos = this->initPos;
    }

    switch (this->state)
    {
        case E_TUMBLE_ST_ROLL:            
            this->fixVel.x = this->dir == E_ENT_DIR_LEFT ? ftofix(-TUMBLE_VELOCITY) : ftofix(TUMBLE_VELOCITY);

            play_animation(&this->anim, ANIM_TUMBLE_ROLL);

            if (!scroll_rect_on_region((tRectangle){this->pos, this->size}))
            {
                this->dead = true;
            }
        break;
    }       
}

void enemy_trace(tEntity *this)
{
    MY_TRACE_FLAG("Enemy Instance: %d\n\tObj Type:%d\n", this->entInstance, this->entType);
}

void enemy_scorpion_update(tEntity *this, tEnemyLocalData *local)
{              
    #define SCORPION_VELOCITY         0.6
    #define SCORPION_RANGE_PATROL     50
    
    //enemy animations
    #define ANIM_SCORPION_WALK   1,   4,  10, ANIM_LOOP
    #define ANIM_SCORPION_DEAD   0,   0,  ENEMY_DEFAULT_DEAD_TIME, ANIM_ONCE

    //enemy states
    enum E_SCORPION_ENEMY_STATES{E_SCORPION_ST_IDLE, E_SCORPION_ST_MOVING, E_SCORPION_ST_HURT};   

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_SCORPION_ST_HURT;
    
    //terrain collisions
    uint8_t colDir = 0;
    this->ground = false;
    //check all the entity collision points    
    for (uint8_t i = 0; i < E_NUM_COL_POINTS; i++)
    {                
        //check collision tile for collision point
        colDir = collision_check_tile(this, i);        
        //apply collision direction
        collision_apply_dir(this, colDir, E_COLLISION_NO_BOUNCE);    
        
        //change direction if horizontal collision
        if (colDir == E_COLLISION_DIR_RIGHT || colDir == E_COLLISION_DIR_LEFT)
            this->dir = !this->dir;
    }

    switch (this->state)
    {
        case E_SCORPION_ST_IDLE:            
            this->state++;
        break;
        case E_SCORPION_ST_MOVING:            
            enemy_patrol_ia(this, ftofix(SCORPION_VELOCITY), SCORPION_RANGE_PATROL);
            
            play_animation(&this->anim, ANIM_SCORPION_WALK);
        break;     
        case E_SCORPION_ST_HURT:
            enemy_dead(this, ANIM_SCORPION_DEAD);            
        break;
    }       
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