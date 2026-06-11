/********************************************************************
* Object entities (test)
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#include "allegro.h"
#include "utils.h"
#include "game.h"
#include "objects.h"
#include "collisions.h"
#include "sound.h"
#include "resources.h"
#include "timer.h"
#include "map.h"
#include "enemies.h"

#include "data/odata.h"

#define TRACE_FLAG  "[OBJECT]"

uint16_t numObjectInstances;                    //num of object instances
static void *objectDataList;                    //list of object local data
BITMAP *objectResources[E_OBJECTS_TYPE_NUM];    //array of objects gfx resources
SAMPLE *objectSfx[E_SFX_OBJECT_NUM];            //array of objects sfx resources
DATAFILE_INDEX *objectDataFileIndex;            //object datafile index

tVector objectExplosion;                        //position of a object explosion (dynamite...)
int8_t egyptPuzzle[3];

void object_system_init()
{
    //empty object list
    free(objectDataList);
    objectDataList = NULL;
    //set number of entities
    numObjectInstances = 0;    

    //create data file index
    objectDataFileIndex = create_dat_index("objects.dat");

    //load object sfx
    objectSfx[E_SFX_OBJECT_FULL_LIFE]   = load_dat_wav_indexed(objectDataFileIndex, POWERUP_WAV);
    objectSfx[E_SFX_OBJECT_EXTRA_LIVE]  = load_dat_wav_indexed(objectDataFileIndex, LIVE_WAV);
    
    egyptPuzzle[0] = 0;
    egyptPuzzle[1] = 0;
    egyptPuzzle[2] = 0;

    MY_TRACE_FLAG("Initialized object system\n");
}

void object_system_destroy()
{
    //empty object list
    free(objectDataList);
    objectDataList = NULL;
    //set number of entities
    numObjectInstances = 0;

    //free resources
    for (uint8_t i = 0; i < E_OBJECTS_TYPE_NUM; i++)
    {
        if (objectResources[i])
        {
            destroy_bitmap(objectResources[i]);
            objectResources[i] = NULL;
        }
    }

    //free samples
    for (uint8_t i = 0; i < E_SFX_OBJECT_NUM; i++)
    {
        if (objectSfx[i])
        {
            destroy_sample(objectSfx[i]);
            objectSfx[i] = NULL;
        }
    }

    destroy_dat_index(objectDataFileIndex);

    MY_TRACE_FLAG("Destroyed object system\n");
}

//check object entity type to add the local data structure to local data list and increases instances number
void object_create(tEntity *entity)
{
    //inc num instances
    numObjectInstances++;

    //allocate memory for general solid object
    objectDataList = realloc(objectDataList, numObjectInstances * sizeof(tSolidObjectLocalData)); 
    
    //set object properties    
    switch (entity->entType)
    {
        case E_ROCK_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, ROCK_BMP);
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){16, 16};
            entity->size = (tVector){16, 16};             
            collision_create_entity_points(entity);                  
        break;
        case E_ROCK_2_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, ROCK2_BMP);
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){16, 16};
            entity->size = (tVector){16, 16};      
            collision_create_entity_points(entity); 
        break;                   
        case E_ROCK_3_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, ROCK3_BMP);
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){17, 16};
            entity->size = (tVector){16, 16};      
            collision_create_entity_points(entity);     
        break;
        case E_EGG_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, EGG_BMP);
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){21, 16};
            entity->size = (tVector){21, 16};      
            collision_create_entity_points(entity);                    
            entity->properties = E_ENT_PROP_NO_BREAKABLE;
        break;
        case E_END_LEVEL_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, END_BMP);
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){27, 27};
            entity->size = (tVector){8, 8};      
            entity->properties = E_ENT_PROP_NO_COLLISION;
        break;
        case E_CHECKPOINT_OBJECT_TYPE:            
            entity->img = NULL;
            entity->size = (tVector){16, 36};      
            entity->properties = E_ENT_PROP_NO_COLLISION;
        break;
        case E_ITEM_OBJECT_TYPE:         
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, ITEMS_BMP);
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){16, 16};
            entity->size = (tVector){16, 16};      
            entity->properties = E_ENT_PROP_NO_COLLISION | E_ENT_PROP_PERSISTENT;
        break;
        case E_STOP_SCROLL_OBJECT_TYPE:            
            entity->img = NULL;
            entity->size = (tVector){16, 16};      
            entity->properties = E_ENT_PROP_NO_COLLISION;
        break;
        case E_BACKCOLOR_OBJECT_TYPE:            
            entity->img = NULL;
            entity->size = (tVector){16, 16};      
            entity->properties = E_ENT_PROP_NO_COLLISION;
        break;
        case E_WAGON_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, WAGON_BMP);
            load_entity_wav_resources(&objectSfx[E_SFX_WAGON], objectDataFileIndex, WAGON_WAV);
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){36, 27};            
            entity->size = (tVector){20, 12};                                     
            entity->properties =  E_ENT_PROP_PHYSICS_ON | E_ENT_PROP_NO_PICKABLE | E_ENT_PROP_NO_BREAKABLE;            
            entity->axis = E_ENT_AXIS_DOWN;
            collision_create_entity_points(entity);            
        break;
        case E_DYNAMITE_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, DYNAMITE_BMP);
            load_entity_wav_resources(&objectSfx[E_SFX_EXPLOSION], objectDataFileIndex, EXPLO_WAV);
            load_entity_wav_resources(&objectSfx[E_SFX_EXPLOSION_COUNTER], objectDataFileIndex, COUNTER_WAV);
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){16, 21};
            entity->size = (tVector){16, 16};
            entity->axis = E_ENT_AXIS_DOWN;             
            collision_create_entity_points(entity);    
            entity->properties =  E_ENT_PROP_NO_BREAKABLE;                          
        break;
        case E_BRIDGE_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, BRIDGE_BMP);
            load_entity_wav_resources(&objectSfx[E_SFX_OBJECT_FALL], objectDataFileIndex, ROCKFALL_WAV);
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){16, 16};            
            entity->size = (tVector){16, 16};                                     
            entity->properties =  E_ENT_PROP_NO_PICKABLE | E_ENT_PROP_NO_BREAKABLE;            
        break;
        case E_ROCK_EXPLOSION_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, ROCKEX_BMP);
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){16, 16};
            entity->size = (tVector){16, 16};             
            collision_create_entity_points(entity); 
            entity->properties =  E_ENT_PROP_NO_PICKABLE | E_ENT_PROP_NO_BREAKABLE;                 
        break;
        case E_GAME_OVER_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, GAMEOVER_BMP);
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){27, 40};
            entity->size = (tVector){27, 40};                           
        break;
        case E_ROCK_FALL_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, ROCKFALL_BMP);
            load_entity_wav_resources(&objectSfx[E_SFX_OBJECT_FALL], objectDataFileIndex, ROCKFALL_WAV);            
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){16, 21};
            entity->size = (tVector){16, 16};      
            entity->axis = E_ENT_AXIS_UP;       
            collision_create_entity_points(entity);
            entity->properties = E_ENT_PROP_NO_PICKABLE | E_ENT_PROP_NO_HURT | E_ENT_PROP_NO_COLLISION;
        break;
        case E_SPIKE_FALL_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, SPKFALL_BMP);
            load_entity_wav_resources(&objectSfx[E_SFX_OBJECT_FALL], objectDataFileIndex, ROCKFALL_WAV);            
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){16, 16};
            entity->size = (tVector){16, 16};      
            entity->axis = E_ENT_AXIS_UP;       
            collision_create_entity_points(entity);
            entity->properties = E_ENT_PROP_NO_PICKABLE | E_ENT_PROP_NO_HURT | E_ENT_PROP_NO_COLLISION;
        break;
        case E_BOX_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, BOX_BMP);
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){16, 16};
            entity->size = (tVector){16, 16};             
            collision_create_entity_points(entity);                  
            entity->properties = E_ENT_PROP_NO_BREAKABLE | E_ENT_PROP_PERSISTENT;
        break;
        case E_HIDDEN_OBJECT_TYPE:        
            entity->size = (tVector){16, 16};                         
            entity->properties = E_ENT_PROP_NO_COLLISION;            
        break;
        case E_QUICKSAND_OBJECT_TYPE:            
            entity->size = (tVector){128, 32};                                     
            entity->properties =  E_ENT_PROP_NO_PICKABLE | E_ENT_PROP_NO_BREAKABLE;            
            entity->axis = E_ENT_AXIS_DOWN;
            collision_create_entity_points(entity);            
        break;
        case E_TRAP_ARROW_OBJECT_TYPE:
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, OTRAPA_BMP);
            load_entity_wav_resources(&objectSfx[E_SFX_OBJECT_ARROW], objectDataFileIndex, TRAPA_WAV);
            entity->img = objectResources[entity->entType]; 
            entity->spriteSize = (tVector){16, 16};                          
            entity->size = (tVector){16, 16};                     
        break;
        case E_EGYPT_SYMBOL_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, SYMBOL1_BMP);
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){16, 16};
            entity->size = (tVector){16, 16};         
            collision_create_entity_points(entity);        
            entity->properties = E_ENT_PROP_NO_BREAKABLE | E_ENT_PROP_PERSISTENT;
        break;
        case E_SYMBOL_HOLE_OBJECT_TYPE:                              
            load_entity_wav_resources(&objectSfx[E_SFX_WAGON], objectDataFileIndex, WAGON_WAV);    
            load_entity_wav_resources(&objectSfx[E_SFX_PUZZLE_NO], objectDataFileIndex, PUZZLENO_WAV);    
            load_entity_wav_resources(&objectSfx[E_SFX_PUZZLE_OK], objectDataFileIndex, PUZZLEOK_WAV);    
            entity->size = (tVector){16, 16};                         
            entity->properties = E_ENT_PROP_NO_BREAKABLE | E_ENT_PROP_NO_PICKABLE;                              
            collision_create_entity_points(entity);        
        break;
        default:
            abort_on_error("Object entity type (%i) not valid", entity->entType);
        break;
    }
    
    //test memory allocation
    MY_ASSERT(objectDataList);

    //set actual instance num
    entity->entInstance = numObjectInstances - 1;  
    
    #if DEBUG_TRACE_CREATE_ENTITIES
        MY_TRACE_FLAG("Created object instance:%i\n", entity->entInstance);
    #endif
};

//calls specified object type update function
void object_update(tEntity *entity)
{   
    switch (entity->entType)
    {
        case E_END_LEVEL_OBJECT_TYPE:
        case E_CHECKPOINT_OBJECT_TYPE:
        case E_STOP_SCROLL_OBJECT_TYPE:
        case E_BACKCOLOR_OBJECT_TYPE:
        case E_SYMBOL_HOLE_OBJECT_TYPE:
            object_trigger_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;
        case E_ITEM_OBJECT_TYPE:
            object_item_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;
        case E_WAGON_OBJECT_TYPE:
            object_wagon_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;
        case E_DYNAMITE_OBJECT_TYPE:
            object_dynamite_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;
        case E_BRIDGE_OBJECT_TYPE:
            object_bridge_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;
        case E_ROCK_EXPLOSION_OBJECT_TYPE:
            object_rock_explosion_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;
        case E_GAME_OVER_OBJECT_TYPE:
            object_game_over_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;
        case E_ROCK_FALL_OBJECT_TYPE:
        case E_SPIKE_FALL_OBJECT_TYPE:
            object_fall_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;
        case E_QUICKSAND_OBJECT_TYPE:
            object_quick_sand_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;
        case E_TRAP_ARROW_OBJECT_TYPE:
            object_trap_arrow_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;                
        default:
            object_solid_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;
    }
}

//calls specified object type init function
void object_init(tEntity *entity)
{   
    MY_TRACE_FLAG("Init instance %i\n", entity->entInstance);

    switch (entity->entType)
    {       
        case E_SYMBOL_HOLE_OBJECT_TYPE:
            ((tSolidObjectLocalData*)objectDataList)[entity->entInstance].timer = 0;
            ((tSolidObjectLocalData*)objectDataList)[entity->entInstance].flag = 0;        
            for (uint8_t i; i < 7; i++)
            {
                map_change_tile((tVector){134, 7 - i}, 17, 0x00);    
            }
            egyptPuzzle[0] = 0;
            egyptPuzzle[1] = 0;
            egyptPuzzle[2] = 0;
        break;
        default:
            ((tSolidObjectLocalData*)objectDataList)[entity->entInstance].timer = 0;
            ((tSolidObjectLocalData*)objectDataList)[entity->entInstance].flag = 0;        
        break;        
    }
}

void object_destroy(tEntity *entity)
{
    uint16_t objectIndex;
    objectIndex = entity->entInstance;
    
    //copies last enemy to deleted enemy position
    ((tSolidObjectLocalData*)objectDataList)[objectIndex] = ((tSolidObjectLocalData*)objectDataList)[numObjectInstances - 1];
    
    //decrement entity number
    numObjectInstances--;
    if (numObjectInstances == 0)
    {
        //free entity list
        free(objectDataList);
        objectDataList = NULL;
    }
    else
        //reallocates the array with decremented entity number    
        //TODO: this is incorrect when entities has different local data structure
        objectDataList = realloc(objectDataList, numObjectInstances * sizeof(tSolidObjectLocalData));     

    MY_TRACE_FLAG("Destroyed object instance:%i\n", objectIndex);
}

void object_solid_update(tEntity *this, tSolidObjectLocalData *local)
{
    //object defines
    #define SOLID_PICKED_OFFSET_Y   20
    #define SOLID_PICKED_OFFSET_X   1

    //object states
    enum E_SOLID_OBJECT_STATES{E_SOLID_ST_IDLE, E_SOLID_ST_PICKED, E_SOLID_ST_THROWING, E_SOLID_ST_BREAK, E_SOLID_ST_CHECK_PUZZLE};
    
    if (this->signal == E_ENT_SIGNAL_HURT)
    {
        this->state = E_SOLID_ST_BREAK;
        this->signal = 0;
    }
    
    switch (this->state)
    {
        case E_SOLID_ST_IDLE:
            CLEAR_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            CLEAR_FLAG(this->properties, E_ENT_PROP_PHYSICS_ON);

            this->fixVel.x = 0;
            this->fixVel.y = 0;

            if (this->signal == E_ENT_SIGNAL_PICKING)
                this->state = E_SOLID_ST_PICKED;    
                
            if (this->entType == E_EGYPT_SYMBOL_OBJECT_TYPE)
                this->anim.frame = this->spare;
                
        break;
        case E_SOLID_ST_PICKED:
            SET_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            SET_FLAG(this->properties, E_ENT_PROP_PERSISTENT);
            
            //position the object relative to player
            tEntity *playerEnt = entity_get(entity_get_player_id());
            this->fixPos.x = playerEnt->dir ? playerEnt->fixPos.x + itofix(SOLID_PICKED_OFFSET_X) : playerEnt->fixPos.x - itofix(SOLID_PICKED_OFFSET_X);
            this->fixPos.y  = playerEnt->fixPos.y - itofix(SOLID_PICKED_OFFSET_Y);
            
            //reset puzzle data
            if (this->entType == E_EGYPT_SYMBOL_OBJECT_TYPE)
                 egyptPuzzle[this->spare] = 0;

            //check if receive throw signal
            if (this->signal == E_ENT_SIGNAL_THROW || this->signal == E_ENT_SIGNAL_SHORT_THROW)
            {
                CLEAR_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
                SET_FLAG(this->properties, E_ENT_PROP_PHYSICS_ON);
                //set throw velocities
                if (this->signal == E_ENT_SIGNAL_THROW)
                {
                    this->fixVel.x = playerEnt->dir == E_ENT_DIR_LEFT ? -itofix(OBJECT_THROW_VEL_X) : itofix(OBJECT_THROW_VEL_X);
                    this->fixVel.y = itofix(OBJECT_THROW_VEL_Y);
                }
                else
                {
                    //if short throw, position the object one tile far
                    this->fixPos.x += playerEnt->dir == E_ENT_DIR_LEFT ? -itofix(map_get_tile_size()) : itofix(map_get_tile_size());        
                }
                
                this->ground = false;
                
                this->state = E_SOLID_ST_THROWING;
            }
        break;
        case E_SOLID_ST_THROWING:       
            uint8_t colDir;
            this->ground = false;
            //check all the entity collision points    
            for (uint8_t i = 0; i < E_NUM_COL_POINTS; i++)
            {                
                //check collision tile for collision point
                colDir = collision_check_tile(this, i);
                if (CHECK_FLAG(this->properties, E_ENT_PROP_NO_BREAKABLE))
                    //apply collision direction
                    collision_apply_dir(this, colDir, E_COLLISION_BOUNCE_SOFT);                        
                else
                {
                    if (colDir)
                        this->state = E_SOLID_ST_BREAK;
                }
            }
            
            //check entities collisions            
            uint8_t numEntities = entities_get_num();
            tEntity *checkEntity;
            for (uint8_t i = 0; i < numEntities; i++)
            {
                checkEntity = entity_get(i);
                if (checkEntity->id != this->id && checkEntity->id != entity_get_player_id() && !checkEntity->dead)
                {
                    switch (checkEntity->entClass)
                    {
                        case E_ENT_CLASS_OBJECT:
                        case E_ENT_CLASS_PLATFORM:
                        case E_ENT_CLASS_TRIGGER:
                            colDir = collision_check_entity(this, checkEntity, E_CHECK_PROCESS_BOTHAXIS);
                            if (CHECK_FLAG(this->properties, E_ENT_PROP_NO_BREAKABLE))
                            {
                                if (this->entType == E_EGYPT_SYMBOL_OBJECT_TYPE && checkEntity->entClass == E_ENT_CLASS_TRIGGER && checkEntity->entType == E_SYMBOL_HOLE_OBJECT_TYPE && colDir)
                                {
                                    if (this->spare == checkEntity->spare)
                                        egyptPuzzle[this->spare] = 1;                            
                                    else
                                        egyptPuzzle[this->spare] = -1;
                                }
                                collision_apply_dir(this, colDir, E_COLLISION_NO_BOUNCE);
                            }
                            else
                            {
                                if (colDir)
                                    this->state = E_SOLID_ST_BREAK;       
                            }
                        break;
                        case E_ENT_CLASS_ENEMY:
                            colDir = collision_check_entity(this, checkEntity, E_CHECK_PROCESS_INFOONLY);
                            if (colDir)
                            {
                                //send signal to entity
                                checkEntity->signal = E_ENT_SIGNAL_HURT;                                
                                if (!CHECK_FLAG(this->properties, E_ENT_PROP_NO_BREAKABLE))
                                    //change state
                                    this->state = E_SOLID_ST_BREAK;                                 
                            }
                        break;
                    }            
                }
            }

            if (this->ground && abs(this->fixVel.x) < ftofix(0.1))
                this->state = E_SOLID_ST_IDLE;            
        break;
        case E_SOLID_ST_BREAK:
            //stop object
            this->fixVel.x = 0;
            this->fixVel.y = 0;                                            
            CLEAR_FLAG(this->properties, E_ENT_PROP_PHYSICS_ON);
            SET_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            CLEAR_FLAG(this->properties, E_ENT_PROP_PERSISTENT);
            //play break animation
            if (play_animation(&this->anim, ANIM_OBJECT_BREAK))
            {
                //put object to sleep
                //this->sleep = true;                
                this->dead = true;
            }
        break;        
    }
}

void object_item_update(tEntity *this, tSolidObjectLocalData *local)
{
    //object states
    enum E_ITEM_STATE {E_ITEM_ST_IDLE};

    //object animations
    #define ANIM_ITEM_EXTRA_LIFE   0,   0, 10,  ANIM_LOOP
    #define ANIM_ITEM_FULL_LIFE    1,   1, 10,  ANIM_LOOP

    switch (this->state)
    {
        case E_ITEM_ST_IDLE:
            switch (this->spare)
            {
                case E_ITEM_EXTRA_LIFE:
                    SET_FLAG(this->properties, E_ENT_PROP_NO_SPAWN);
                    
                    //if collision with player
                    if (collision_check_entity(this, entity_get(entity_get_player_id()), E_CHECK_PROCESS_INFOONLY))
                    {
                        game.lives++;
                        sfx_play(objectSfx[E_SFX_OBJECT_EXTRA_LIVE], E_SFX_OBJECT_VOICE);
                        this->dead = true;
                    }

                    play_animation(&this->anim, ANIM_ITEM_EXTRA_LIFE);
                break;
                case E_ITEM_FULL_LIFE:
                    //if collision with player
                    if (collision_check_entity(this, entity_get(entity_get_player_id()), E_CHECK_PROCESS_INFOONLY))
                    {
                        if (game.life < GAME_INI_LIFE)
                            game.life = GAME_INI_LIFE;
                        else
                            game.score += SCORE_POINT_FULL_LIFE;

                        sfx_play(objectSfx[E_SFX_OBJECT_FULL_LIFE], E_SFX_OBJECT_VOICE);
                        this->dead = true;
                    }

                    play_animation(&this->anim, ANIM_ITEM_FULL_LIFE);
                break;
            }
        break;        
        default:
            this->state = E_ITEM_ST_IDLE;
    }
}

void object_trigger_update(tEntity *this, tSolidObjectLocalData *local)
{
    switch (this->entType)
    {
        case E_CHECKPOINT_OBJECT_TYPE:
            //if the object is in scroll range, sets the initial position of the player for checkpoint spawn
            tEntity *player = entity_get(entity_get_player_id());

            if ( (player->pos.x >= this->pos.x && this->dir == E_ENT_DIR_RIGHT) ||
                 (player->pos.x <= this->pos.x && this->dir == E_ENT_DIR_LEFT))
            {
                player->initPos = this->pos;
                player->initDir = this->dir;            
            }
        break;
        case E_END_LEVEL_OBJECT_TYPE:
            #define END_LEVEL_ANIM  0,   7,  14, ANIM_LOOP

            //if collision with player
            if (collision_check_entity(this, entity_get(entity_get_player_id()), E_CHECK_PROCESS_INFOONLY))
            {
                game.levelComplete[game.actualLevel] = true;
            }

            play_animation(&this->anim, END_LEVEL_ANIM);
        break;
        //stop scroll trigger
        case E_STOP_SCROLL_OBJECT_TYPE:
            if (CHECK_FLAG(this->spare, E_STOP_SCROLL_LEFT))
            {
                if (entity_get(entity_get_player_id())->pos.x > this->pos.x)    
                {
                    scroll_stop_scroll(E_STOP_SCROLL_LEFT, this->pos.x);
                    //MY_TRACE_FLAG("Entity %i set scroll stop left\n", this->id);
                }
            }
            if (CHECK_FLAG(this->spare, E_STOP_SCROLL_RIGHT))
            {
                if (entity_get(entity_get_player_id())->pos.x < this->pos.x)
                {
                    scroll_stop_scroll(E_STOP_SCROLL_RIGHT, this->pos.x);
                    //MY_TRACE_FLAG("Entity %i set scroll stop right\n", this->id);
                }
            }
            if (CHECK_FLAG(this->spare, E_STOP_SCROLL_DOWN))
            {                
                scroll_stop_scroll(E_STOP_SCROLL_DOWN, this->pos.y);
                //MY_TRACE_FLAG("Entity %i set scroll stop down\n", this->id);                
            }   
            if (CHECK_FLAG(this->spare, E_STOP_SCROLL_UP))
            {                
                scroll_stop_scroll(E_STOP_SCROLL_UP, this->pos.y);
                //MY_TRACE_FLAG("Entity %i set scroll stop up\n", this->id);                
            }
        break;
        case E_BACKCOLOR_OBJECT_TYPE:
            map_change_background_color(this->spare);
        break;
        case E_SYMBOL_HOLE_OBJECT_TYPE:
            //object states
            enum E_WAGON_OBJECT_STATES{E_SYMBOL_HOLE_ST_IDLE, E_SYMBOL_HOLE_ST_OK, E_SYMBOL_HOLE_ST_OPEN_DOOR};    

            switch (this->state)
            {
                case E_SYMBOL_HOLE_ST_IDLE:
                    show_debug("puzzle %i:%i", this->spare,egyptPuzzle[this->spare]);
                    //check puzzle completed
                    if (egyptPuzzle[0] == 1 && egyptPuzzle[1] == 1 && egyptPuzzle[2] == 1)
                    {
                        this->state = E_SYMBOL_HOLE_ST_OK;    
                        sfx_play(objectSfx[E_SFX_PUZZLE_OK], E_SFX_OBJECT_VOICE);                    
                    }
                    else if (egyptPuzzle[0] != 0 && egyptPuzzle[1] != 0 && egyptPuzzle[2] != 0)
                    {                                    
                        sfx_play(objectSfx[E_SFX_PUZZLE_NO], E_SFX_OBJECT_VOICE);                                    
                        scroll_shake_camera();                                    
                        egyptPuzzle[0] = egyptPuzzle[0] == 1 ? 1 : 0;
                        egyptPuzzle[1] = egyptPuzzle[1] == 1 ? 1 : 0;
                        egyptPuzzle[2] = egyptPuzzle[2] == 1 ? 1 : 0;
                    }
                break;                
                case E_SYMBOL_HOLE_ST_OK:
                    if (local->timer >= 80)
                    {
                        local->timer = 0;
                        this->state++;
                    }
                    else
                        local->timer += clock_tick_get();
                break;
                case E_SYMBOL_HOLE_ST_OPEN_DOOR:
                    if (clock_counter_check(20))
                    {
                        sfx_play(objectSfx[E_SFX_WAGON], E_SFX_OBJECT_VOICE);
                        map_change_tile((tVector){134, 7 - local->timer}, 131, E_TILE_PROP_NO_SOLID);
                        local->timer++;
                    }

                    if (local->timer >= 7)
                        this->state++;
                break;
            }
        break;
    }
}

void object_wagon_update(tEntity *this, tSolidObjectLocalData *local)
{
    //object defines
    #define WAGON_VELOCITY              0.9
    #define WAGON_SOUND_CADENCE         20
    //#define WAGON_BORDER_OBJ_OFFSET_X   18
    //#define WAGON_BORDER_OBJ_OFFSET_Y   16

    //object states
    enum E_WAGON_OBJECT_STATES{E_WAGON_ST_IDLE, E_WAGON_ST_MOVE};

    //object animations
    #define ANIM_WAGON_IDLE   0,   0, 10,  ANIM_ONCE
    #define ANIM_WAGON_MOVE   0,   3, 10,  ANIM_LOOP
    
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

    switch (this->state)
    {
        case E_WAGON_ST_IDLE:
            
            //reset velocity
            this->fixVel.x = itofix(0);    

            play_animation(&this->anim, ANIM_WAGON_IDLE);

            //if player on this platform            
            if (collision_get_player_platform_id() == this->id)
            {
                this->state++;

                //creation of wagon border object
                //can write on local flag the entity id because when create, the local data array changes is pointer until next frame
                //instead, save the next instance object number
                //local->flag = numObjectInstances;
                //entity_create(E_ENT_CLASS_OBJECT, E_HIDDEN_OBJECT_TYPE, (tVector){this->pos.x + WAGON_BORDER_OBJ_OFFSET_X, this->pos.y - WAGON_BORDER_OBJ_OFFSET_Y}, this->dir, this->spare);                                                
                
            }
        break;
        case E_WAGON_ST_MOVE:
            //apply linear wagon velocity
            this->fixVel.x = ftofix(WAGON_VELOCITY);

            //calculate next wagon integer position (entity update do this)
            int16_t nextPosX = fixtoi(this->fixPos.x + fixmul(this->fixVel.x, ftofix(deltaTime)));
            int16_t nextPosY;
            if (this->ground)
                nextPosY = fixtoi(this->fixPos.y + fixmul(this->fixVel.y, ftofix(deltaTime)));
            else{
                nextPosY = fixtoi(this->fixPos.y + fixmul(this->fixVel.y, ftofix(deltaTime)) + fixmul(ftofix(ENTITY_GRAVITY), ftofix(deltaTime))) + 1;                
            }

            //only move player if collided
            if (collision_get_player_platform_id() == this->id)
            {
                //adds to player x position the integer part of platform delta movement
                entity_get(entity_get_player_id())->fixPos.x += itofix(nextPosX - this->pos.x);
                entity_get(entity_get_player_id())->fixPos.y += itofix((nextPosY - this->pos.y));                
            }

            //move border object
            /*
            tEntity *borderObj;            
            borderObj = entity_get_by_instance(E_ENT_CLASS_OBJECT, local->flag);
            if (borderObj)
            {
                borderObj->fixPos.x = this->fixPos.x + itofix(WAGON_BORDER_OBJ_OFFSET_X);
                borderObj->fixPos.y = this->fixPos.y - itofix(WAGON_BORDER_OBJ_OFFSET_Y);            
            }
            */
            
            play_animation(&this->anim, ANIM_WAGON_MOVE);

            //play wagon sound
            if (clock_counter_check(WAGON_SOUND_CADENCE) && this->ground)
                sfx_play(objectSfx[E_SFX_WAGON], E_SFX_OBJECT_VOICE);

        break;
    }
    
}

void object_dynamite_update(tEntity *this, tSolidObjectLocalData *local)
{
    //object defines
    #define DYNAMITE_PICKED_OFFSET_Y   20
    #define DYNAMITE_PICKED_OFFSET_X    1
    #define DYNAMITE_TILE_RANGE_X       4
    #define DYNAMITE_TILE_RANGE_Y       4
    #define DYNAMITE_TIMER_EXPLOSION    4 //seconds
    
    //object animations
    #define ANIM_DYNAMITE_IDLE          0,   0, 10,  ANIM_LOOP
    #define ANIM_DYNAMITE_EXPLOSION     1,   2, 10,  ANIM_ONCE

    //object states
    enum E_DYNAMITE_OBJECT_STATES{E_DYNAMITE_ST_IDLE, E_DYNAMITE_ST_PICKED, E_DYNAMITE_ST_THROWING, E_DYNAMITE_ST_EXPLODE};
    
    //get player entity
    tEntity *playerEnt = entity_get(entity_get_player_id());

    if (this->signal == E_ENT_SIGNAL_HURT)
    {
        this->state = E_DYNAMITE_ST_EXPLODE;
        this->signal = 0;
    }

    //if picked dynamite
    if (local->flag && this->state != E_DYNAMITE_ST_EXPLODE)
    {        
        if (local->timer >= DYNAMITE_TIMER_EXPLOSION)
        {
            //explode dynamite
            this->state = E_DYNAMITE_ST_EXPLODE;
            sfx_play(objectSfx[E_SFX_EXPLOSION], E_SFX_OBJECT_VOICE);  
            scroll_shake_camera();
            //hurt player if on explosion region
            if (in_range_vector(playerEnt->pos, this->pos, (tVector){(DYNAMITE_TILE_RANGE_X * map_get_tile_size()), (DYNAMITE_TILE_RANGE_X * map_get_tile_size())}))
                playerEnt->signal = E_ENT_SIGNAL_HURT;
        } 
        else
        {
            //add timer and play counter sfx
            if (clock_tick_1sec_get())
            {
                local->timer++;                
                sfx_play(objectSfx[E_SFX_EXPLOSION_COUNTER], E_SFX_OBJECT_VOICE);
            }
        }
    }

    switch (this->state)
    {
        case E_DYNAMITE_ST_IDLE:
            CLEAR_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            CLEAR_FLAG(this->properties, E_ENT_PROP_PHYSICS_ON);

            this->fixVel.x = 0;
            this->fixVel.y = 0;

            if (this->signal == E_ENT_SIGNAL_PICKING)
                this->state = E_DYNAMITE_ST_PICKED;  
        break;
        case E_DYNAMITE_ST_PICKED:
            SET_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            SET_FLAG(this->properties, E_ENT_PROP_PERSISTENT);
            
            //set flag to start explosion counter
            local->flag = true;

            //position the object relative to player
            tEntity *playerEnt = entity_get(entity_get_player_id());
            this->fixPos.x = playerEnt->dir ? playerEnt->fixPos.x + itofix(DYNAMITE_PICKED_OFFSET_X) : playerEnt->fixPos.x - itofix(DYNAMITE_PICKED_OFFSET_X);
            this->fixPos.y  = playerEnt->fixPos.y - itofix(DYNAMITE_PICKED_OFFSET_Y);
            
            //check if receive throw signal
            if (this->signal == E_ENT_SIGNAL_THROW || this->signal == E_ENT_SIGNAL_SHORT_THROW)
            {
                CLEAR_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
                SET_FLAG(this->properties, E_ENT_PROP_PHYSICS_ON);
                
                //set throw velocities
                if (this->signal == E_ENT_SIGNAL_THROW)
                {
                    this->fixVel.x = playerEnt->dir == E_ENT_DIR_LEFT ? -itofix(OBJECT_THROW_VEL_X) : itofix(OBJECT_THROW_VEL_X);
                    this->fixVel.y = itofix(OBJECT_THROW_VEL_Y);
                }
                else
                {
                    //short throw positions the object 1 tile far 
                    this->fixPos.x += playerEnt->dir == E_ENT_DIR_LEFT ? -itofix(map_get_tile_size()) : itofix(map_get_tile_size());        
                }
                
                this->ground = false;
                this->state = E_DYNAMITE_ST_THROWING;
            }
        break;
        case E_DYNAMITE_ST_THROWING:                        
            uint8_t colDir;
            this->ground = false;
            //check all the entity collision points    
            for (uint8_t i = 0; i < E_NUM_COL_POINTS; i++)
            {                
                //check collision tile for collision point
                colDir = collision_check_tile(this, i);
                if (CHECK_FLAG(this->properties, E_ENT_PROP_NO_BREAKABLE))
                    //apply collision direction
                    collision_apply_dir(this, colDir, E_COLLISION_BOUNCE_SOFT);                        
                else
                {
                    if (colDir)
                        this->state = E_DYNAMITE_ST_EXPLODE;
                }
            }
            
            //check entities collisions            
            uint8_t numEntities = entities_get_num();
            tEntity *checkEntity;
            for (uint8_t i = 0; i < numEntities; i++)
            {
                checkEntity = entity_get(i);
                if (checkEntity->id != this->id && checkEntity->id != entity_get_player_id() && !checkEntity->dead)
                {
                    switch (checkEntity->entClass)
                    {
                        case E_ENT_CLASS_OBJECT:
                            colDir = collision_check_entity(this, checkEntity, E_CHECK_PROCESS_BOTHAXIS);
                            if (CHECK_FLAG(this->properties, E_ENT_PROP_NO_BREAKABLE))
                                collision_apply_dir(this, colDir, E_COLLISION_NO_BOUNCE);
                            else
                            {
                                if (colDir)
                                    this->state = E_DYNAMITE_ST_EXPLODE;       
                            }
                        break;
                    }            
                }
            }

            if (this->ground && abs(this->fixVel.x) < ftofix(0.1))
                this->state = E_DYNAMITE_ST_IDLE;
        break;
        case E_DYNAMITE_ST_EXPLODE:
            //stop object
            this->fixVel.x = 0;
            this->fixVel.y = 0;                                            
            CLEAR_FLAG(this->properties, E_ENT_PROP_PHYSICS_ON);
            SET_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            CLEAR_FLAG(this->properties, E_ENT_PROP_PERSISTENT);

            objectExplosion = this->pos;

            //play break animation
            if (play_animation(&this->anim, ANIM_DYNAMITE_EXPLOSION))
            {
                this->dead = true;
                objectExplosion = (tVector){0, 0};                
            }
        break;
    }
}

void object_bridge_update(tEntity *this, tSolidObjectLocalData *local)
{
    //object defines
    #define BRIDGE_WAIT_TO_FALL     20
    #define BRIDGE_FALL_VEL_Y       1.8
    
    //object states
    enum E_BRIDGE_OBJECT_STATES{E_BRIDGE_ST_IDLE, E_BRIDGE_ST_FALL};

    this->anim.frame = 0;

    if (this->signal == E_ENT_SIGNAL_AWAKE)
    {
        this->signal = E_ENT_SIGNAL_NONE;
        this->pos = this->initPos;
        this->state = 0;
    }

    switch (this->state)
    {
        case E_BRIDGE_ST_IDLE:
            this->fixVel.y = 0;

            //if player on this platform            
            if (collision_get_player_platform_id() == this->id)
                local->flag = true;

            if (local->flag)
            {
                if (local->timer >= BRIDGE_WAIT_TO_FALL)
                {
                    this->state++;
                    sfx_play(objectSfx[E_SFX_OBJECT_FALL], E_SFX_OBJECT_VOICE);
                    local->timer = 0;
                    local->flag = 0;
                }
                else
                {    
                    local->timer += clock_tick_get();
                }
            }
        break;
        case E_BRIDGE_ST_FALL:
            this->fixVel.y = ftofix(BRIDGE_FALL_VEL_Y);
            
            //calculate next integer position (entity update do this)
            //int16_t nextPosX = fixtoi(this->fixPos.x + fixmul(this->fixVel.x, ftofix(deltaTime)));
            int16_t nextPosY = fixtoi(this->fixPos.y + fixmul(this->fixVel.y, ftofix(deltaTime)));

            //only move player if collided
            if (collision_get_player_platform_id() == this->id)
            {
                //adds to player x position the integer part of platform delta movement
                //entity_get(entity_get_player_id())->fixPos.x += itofix(nextPosX - this->pos.x);
                entity_get(entity_get_player_id())->fixPos.y += itofix((nextPosY - this->pos.y) + 1); //add 1 to position to ensue player on platform on next frame                
            }
        break;
    }
}

void object_rock_explosion_update(tEntity *this, tSolidObjectLocalData *local)
{
    //object animations
    #define ANIM_ROCK_EXPLOSION     1,   2, 10,  ANIM_ONCE

    //object states
    enum E_ROCK_EXPLOSION_OBJECT_STATES{E_ROCK_EXPLOSION_ST_IDLE, E_ROCK_EXPLOSION_ST_EXPLOSION};

    switch (this->state)
    {
        case E_ROCK_EXPLOSION_ST_IDLE:
            this->anim.frame = 0;
            
            //check if there's some explosion on object range position
            if (in_range_vector(this->pos, objectExplosion, (tVector){(DYNAMITE_TILE_RANGE_X * map_get_tile_size()), (DYNAMITE_TILE_RANGE_X * map_get_tile_size())}))
                this->state++;
        break; 
        case E_ROCK_EXPLOSION_ST_EXPLOSION:
            //play break animation
            if (play_animation(&this->anim, ANIM_ROCK_EXPLOSION))
            {
                this->dead = true;                
            }    
        break;       
    }
}


void object_fall_update(tEntity *this, tSolidObjectLocalData *local)
{
    //object defines
    #define OBJECT_FALL_FALL_VEL_Y       3
    #define OBJECT_FALL_PLAYER_RANGE_X   30
    
    //object states
    enum E_OBJECT_FALL_OBJECT_STATES{E_OBJECT_FALL_ST_IDLE, E_OBJECT_FALL_ST_FALL, E_OBJECT_FALL_ST_BREAK};
    
    //object animations
    #define ANIM_OBJECT_FALL_BREAK                1,  2, 10, ANIM_ONCE

    //get player
    tEntity *player = entity_get(entity_get_player_id());

    switch (this->state)
    {
        case E_OBJECT_FALL_ST_IDLE:
            this->fixVel.y = 0;

            //check player range            
            if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, this->spare != 0 ? this->spare : OBJECT_FALL_PLAYER_RANGE_X))
            {
                this->state++;
                sfx_play(objectSfx[E_SFX_OBJECT_FALL], E_SFX_OBJECT_VOICE);
            }
            
            this->anim.frame = 0;        
        break;
        case E_OBJECT_FALL_ST_FALL:
            this->fixVel.y = ftofix(OBJECT_FALL_FALL_VEL_Y);
            
            this->ground = false;            
            //check only down point    
            if (collision_check_tile(this, E_COLPOINT_DOWN_L) || collision_check_tile(this, E_COLPOINT_DOWN_R))            
                this->state = E_OBJECT_FALL_ST_BREAK;            
            else if (collision_check_entity(this, player, E_CHECK_PROCESS_INFOONLY))
            {
                //hurt player if collided
                player->signal = E_ENT_SIGNAL_HURT;
                this->state = E_OBJECT_FALL_ST_BREAK;
            }
        break;
        case E_OBJECT_FALL_ST_BREAK:
            //stop object
            this->fixVel.x = 0;
            this->fixVel.y = 0;                                                        
            //play break animation
            if (play_animation(&this->anim, ANIM_OBJECT_BREAK))
            {
                this->dead = true;
            }
        break;
    }
}

void object_quick_sand_update(tEntity *this, tSolidObjectLocalData *local)
{
    //object defines
    #define QUICK_SAND_VELOCITY              0.36
    
    //object states
    enum E_QUICKSAND_OBJECT_STATES{E_QUICKSAND_ST_IDLE, E_QUICKSAND_ST_MOVE};

    switch (this->state)
    {
        case E_QUICKSAND_ST_IDLE:
            
            //reset velocity
            this->fixVel.y = itofix(0); 

            //if player on this platform            
            if (collision_get_player_platform_id() == this->id)
            {
                this->state++;                
            }
        break;
        case E_QUICKSAND_ST_MOVE:
            //only move player if collided
            if (collision_get_player_platform_id() == this->id)
            {
                //apply linear velocity
                this->fixVel.y = ftofix(QUICK_SAND_VELOCITY);

                //calculate next integer position (entity update do this)
                int16_t nextPosY = fixtoi(this->fixPos.y + fixmul(this->fixVel.y, ftofix(deltaTime)));

                //adds to player x position the integer part of platform delta movement
                entity_get(entity_get_player_id())->fixPos.y += itofix((nextPosY - this->pos.y) + 1);
            }
            else{
                //restore platform position
                this->fixVel.y = -ftofix(QUICK_SAND_VELOCITY);
                //return to idle when reach position
                if (this->pos.y <= this->initPos.y)
                    this->state = E_QUICKSAND_ST_IDLE;     
            }
        break;
    }    
}

void object_trap_arrow_update(tEntity *this, tSolidObjectLocalData *local)
{
    //object defines
    #define TRAP_ARROW_TIMER    200   

    //object animations
    #define ANIM_TRAP_ARROW_IDLE_FRAME   0
    #define ANIM_TRAP_ARROW_SHOOT        1,   1, 20,  ANIM_ONCE
    
    //object states
    enum E_TRAP_ARROW_OBJECT_STATES{E_TRAP_ARROW_ST_IDLE, E_TRAP_ARROW_ST_SHOOT};

    switch (this->state)
    {
        case E_TRAP_ARROW_ST_IDLE:
            if (local->timer >= TRAP_ARROW_TIMER)
            {
                this->state++;                
                local->timer = 0;
                local->flag = false;
            }
            else 
                local->timer += clock_tick_get();
            
            this->anim.frame = ANIM_TRAP_ARROW_IDLE_FRAME;

        break;
        case E_TRAP_ARROW_ST_SHOOT:
            //TODO: after entity create must not modify any entity structure data in case pointer moves!            
            if (play_animation(&this->anim, ANIM_TRAP_ARROW_SHOOT))
            {
                this->state--;
            }    
            
            if (!local->flag)
            {
                local->flag = true; //it's important to set the local flag before entity creation in case pointer moves
                sfx_play(objectSfx[E_SFX_OBJECT_ARROW], E_SFX_OBJECT_VOICE);
                entity_create(E_ENT_CLASS_ENEMY, E_TRAP_ARROW_ENEMY_TYPE, (tVector){this->pos.x, this->pos.y + 8}, this->dir, this->spare);
            }
        break;
    }    
}

void object_game_over_update(tEntity *this, tSolidObjectLocalData *local)
{
    //object animation
    #define ANIM_GAME_OVER     0,   9, 10,  ANIM_LOOP
    
    play_animation(&this->anim, ANIM_GAME_OVER);    
    
}

void object_trace(tEntity *this)
{
    MY_TRACE_FLAG("Object Instance: %d\n\tObj Type:%d\n", this->entInstance, this->entType);
}