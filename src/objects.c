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

#include "data/odata.h"

#define TRACE_FLAG  "[OBJECT]"

uint16_t numObjectInstances;        //num of object instances
static void *objectDataList;        //list of object local data
BITMAP *objectResources[E_OBJECTS_TYPE_NUM];
SAMPLE *objectSfx[E_SFX_OBJECT_NUM];
DATAFILE_INDEX *objectDataFileIndex;

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
    objectSfx[E_SFX_OBJECT_FULL_LIFE]  = load_dat_wav_indexed(objectDataFileIndex, POWERUP_WAV);
    objectSfx[E_SFX_OBJECT_EXTRA_LIVE] = load_dat_wav_indexed(objectDataFileIndex, LIVE_WAV);

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
            if (!objectResources[E_ROCK_OBJECT_TYPE])
                objectResources[E_ROCK_OBJECT_TYPE] = load_dat_bmp_indexed(objectDataFileIndex, ROCK_BMP);

            entity->img = objectResources[E_ROCK_OBJECT_TYPE];
            entity->spriteSize = (tVector){16, 16};
            entity->size = (tVector){16, 16};             
            collision_create_entity_points(entity);                  
        break;
        case E_ROCK_2_OBJECT_TYPE:            
            if (!objectResources[E_ROCK_2_OBJECT_TYPE])
                objectResources[E_ROCK_2_OBJECT_TYPE] = load_dat_bmp_indexed(objectDataFileIndex, ROCK2_BMP);

            entity->img = objectResources[E_ROCK_2_OBJECT_TYPE];
            entity->spriteSize = (tVector){16, 16};
            entity->size = (tVector){16, 16};      
            collision_create_entity_points(entity);                    
        break;
        case E_EGG_OBJECT_TYPE:            
            if (!objectResources[E_EGG_OBJECT_TYPE])
                objectResources[E_EGG_OBJECT_TYPE] = load_dat_bmp_indexed(objectDataFileIndex, EGG_BMP);

            entity->img = objectResources[E_EGG_OBJECT_TYPE];
            entity->spriteSize = (tVector){21, 16};
            entity->size = (tVector){21, 16};      
            collision_create_entity_points(entity);                    
            entity->properties = E_ENT_PROP_NO_BREAKABLE;
        break;
        case E_END_OBJECT_TYPE:            
            if (!objectResources[E_END_OBJECT_TYPE])
                objectResources[E_END_OBJECT_TYPE] = load_dat_bmp_indexed(objectDataFileIndex, END_BMP);

            entity->img = objectResources[E_END_OBJECT_TYPE];
            entity->spriteSize = (tVector){16, 16};
            entity->size = (tVector){8, 8};      
            entity->properties = E_ENT_PROP_NO_COLLISION;
        break;
        case E_CHECKPOINT_OBJECT_TYPE:            
            entity->img = NULL;
            entity->size = (tVector){16, 36};      
            entity->properties = E_ENT_PROP_NO_COLLISION;
        break;
        case E_ITEM_OBJECT_TYPE:         
            if (!objectResources[E_ITEM_OBJECT_TYPE])
                objectResources[E_ITEM_OBJECT_TYPE] = load_dat_bmp_indexed(objectDataFileIndex, ITEMS_BMP);

            entity->img = objectResources[E_ITEM_OBJECT_TYPE];
            entity->spriteSize = (tVector){16, 16};
            entity->size = (tVector){16, 16};      
            entity->properties = E_ENT_PROP_NO_COLLISION | E_ENT_PROP_PERSISTENT | E_ENT_PROP_NO_SPAWN;
        break;
        case E_STOP_SCROLL_OBJECT_TYPE:            
            entity->img = NULL;
            entity->size = (tVector){16, 16};      
            entity->properties = E_ENT_PROP_NO_COLLISION;
        break;
        default:
            abort_on_error("Object entity type not valid");
        break;
    }
    
    //test memory allocation
    MY_ASSERT(objectDataList);

    //set actual instance num
    entity->entInstance = numObjectInstances - 1;  
    
    MY_TRACE_FLAG("Created object instance:%i\n", entity->entInstance);
};

//calls specified object type update function
void object_update(tEntity *entity)
{   
    switch (entity->entType)
    {
        case E_END_OBJECT_TYPE:
            object_end_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;
        case E_CHECKPOINT_OBJECT_TYPE:
            object_checkpoint_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;
        case E_ITEM_OBJECT_TYPE:
            object_item_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;
        case E_STOP_SCROLL_OBJECT_TYPE:
            object_trigger_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;
        default:
            object_solid_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;
    }
}

//calls specified object type init function
void object_init(tEntity *entity)
{   
    switch (entity->entType)
    {        
        default:
            ((tSolidObjectLocalData*)objectDataList)[numObjectInstances - 1].timer = 0;
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
    #define SOLID_THROW_VEL_X       2
    #define SOLID_THROW_VEL_Y       -2
    #define SOLID_PICKED_OFFSET_Y   20
    #define SOLID_PICKED_OFFSET_X   1

    //object states
    enum E_SOLID_OBJECT_STATES{E_SOLID_ST_IDLE, E_SOLID_ST_PICKED, E_SOLID_ST_THROWING, E_SOLID_ST_BREAK};
    
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
        break;
        case E_SOLID_ST_PICKED:
            SET_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            SET_FLAG(this->properties, E_ENT_PROP_PERSISTENT);
            
            //position the object relative to player
            tEntity *playerEnt = entity_get(PLAYER_ENTITY_ID);
            this->fixPos.x = playerEnt->dir ? playerEnt->fixPos.x + itofix(SOLID_PICKED_OFFSET_X) : playerEnt->fixPos.x - itofix(SOLID_PICKED_OFFSET_X);
            this->fixPos.y  = playerEnt->fixPos.y - itofix(SOLID_PICKED_OFFSET_Y);
            
            //check if receive throw signal
            if (this->signal == E_ENT_SIGNAL_THROW)
            {
                CLEAR_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
                SET_FLAG(this->properties, E_ENT_PROP_PHYSICS_ON);
                //set throw velocities
                this->fixVel.x = playerEnt->dir == E_ENT_DIR_LEFT ? itofix(-SOLID_THROW_VEL_X) : itofix(SOLID_THROW_VEL_X);
                this->fixVel.y = itofix(SOLID_THROW_VEL_Y);
                this->ground = false;
                
                this->state = E_SOLID_ST_THROWING;
            }
        break;
        case E_SOLID_ST_THROWING:            
            CLEAR_FLAG(this->properties, E_ENT_PROP_PERSISTENT);
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
                if (checkEntity->id != this->id && checkEntity->id != PLAYER_ENTITY_ID && !checkEntity->dead)
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
                    //if collision with player
                    if (collision_check_entity(this, entity_get(PLAYER_ENTITY_ID), E_CHECK_PROCESS_INFOONLY))
                    {
                        game.lives++;
                        sfx_play(objectSfx[E_SFX_OBJECT_EXTRA_LIVE], E_SFX_OBJECT_VOICE);
                        this->dead = true;
                    }

                    play_animation(&this->anim, ANIM_ITEM_EXTRA_LIFE);
                break;
                case E_ITEM_FULL_LIFE:
                    //if collision with player
                    if (collision_check_entity(this, entity_get(PLAYER_ENTITY_ID), E_CHECK_PROCESS_INFOONLY))
                    {
                        game.life = GAME_INI_LIFE;
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

void object_end_update(tEntity *this, tSolidObjectLocalData *local)
{
    //object states
    enum E_END_STATE {E_END_ST_IDLE};

    switch (this->state)
    {
        case E_END_ST_IDLE:

            //if collision with player
            if (collision_check_entity(this, entity_get(PLAYER_ENTITY_ID), E_CHECK_PROCESS_INFOONLY))
            {
                game.levelComplete = true;
            }

            this->anim.frame = 0;
        break;        
        default:
            this->state = E_END_ST_IDLE;
    }
}

void object_checkpoint_update(tEntity *this, tSolidObjectLocalData *local)
{
    //object states
    enum E_CHECKPOINT_STATE {E_CHECKPOINT_ST_IDLE};

    switch (this->state)
    {
        case E_CHECKPOINT_ST_IDLE:
            //if the object is in scroll range, sets the initial position of the player for checkpoint spawn
            entity_get(PLAYER_ENTITY_ID)->initPos = this->pos;
            entity_get(PLAYER_ENTITY_ID)->initDir = this->dir;
        break;        
        default:
            this->state = E_CHECKPOINT_ST_IDLE;
    }
}

void object_trigger_update(tEntity *this, tSolidObjectLocalData *local)
{
    switch (this->entType)
    {
        case E_STOP_SCROLL_OBJECT_TYPE:
            if (CHECK_FLAG(this->spare, E_STOP_SCROLL_LEFT))
                game.stopScrollLeft = this->pos.x;
            if (CHECK_FLAG(this->spare, E_STOP_SCROLL_RIGHT))
                game.stopScrollRight = this->pos.x;
            if (CHECK_FLAG(this->spare, E_STOP_SCROLL_DOWN))
                game.stopScrollDown = true;
            if (CHECK_FLAG(this->spare, E_STOP_SCROLL_UP))
                game.stopScrollUp = true;    
        break;
    }
}

void object_trace(tEntity *this)
{
    MY_TRACE_FLAG("Object Instance: %d\n\tObj Type:%d\n", this->entInstance, this->entType);
}