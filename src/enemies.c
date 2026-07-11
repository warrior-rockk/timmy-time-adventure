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
#include "player.h"

#include "data/edata.h"

#define TRACE_FLAG  "[ENEMY]"

uint16_t numEnemyInstances;                 //num of enemy instances
tEnemyLocalData *enemyDataList;             //list of enemy local data
BITMAP *enemyResources[E_ENEMIES_TYPE_NUM]; //enemy gfx resources
SAMPLE *enemySfx[E_SFX_ENEMY_NUM];          //enemy sfx resources
DATAFILE_INDEX *enemyDataFileIndex;         //enemy datafile index

void enemy_system_init()
{
    //initialize array to NULL
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

//function to add enemy to local enemy data. Returns the new array and increments the size counter
static tEnemyLocalData* enemy_data_add(tEnemyLocalData *array, uint16_t *listSize, uint8_t localDataType, void *data) {
    int newSize = *listSize + 1;
    
    //allocates one more element
    tEnemyLocalData *temp = realloc(array, newSize * sizeof(tEnemyLocalData));
    
    //check allocation
    if (temp == NULL) {
        abort_on_error("Can't assign memory for enemies entities\n");
        return array;
    }
    
    //realloc checked. Assign to original array
    array = temp;
    
    //store the metadata and pointer
    array[*listSize].structureType = localDataType;
    array[*listSize].data = data;
    
    #if DEBUG_LOCAL_DATA_MEMORY
        switch(array[*listSize].structureType)
        {
            case E_ENEMY_DEFAULT_LOCAL_DATA_TYPE:
                MY_TRACE_FLAG("Allocating default enemy local data\n");
                MY_TRACE_FLAG("Memory address of data %p\n", array[*listSize].data);
            break;
            case E_ENEMY_AXE_LOCAL_DATA_TYPE:
                MY_TRACE_FLAG("Allocating axel enemy local data\n");
                MY_TRACE_FLAG("Memory address of data %p\n", array[*listSize].data);            
            break;
        }
    #endif

    //increment size counter
    (*listSize)++;
    
    return array;
}

//function to delete enemy local data from array with swap and pop (last array position moves to removed element)
static tEnemyLocalData* enemy_data_remove(tEnemyLocalData *array, uint16_t *listSize, uint16_t enemyIndex) {
    //check enemy index bounds
    if (enemyIndex < 0 || enemyIndex >= *listSize) {
        //abort_on_error("Enemy index %d out of range\n", enemyIndex);
        MY_TRACE_FLAG("ERROR: Enemy index %d out of range\n", enemyIndex);
        return array;
    }
    
    #if DEBUG_LOCAL_DATA_MEMORY
        switch(array[enemyIndex].structureType)
        {
            case E_ENEMY_DEFAULT_LOCAL_DATA_TYPE:
                MY_TRACE_FLAG("Deallocating default enemy local data\n");
                MY_TRACE_FLAG("Memory address of data %p\n", array[enemyIndex].data);
            break;
            case E_ENEMY_AXE_LOCAL_DATA_TYPE:
                MY_TRACE_FLAG("Deallocating axe enemy local data\n");
                MY_TRACE_FLAG("Memory address of data %p\n", array[enemyIndex].data);                
            break;
        }
    #endif

    //free the allocated data of the enemy to delete    
    free(array[enemyIndex].data);
    array[enemyIndex].data = NULL;

    //get last index
    int16_t last_index = *listSize - 1;
    //MY_TRACE_FLAG("enemyIndex %i last_index %i numEnemyinstances %i\n", enemyIndex, last_index, numEnemyInstances);
    //if element isn't last, make the swap
    if (enemyIndex != last_index) {
        //copy data from last element to remove element position
        array[enemyIndex] = array[last_index];

        //MY_TRACE_FLAG("Copied Enemy index %i to %i\n", last_index , enemyIndex);
        
        /*
        //get the entity of last index enemy index (we need to update his instace number)
        tEntity *lastEnemyEntity = entity_get_by_instance(E_ENT_CLASS_ENEMY, last_index);
        if (lastEnemyEntity == NULL)
            abort_on_error("Can't find enemy entity with entity instance: %i\n", last_index);            
        
        //updates instance number
        MY_TRACE_FLAG("Enemy id %i instance %i moved to instance %i\n", lastEnemyEntity->id, lastEnemyEntity->entInstance, enemyIndex);
        lastEnemyEntity->entInstance = enemyIndex;    */
    }
    
    //decrease the size counter (pop)
    (*listSize)--;

    //reduces memory space of the array
    if (*listSize > 0) 
    {
        tEnemyLocalData *temp = realloc(array, (*listSize) * sizeof(tEnemyLocalData));
        if (temp != NULL) array = temp;
    } 
    else 
    {
        free(array);
        array = NULL;
    }

    //MY_TRACE_FLAG("Num enemy instances after destroy: %i\n", *listSize);

    return array;
}

void enemy_destroy(tEntity *entity)
{
    MY_TRACE_FLAG("Destroying enemy id: %i, instance:%i\n", entity->id, entity->entInstance);

    //remove enemy instance from list
    enemyDataList = enemy_data_remove(enemyDataList, &numEnemyInstances, entity->entInstance);    
}

//check enemy entity type to add the local data structure to local data list and increases instances number
void enemy_create(tEntity *entity)
{
    
    //tDefaultEnemyLocalData *enemyLocalData;
    void *enemyLocalData = NULL;
    uint8_t enemyLocalDataType = E_ENEMY_DEFAULT_LOCAL_DATA_TYPE;

    //set enemy type properties
    switch (entity->entType)
    {
        case E_PTERO_ENEMY_TYPE:            
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, PTERO_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){71, 64};
            entity->size.x = 30;
            entity->size.y = 16;                                      
        break;
        case E_RAPTOR_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, RAPTOR2_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){72, 44};                          
            entity->size = (tVector){46, 32};
            entity->axis = E_ENT_AXIS_DOWN;  
            SET_FLAG(entity->properties, E_ENT_PROP_PHYSICS_ON);     
            collision_create_entity_points(entity);     
        break;
        case E_TRICE_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, TRICE_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){48, 34};                          
            entity->size = (tVector){40, 32};
            entity->axis = E_ENT_AXIS_DOWN;  
            SET_FLAG(entity->properties, E_ENT_PROP_PHYSICS_ON);     
            collision_create_entity_points(entity);     
        break;
        case E_SPIDER_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, SPIDER_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){20, 29};                          
            entity->size = (tVector){16, 16};        
            collision_create_entity_points(entity);              
        break;        
        case E_PIRANHA_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, PIRANHA_BMP);
            load_entity_wav_resources(&enemySfx[E_SFX_ENEMY_SPLASH], enemyDataFileIndex, SPLASH_WAV);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){42, 33};                          
            entity->size = (tVector){32, 32};                    
        break;
        case E_COWBOY_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, COWBOY_BMP);
            load_entity_wav_resources(&enemySfx[E_SFX_ENEMY_SHOOT], enemyDataFileIndex, SHOOT_WAV);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){50, 45};                          
            entity->size = (tVector){18, 32};        
            entity->axis = E_ENT_AXIS_DOWN;                        
        break;
        case E_BULLET_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, BULLET_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){2, 2};                          
            entity->size = (tVector){2, 2};  
            entity->properties = E_ENT_PROP_AUTO_DESTROY | E_ENT_PROP_NO_HURT;                  
        break;
        case E_EAGLE_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, EAGLE_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){32, 34};                          
            entity->size = (tVector){18, 12};      
            collision_create_entity_points(entity);              
        break;        
        case E_TUMBLE_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, TUMBLE_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){30, 29};                          
            entity->size = (tVector){28, 20};                  
            entity->axis = E_ENT_AXIS_DOWN;
            entity->properties = E_ENT_PROP_PHYSICS_ON;
            collision_create_entity_points(entity);
        break;
        case E_SCORPION_ENEMY_TYPE:
            if (entity->spare == 0)
                load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, SCORPION_BMP);
            else
                load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, SCORRED_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){30, 23};                          
            entity->size = (tVector){24, 16};      
            entity->axis = E_ENT_AXIS_DOWN;
            entity->properties = E_ENT_PROP_NO_HURT;
            collision_create_entity_points(entity);              
        break;
        case E_INDIAN_AXE_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, INDIAN_A_BMP);
            load_entity_wav_resources(&enemySfx[E_SFX_ENEMY_AXE], enemyDataFileIndex, AXE_WAV);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){47, 47};                          
            entity->size = (tVector){16, 32};      
            entity->axis = E_ENT_AXIS_DOWN;
        break;       
        case E_INDIAN_BOW_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, INDIAN_B_BMP);
            load_entity_wav_resources(&enemySfx[E_SFX_ENEMY_ARROW], enemyDataFileIndex, ARROW_WAV);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){56, 51};                          
            entity->size = (tVector){16, 32};      
            entity->axis = E_ENT_AXIS_DOWN;
        break;       
        case E_AXE_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, AXE_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){19, 19};                          
            entity->size = (tVector){16, 16};  
            entity->properties = E_ENT_PROP_AUTO_DESTROY | E_ENT_PROP_NO_HURT;   
            enemyLocalData = malloc(sizeof(tAxeLocalData));
            enemyLocalDataType = E_ENEMY_AXE_LOCAL_DATA_TYPE;               
        break; 
        case E_ARROW_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, ARROW_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){24, 5};                          
            entity->size = (tVector){20, 5};  
            entity->properties = E_ENT_PROP_AUTO_DESTROY | E_ENT_PROP_NO_HURT;                  
        break; 
        case E_BAT_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, BAT_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){44, 41};                          
            entity->size = (tVector){16, 16};      
            collision_create_entity_points(entity);              
        break;        
        case E_EGYPTIAN_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, EGYPTIAN_BMP);
            load_entity_wav_resources(&enemySfx[E_SFX_ENEMY_WIP], enemyDataFileIndex, WIP_WAV);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){65, 50};                          
            entity->size = (tVector){20, 32};
            entity->axis = E_ENT_AXIS_DOWN;  
            SET_FLAG(entity->properties, E_ENT_PROP_PHYSICS_ON);     
            collision_create_entity_points(entity);     
        break;
        case E_TRAP_ARROW_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, ETRAPA_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){13, 5};                          
            entity->size = (tVector){13, 5};                     
            entity->properties = E_ENT_PROP_AUTO_DESTROY | E_ENT_PROP_NO_HURT;
        break;
        case E_BEETLE_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, BEETLE_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){32, 18};                          
            entity->size = (tVector){26, 16};      
            entity->axis = E_ENT_AXIS_DOWN;            
            entity->properties = E_ENT_PROP_PHYSICS_ON;
            collision_create_entity_points(entity);              
        break;
        case E_MUMMY_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, MUMMY_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){57, 49};                          
            entity->size = (tVector){16, 32};
            entity->axis = E_ENT_AXIS_DOWN;  
            SET_FLAG(entity->properties, E_ENT_PROP_PHYSICS_ON);     
            collision_create_entity_points(entity);     
        break;
        case E_HITBOX_ENEMY_TYPE:
            entity->size = (tVector){16, 16};
            entity->properties = E_ENT_PROP_AUTO_DESTROY;
        break;
        case E_VULTURE_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, VULTURE_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){33, 30};                          
            entity->size = (tVector){20, 16};                  
        break;
        case E_ANUBIS_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, ANUBIS_BMP);
            load_entity_wav_resources(&enemySfx[E_SFX_ENEMY_WIP], enemyDataFileIndex, WIP_WAV);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){57, 44};                          
            entity->size = (tVector){20, 32};
            entity->axis = E_ENT_AXIS_DOWN;              
        break;
        case E_TRAP_FIRE_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, FIREBALL_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){28, 15};                          
            entity->size = (tVector){28, 15};                     
            entity->properties = E_ENT_PROP_AUTO_DESTROY | E_ENT_PROP_NO_HURT;
        break;
        case E_GHOST_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, GHOST_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){102, 65};                          
            entity->size = (tVector){20, 32};       
            entity->axis = E_ENT_AXIS_UP;   
            if (entity->spare) //patrol?
                collision_create_entity_points(entity);
        break;
        case E_KNIGHT_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, KNIGHT_BMP);
            load_entity_wav_resources(&enemySfx[E_SFX_ENEMY_SWORD], enemyDataFileIndex, SWORD_WAV);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){59, 49};                          
            entity->size = (tVector){20, 32};
            entity->axis = E_ENT_AXIS_DOWN;  
            SET_FLAG(entity->properties, E_ENT_PROP_PHYSICS_ON);     
            collision_create_entity_points(entity);     
        break;
        case E_SKELETON_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, SKELETON_BMP);
            load_entity_wav_resources(&enemySfx[E_SFX_ENEMY_SWORD], enemyDataFileIndex, SWORD_WAV);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){64, 50};                          
            entity->size = (tVector){20, 32};
            entity->axis = E_ENT_AXIS_DOWN;  
            SET_FLAG(entity->properties, E_ENT_PROP_PHYSICS_ON);     
            collision_create_entity_points(entity);     
        break;
        case E_SPIKE_BALL_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, SPKBALL_BMP);            
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){22, 21};                          
            entity->size = (tVector){16, 16};
            entity->axis = E_ENT_AXIS_CENTER;  
            entity->properties = E_ENT_PROP_NO_HURT | E_ENT_PROP_NO_FLIP;     
            collision_create_entity_points(entity);     
        break;        
        case E_CANNONBALL_ENEMY_TYPE:
            load_entity_bmp_resources(&enemyResources[entity->entType], enemyDataFileIndex, CANBALL_BMP);
            entity->img = enemyResources[entity->entType]; 
            entity->spriteSize = (tVector){16, 16};                          
            entity->size = (tVector){16, 16};  
            entity->properties = E_ENT_PROP_AUTO_DESTROY | E_ENT_PROP_NO_HURT;                  
        break;
        default:
            abort_on_error("Enemy type entity not valid");
        break;
    }

    //if not local data type assigned
    if (enemyLocalData == NULL)
        //allocate default enemy local data
        enemyLocalData = malloc(sizeof(tDefaultEnemyLocalData));
    
    //adds enemy local data to list
    enemyDataList = enemy_data_add(enemyDataList, &numEnemyInstances, enemyLocalDataType, enemyLocalData);

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
    #if DEBUG_LOCAL_DATA_MEMORY
        MY_TRACE_FLAG("Update Memory address of data %p\n", enemyDataList[entity->entInstance].data);
    #endif

    switch (entity->entType)
    {
        case E_PTERO_ENEMY_TYPE:            
            enemy_ptero_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;        
        case E_RAPTOR_ENEMY_TYPE:            
            enemy_raptor_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;        
        case E_SPIDER_ENEMY_TYPE:            
            enemy_spider_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_PIRANHA_ENEMY_TYPE:            
            enemy_piranha_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_COWBOY_ENEMY_TYPE:            
            enemy_cowboy_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_BULLET_ENEMY_TYPE:            
            enemy_bullet_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_EAGLE_ENEMY_TYPE:            
            enemy_eagle_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_TUMBLE_ENEMY_TYPE:            
            enemy_tumble_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_TRICE_ENEMY_TYPE:            
            enemy_trice_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_SCORPION_ENEMY_TYPE:          
            enemy_scorpion_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_INDIAN_AXE_ENEMY_TYPE:            
            enemy_indian_axe_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_INDIAN_BOW_ENEMY_TYPE:            
            enemy_indian_bow_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_AXE_ENEMY_TYPE:            
            enemy_axe_update(entity, (tAxeLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_ARROW_ENEMY_TYPE:            
        case E_TRAP_ARROW_ENEMY_TYPE:
        case E_TRAP_FIRE_ENEMY_TYPE:
        case E_CANNONBALL_ENEMY_TYPE:
            enemy_projectile_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_BAT_ENEMY_TYPE:            
            enemy_bat_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_EGYPTIAN_ENEMY_TYPE:             
            enemy_egyptian_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;        
        case E_BEETLE_ENEMY_TYPE:          
            enemy_beetle_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_MUMMY_ENEMY_TYPE:          
            enemy_mummy_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_HITBOX_ENEMY_TYPE:
            enemy_hitbox_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_VULTURE_ENEMY_TYPE:
            enemy_vulture_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_ANUBIS_ENEMY_TYPE:             
            enemy_anubis_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;        
        case E_GHOST_ENEMY_TYPE:             
            enemy_ghost_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_KNIGHT_ENEMY_TYPE:             
            enemy_knight_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_SKELETON_ENEMY_TYPE:             
            enemy_skeleton_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;
        case E_SPIKE_BALL_ENEMY_TYPE:             
            enemy_spike_ball_update(entity, (tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data);
        break;        
        default:
        break;
    }

    //show_debug("Num enemies:%i", numEnemyInstances);
}

//calls specified enemy type init function
void enemy_init(tEntity *entity)
{   
    switch (entity->entType)
    {   
        default:            
            ((tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data)->flag = 0;
            ((tDefaultEnemyLocalData*)enemyDataList[entity->entInstance].data)->timer = 0;            
        break;
    }
}


//ENEMY IA FUNCTIONS
//=========================================================================

//Enemy patrol: moves around a X range
static void enemy_patrol_ia(tEntity *entity, fixed velocity, int16_t patrol_range)
{
    //linear movement
    entity->fixVel.x = entity->dir == E_ENT_DIR_LEFT ? -velocity : velocity;            
    
    //change direction on range patrol
    if ((entity->dir && entity->pos.x > (entity->initPos.x + patrol_range)) || (!entity->dir && entity->pos.x < (entity->initPos.x - patrol_range)))
        entity->dir = !entity->dir;
}

//Enemy fixed move: moves enemy and dead when exit screen
static void enemy_fixed_move(tEntity *this, fixed velocity)
{
    this->fixVel.x = this->dir == E_ENT_DIR_LEFT ? ftofix(-velocity) : ftofix(velocity);

    if (!scroll_rect_on_region((tRectangle){this->pos, this->size}))
    {
        this->dead = true;
    }
}

//Do enemy dead
void enemy_dead(tEntity *entity, int startFrame, int endFrame, int speed, uint8_t mode)
{
    //stop enemy
    entity->fixVel.x = 0;
    if (!CHECK_FLAG(entity->properties, E_ENT_PROP_PHYSICS_ON))
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

//ENEMIES CODE

void enemy_ptero_update(tEntity *this, tDefaultEnemyLocalData *local)
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

void enemy_raptor_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    #define RAPTOR_VELOCITY                     0.6
    #define RAPTOR_RANGE_PATROL                 50
    #define RAPTOR_PLAYER_RANGE                 20
    #define RAPTOR_HITBOX_X_OFFSET_LEFT         19
    #define RAPTOR_HITBOX_X_OFFSET_RIGHT        3
    #define RAPTOR_HITBOX_DURATION              20
    #define RAPTOR_HITBOX_FRAME                 3
    
    //enemy animations
    #define ANIM_RAPTOR_WALK   4,   6,  10, ANIM_PING_PONG
    #define ANIM_RAPTOR_ATACK  0,   3,  10, ANIM_PING_PONG_ONCE
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
            local->flag = false;    
            
            enemy_patrol_ia(this, ftofix(RAPTOR_VELOCITY), RAPTOR_RANGE_PATROL);
            
            //check range of player
            player = entity_get(entity_get_player_id());
            if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, RAPTOR_PLAYER_RANGE))
                this->state = E_RAPTOR_ST_ATTACK;

            play_animation(&this->anim, ANIM_RAPTOR_WALK);
        break;     
        case E_RAPTOR_ST_ATTACK:
            if (this->anim.frame == RAPTOR_HITBOX_FRAME)
            {
                if (!local->flag)
                {
                    local->flag = true;
                    int16_t hitX = this->dir == E_ENT_DIR_LEFT ? -RAPTOR_HITBOX_X_OFFSET_LEFT : this->size.x + RAPTOR_HITBOX_X_OFFSET_RIGHT; 
                    entity_create(E_ENT_CLASS_ENEMY, E_HITBOX_ENEMY_TYPE, (tVector){this->pos.x + hitX, this->pos.y}, this->dir, RAPTOR_HITBOX_DURATION);
                }               
            }
            
            if (play_animation(&this->anim, ANIM_RAPTOR_ATACK))
            {
                this->state = E_RAPTOR_ST_MOVING;
            }
        break;   
        case E_RAPTOR_ST_HURT:
            enemy_dead(this, ANIM_RAPTOR_DEAD);            
        break;
    }       
}

void enemy_trice_update(tEntity *this, tDefaultEnemyLocalData *local)
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

void enemy_piranha_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    //enemy definitions
    #define PIRANHA_DEFAULT_WAIT_TIME   100
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
                sfx_play(enemySfx[E_SFX_ENEMY_SPLASH], E_SFX_ENEMY_VOICE);           
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


void enemy_spider_update(tEntity *this, tDefaultEnemyLocalData *local)
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

void enemy_cowboy_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    //enemy definitions
    #define COWBOY_PLAYER_RANGE             160
    #define COWBOY_WAIT_TIME                15
    #define COWBOY_BULLET_Y_OFFSET          4   
    #define COWBOY_BULLET_Y_CROUCH_OFFSET   14
    #define COWBOY_BULLET_X_OFFSET          4

    //enemy animations
    #define ANIM_COWBOY_IDLE            0,   0, 10, ANIM_LOOP
    #define ANIM_COWBOY_SHOOT           1,   16, 5, ANIM_ONCE
    #define ANIM_COWBOY_SHOOT_CROUCH    24,  32, 6, ANIM_PING_PONG_ONCE
    #define ANIM_COWBOY_DEAD            17,  23,    ENEMY_DEFAULT_DEAD_TIME, ANIM_ONCE

    //enemy states
    enum E_COWBOY_ENEMY_STATES{E_COWBOY_ST_IDLE, E_COWBOY_ST_SHOOT, E_COWBOY_ST_SHOOT_CROUCH, E_COWBOY_ST_WAIT, E_COWBOY_ST_HURT};   

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
            {
                //if spare!=0, shoot crouch if player crouch
                if (player->size.y == PLAYER_SIZE_H_CROUCH && this->spare)
                    this->state = E_COWBOY_ST_SHOOT_CROUCH;
                else
                    this->state = E_COWBOY_ST_SHOOT;
            }

            play_animation(&this->anim, ANIM_COWBOY_IDLE);
        break;
        case E_COWBOY_ST_SHOOT:            
            if (play_animation(&this->anim, ANIM_COWBOY_SHOOT))
            {
                this->state = E_COWBOY_ST_WAIT;      
            }
            if (this->anim.frame == 11 && !local->flag)
            {
                local->flag = true; //it's important to set the local flag before entity creation in case pointer moves
                entity_create(E_ENT_CLASS_ENEMY, E_BULLET_ENEMY_TYPE, (tVector){this->pos.x, this->pos.y + COWBOY_BULLET_Y_OFFSET}, this->dir, this->spare);
                sfx_play(enemySfx[E_SFX_ENEMY_SHOOT], E_SFX_ENEMY_VOICE);
                
            }
        break;     
        case E_COWBOY_ST_SHOOT_CROUCH:            
            if (play_animation(&this->anim, ANIM_COWBOY_SHOOT_CROUCH))
            {
                this->state = E_COWBOY_ST_WAIT;      
            }
            if (this->anim.frame == 31 && !local->flag)
            {
                local->flag = true; //it's important to set the local flag before entity creation in case pointer moves
                entity_create(E_ENT_CLASS_ENEMY, E_BULLET_ENEMY_TYPE, (tVector){this->pos.x, this->pos.y + COWBOY_BULLET_Y_CROUCH_OFFSET}, this->dir, this->spare);
                sfx_play(enemySfx[E_SFX_ENEMY_SHOOT], E_SFX_ENEMY_VOICE);
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

void enemy_bullet_update(tEntity *this, tDefaultEnemyLocalData *local)
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

void enemy_eagle_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    //enemy definitions
    #define EAGLE_PATROL_VELOCITY     0.8
    #define EAGLE_PATROL_RANGE        50
    #define EAGLE_PLAYER_RANGE        55
    #define EAGLE_ATTACK_VEL_Y        1.2
    #define EAGLE_ATTACK_MAX_VEL_X    2
    
    //enemy animations
    #define ANIM_EAGLE_FLY     0,   7, 10,  ANIM_LOOP
    #define ANIM_EAGLE_ATTACK  9,   9, 10,  ANIM_LOOP
    #define ANIM_EAGLE_RETURN  10,  10, 10, ANIM_LOOP
    #define ANIM_EAGLE_TURN    15,  17, 5, ANIM_ONCE
    #define ANIM_EAGLE_HURT    11,  14, ENEMY_DEFAULT_DEAD_TIME, ANIM_ONCE

    //enemy states
    enum E_EAGLE_ENEMY_STATES{E_EAGLE_ST_IDLE, E_EAGLE_ST_FLY, E_EAGLE_ST_ATTACK, E_EAGLE_ST_RETURN, E_EAGLE_ST_TURN, E_EAGLE_ST_HURT};   

    tEntity *player;

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_EAGLE_ST_HURT;

    switch (this->state)
    {
        case E_EAGLE_ST_IDLE:
            //use local flag to store direction
            local->flag = this->dir;
            this->state++;
        break;
        case E_EAGLE_ST_FLY:        
            //get player instance
            player = entity_get(entity_get_player_id());
            
            enemy_patrol_ia(this, ftofix(EAGLE_PATROL_VELOCITY), EAGLE_PATROL_RANGE);
            //if direction changes, do turn animation
            if (this->dir != local->flag)
                this->state = E_EAGLE_ST_TURN;
            
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
        case E_EAGLE_ST_TURN:
            //set the new direction
            local->flag = this->dir;
            //play turn animation
            if (play_animation(&this->anim, ANIM_EAGLE_TURN))
                this->state = E_EAGLE_ST_FLY;
        break;
        case E_EAGLE_ST_HURT:
            enemy_dead(this, ANIM_EAGLE_HURT);            
        break;
    }       
}

void enemy_tumble_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    //enemy defines
    #define TUMBLE_VELOCITY     2.0

    //enemy animations
    #define ANIM_TUMBLE_ROLL   0,   7, 8,  ANIM_LOOP

    //enemy states
    enum E_TUMBLE_ENEMY_STATES{E_TUMBLE_ST_ROLL};
    
    if (this->signal == E_ENT_SIGNAL_AWAKE)
    {
        this->signal = E_ENT_SIGNAL_NONE;
        this->pos = this->initPos;
        this->fixPos = vector2fixvector(this->pos);
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

    switch (this->state)
    {
        case E_TUMBLE_ST_ROLL:            
            enemy_fixed_move(this, TUMBLE_VELOCITY);

            play_animation(&this->anim, ANIM_TUMBLE_ROLL);
        break;
    }       
}

void enemy_trace(tEntity *this)
{
    MY_TRACE_FLAG("Enemy Instance: %d\n\tObj Type:%d\n", this->entInstance, this->entType);
}

void enemy_scorpion_update(tEntity *this, tDefaultEnemyLocalData *local)
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

void enemy_indian_axe_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    //enemy definitions
    #define INDIAN_AXE_PLAYER_RANGE     120
    #define INDIAN_AXE_INIT_WAIT        50
    #define INDIAN_AXE_WAIT_TIME        30
    #define INDIAN_AXE_AXE_FRAME_THROW  6
    #define INDIAN_AXE_AXE_Y_OFFSET     6
    
    //enemy animations
    #define ANIM_INDIAN_AXE_IDLE   0,   0, 10,  ANIM_LOOP
    #define ANIM_INDIAN_AXE_SHOOT  5,   8, 20,  ANIM_ONCE
    #define ANIM_INDIAN_AXE_DEAD   9,  15, ENEMY_DEFAULT_DEAD_TIME, ANIM_ONCE

    //enemy states
    enum E_INDIAN_AXE_ENEMY_STATES{E_INDIAN_AXE_ST_INIT_WAIT, E_INDIAN_AXE_ST_IDLE, E_INDIAN_AXE_ST_SHOOT, E_INDIAN_AXE_ST_WAIT, E_INDIAN_AXE_ST_HURT};   

    tEntity *player;

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_INDIAN_AXE_ST_HURT;

    switch (this->state)
    {
        case E_INDIAN_AXE_ST_INIT_WAIT:
            play_animation(&this->anim, ANIM_INDIAN_AXE_IDLE);

            player = entity_get(entity_get_player_id());

            if (local->timer > INDIAN_AXE_INIT_WAIT)
            {
                local->timer = 0;
                this->state = E_INDIAN_AXE_ST_IDLE;
            }
            else if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, INDIAN_AXE_PLAYER_RANGE))
                local->timer += clock_tick_get();
        break;
        case E_INDIAN_AXE_ST_IDLE:        
            local->flag = false;    
            //get player instance
            player = entity_get(entity_get_player_id());            
            
            //direction faces player
            this->dir = player->pos.x < this->pos.x ? E_ENT_DIR_LEFT : E_ENT_DIR_RIGHT;

            //shoot in player range
            if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, INDIAN_AXE_PLAYER_RANGE))
                this->state++;

            play_animation(&this->anim, ANIM_INDIAN_AXE_IDLE);
        break;
        case E_INDIAN_AXE_ST_SHOOT:            
            if (play_animation(&this->anim, ANIM_INDIAN_AXE_SHOOT))
            {
                this->state++;      
            }
            if (this->anim.frame == INDIAN_AXE_AXE_FRAME_THROW && !local->flag)
            {
                local->flag = true; //it's important to set the local flag before entity creation in case pointer moves
                sfx_play(enemySfx[E_SFX_ENEMY_AXE], E_SFX_ENEMY_VOICE);
                entity_create(E_ENT_CLASS_ENEMY, E_AXE_ENEMY_TYPE, (tVector){this->pos.x, this->pos.y - INDIAN_AXE_AXE_Y_OFFSET}, this->dir, this->spare);
            }
        break;     
        case E_INDIAN_AXE_ST_WAIT:
            if (local->timer > INDIAN_AXE_WAIT_TIME)
            {
                local->timer = 0;
                this->state = E_INDIAN_AXE_ST_IDLE;
            }
            else
                local->timer += clock_tick_get();

            play_animation(&this->anim, ANIM_INDIAN_AXE_IDLE); 
        break;   
        case E_INDIAN_AXE_ST_HURT:
            enemy_dead(this, ANIM_INDIAN_AXE_DEAD);            
        break;
    }       
}

void enemy_indian_bow_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    //enemy definitions
    #define INDIAN_BOW_PLAYER_RANGE        100
    #define INDIAN_BOW_WAIT_TIME           30
    #define INDIAN_BOW_ARROW_FRAME_THROW   17
    #define INDIAN_BOW_ARROW_Y_OFFSET      4

    //enemy animations
    #define ANIM_INDIAN_BOW_IDLE   0,   0, 10,  ANIM_LOOP
    #define ANIM_INDIAN_BOW_SHOOT  9,   20, 5,  ANIM_ONCE
    #define ANIM_INDIAN_BOW_DEAD   21,  27, ENEMY_DEFAULT_DEAD_TIME, ANIM_ONCE

    //enemy states
    enum E_INDIAN_BOW_ENEMY_STATES{E_INDIAN_BOW_ST_IDLE, E_INDIAN_BOW_ST_SHOOT, E_INDIAN_BOW_ST_WAIT, E_INDIAN_BOW_ST_HURT};   

    tEntity *player;

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_INDIAN_BOW_ST_HURT;

    switch (this->state)
    {
        case E_INDIAN_BOW_ST_IDLE:        
            //reset flag
            local->flag = false;    
            //get player instance
            player = entity_get(entity_get_player_id());            
            
            //direction faces player
            this->dir = player->pos.x < this->pos.x ? E_ENT_DIR_LEFT : E_ENT_DIR_RIGHT;

            //shoot in player range
            if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, INDIAN_BOW_PLAYER_RANGE))
                this->state++;

            play_animation(&this->anim, ANIM_INDIAN_BOW_IDLE);
        break;
        case E_INDIAN_BOW_ST_SHOOT:            
            if (play_animation(&this->anim, ANIM_INDIAN_BOW_SHOOT))
            {
                this->state++;      
            }
            if (this->anim.frame == INDIAN_BOW_ARROW_FRAME_THROW && !local->flag)
            {
                local->flag = true; //it's important to set the local flag before entity creation in case pointer moves
                sfx_play(enemySfx[E_SFX_ENEMY_ARROW], E_SFX_ENEMY_VOICE);
                entity_create(E_ENT_CLASS_ENEMY, E_ARROW_ENEMY_TYPE, (tVector){this->pos.x, this->pos.y + INDIAN_BOW_ARROW_Y_OFFSET}, this->dir, this->spare);
            }
        break;     
        case E_INDIAN_BOW_ST_WAIT:
            if (local->timer > INDIAN_BOW_WAIT_TIME)
            {
                local->timer = 0;
                this->state = E_INDIAN_BOW_ST_IDLE;
            }
            else
                local->timer += clock_tick_get();

            play_animation(&this->anim, ANIM_INDIAN_BOW_IDLE); 
        break;   
        case E_INDIAN_BOW_ST_HURT:
            enemy_dead(this, ANIM_INDIAN_BOW_DEAD);            
        break;
    }       
}

void enemy_axe_update(tEntity *this, tAxeLocalData *local)
{              
    //enemy defines
    #define AXE_VELOCITY    3.2     //velocity of horizontal movement
    #define AXE_RADIUS      12      //radius distance from the center axis
    #define AXE_ROTATION    18      //rotation velocity around center axis rotation

    //enemy animations
    #define ANIM_AXE_IDLE   0,   0, 10,  ANIM_LOOP
    #define ANIM_AXE_TURN   0,   2, 10,  ANIM_LOOP

    //enemy states
    enum E_AXE_ENEMY_STATES{E_AXE_ST_INIT, E_AXE_ST_MOVE};   

    switch (this->state)
    {
        case E_AXE_ST_INIT:        
            //set initial values
            local->cx = this->fixPos.x;
            local->cy = this->fixPos.y;
            local->angle = 0;       
            this->fixVel.x = 0;
            this->fixVel.y = 0;
            
            this->state++;
        break;
        case E_AXE_ST_MOVE:      
            //horizonal movement of center axis
            local->cx += this->dir == E_ENT_DIR_LEFT ? ftofix(-AXE_VELOCITY) : ftofix(AXE_VELOCITY); 
    
            //convert radius to fixed
            fixed radio = itofix(AXE_RADIUS);
            
            //calculate the orbit
            this->fixPos.x = local->cx + fmul(radio, fixcos(itofix(local->angle)));
            this->fixPos.y = local->cy + fmul(radio, fixsin(itofix(local->angle)));

            //calculate rotation. As integer using only 0 to 255, ignore the upper bits
            //allowing to angle reset to zero automatic
            local->angle = (local->angle + AXE_ROTATION) & 255;
        
            play_animation(&this->anim, ANIM_AXE_TURN);   
        break;
    }       
}

void enemy_projectile_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    //enemy defines
    #define PROJECTILE_VELOCITY     4.0

    //enemy animations
    #define ANIM_ARROW_IDLE   0,   0, 10,  ANIM_LOOP
    #define ANIM_FIREBALL     0,   2, 10, ANIM_LOOP
    
    //enemy states
    enum E_PROJECTILE_ENEMY_STATES{E_PROJECTILE_ST_IDLE};   

    switch (this->state)
    {
        case E_PROJECTILE_ST_IDLE:            
            this->fixVel.x = this->dir == E_ENT_DIR_LEFT ? ftofix(-PROJECTILE_VELOCITY) : ftofix(PROJECTILE_VELOCITY);

            if (this->entType == E_TRAP_FIRE_ENEMY_TYPE)
                play_animation(&this->anim, ANIM_FIREBALL);
            else
                play_animation(&this->anim, ANIM_ARROW_IDLE);
        break;
    }       
}

void enemy_bat_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    //enemy defines
    #define BAT_VELOCITY     1.6

    //enemy animations
    #define ANIM_BAT_FLY    0,   4, 10,  ANIM_LOOP
    #define ANIM_BAT_DEAD   5,   7, 20,  ANIM_ONCE

    //enemy states
    enum E_BAT_ENEMY_STATES{E_BAT_ST_FLY, E_BAT_ST_HURT};
    
    if (this->signal == E_ENT_SIGNAL_AWAKE)
    {
        this->signal = E_ENT_SIGNAL_NONE;
        this->pos = this->initPos;
        this->fixPos = vector2fixvector(this->pos);
    }

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_BAT_ST_HURT;

    switch (this->state)
    {
        case E_BAT_ST_FLY:            
            enemy_fixed_move(this, BAT_VELOCITY);

            play_animation(&this->anim, ANIM_BAT_FLY);
        break;
        case E_BAT_ST_HURT:
            enemy_dead(this, ANIM_BAT_DEAD);            
        break;
    }       
}

void enemy_egyptian_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    #define EGYPTIAN_VELOCITY                   0.6
    #define EGYPTIAN_RANGE_PATROL               50
    #define EGYPTIAN_PLAYER_RANGE               30
    #define EGYPTIAN_ATTACK_FRAME               4
    #define EGYPTIAN_HITBOX_X_OFFSET_LEFT       22
    #define EGYPTIAN_HITBOX_X_OFFSET_RIGHT      6
    #define EGYPTIAN_HITBOX_DURATION            20
    
    //enemy animations
    #define ANIM_EGYPTIAN_WALK   7,   12,  10, ANIM_LOOP
    #define ANIM_EGYPTIAN_ATACK  1,   6,  10, ANIM_ONCE
    #define ANIM_EGYPTIAN_DEAD   13,   19,  ENEMY_DEFAULT_DEAD_TIME, ANIM_ONCE

    //enemy states
    enum E_EGYPTIAN_ENEMY_STATES{E_EGYPTIAN_ST_IDLE, E_EGYPTIAN_ST_MOVING, E_EGYPTIAN_ST_ATTACK, E_EGYPTIAN_ST_HURT};   

    tEntity *player;

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_EGYPTIAN_ST_HURT;
    
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
        case E_EGYPTIAN_ST_IDLE:            
            this->state++;
        break;
        case E_EGYPTIAN_ST_MOVING:            
            local->flag = false;

            if (this->spare)
            {
                enemy_patrol_ia(this, ftofix(EGYPTIAN_VELOCITY), EGYPTIAN_RANGE_PATROL);
                play_animation(&this->anim, ANIM_EGYPTIAN_WALK);
            }
            else
                this->anim.frame = 0;
            
            //check range of player
            player = entity_get(entity_get_player_id());
            if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, EGYPTIAN_PLAYER_RANGE))
                this->state = E_EGYPTIAN_ST_ATTACK;

            
        break;     
        case E_EGYPTIAN_ST_ATTACK:
            if (this->anim.frame == EGYPTIAN_ATTACK_FRAME)
            {
                if (!local->flag)
                {
                    local->flag = true;
                    int16_t hitX = this->dir == E_ENT_DIR_LEFT ? -EGYPTIAN_HITBOX_X_OFFSET_LEFT : this->size.x + EGYPTIAN_HITBOX_X_OFFSET_RIGHT; 
                    sfx_play(enemySfx[E_SFX_ENEMY_WIP], E_SFX_ENEMY_VOICE);
                    entity_create(E_ENT_CLASS_ENEMY, E_HITBOX_ENEMY_TYPE, (tVector){this->pos.x + hitX, this->pos.y}, this->dir, EGYPTIAN_HITBOX_DURATION);
                }               
            }
            else
                local->flag = false;

            if (play_animation(&this->anim, ANIM_EGYPTIAN_ATACK))
            {
                this->state = E_EGYPTIAN_ST_MOVING;
            }
        break;   
        case E_EGYPTIAN_ST_HURT:
            enemy_dead(this, ANIM_EGYPTIAN_DEAD);            
        break;
    }       
}

void enemy_beetle_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    #define BEETLE_VELOCITY                 0.9
    #define BEETLE_RANGE_PATROL             50
    #define BEETLE_PLAYER_RANGE             40
    #define BEETLE_JUMP_VEL_X               2
    #define BEETLE_JUMP_VEL_Y               3
    #define BEETLE_JUMP_HIGH_VEL_Y          5
    #define BEETLE_PLAYER_JUMP_DETECTION    2
    
    //enemy animations
    #define ANIM_BEETLE_IDLE   1,   1,  6, ANIM_LOOP
    #define ANIM_BEETLE_WALK   1,   6,  6, ANIM_LOOP
    #define ANIM_BEETLE_DEAD   0,   0,  60, ANIM_ONCE

    //enemy states
    enum E_BEETLE_ENEMY_STATES{E_BEETLE_ST_IDLE, E_BEETLE_ST_MOVING, E_BEETLE_ST_JUMP, E_BEETLE_ST_HURT};   

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_BEETLE_ST_HURT;
    
    switch (this->state)
    {
        case E_BEETLE_ST_IDLE:            
            this->state++;
            play_animation(&this->anim, ANIM_BEETLE_IDLE);
        break;
        case E_BEETLE_ST_MOVING:            
            enemy_patrol_ia(this, ftofix(BEETLE_VELOCITY), BEETLE_RANGE_PATROL);

            //check range of player for jump
            tEntity *player = entity_get(entity_get_player_id());
            if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, BEETLE_PLAYER_RANGE))
            {
                //jump higher if player jump
                if (player->fixVel.y < -ftofix(BEETLE_PLAYER_JUMP_DETECTION))
                    this->fixVel.y = -ftofix(BEETLE_JUMP_HIGH_VEL_Y);
                else
                    this->fixVel.y = -ftofix(BEETLE_JUMP_VEL_Y);
                this->fixVel.x = this->dir ? ftofix(BEETLE_JUMP_VEL_X) : -ftofix(BEETLE_JUMP_VEL_X);
                this->ground = false;
                this->state = E_BEETLE_ST_JUMP;
            }
            
            play_animation(&this->anim, ANIM_BEETLE_WALK);
        break;     
        case E_BEETLE_ST_JUMP:
            if (this->ground)
                this->state = E_BEETLE_ST_MOVING;
        break;
        case E_BEETLE_ST_HURT:
            enemy_dead(this, ANIM_BEETLE_DEAD);            
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
        
        //change direction if horizontal collision
        if (colDir == E_COLLISION_DIR_RIGHT || colDir == E_COLLISION_DIR_LEFT)
            this->dir = !this->dir;
    }
}

void enemy_mummy_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    #define MUMMY_VELOCITY          1.6    
    #define MUMMY_PLAYER_RANGE      100
    #define MUMMY_WAIT_TIME         60
    
    //enemy animations
    #define ANIM_MUMMY_IDLE     13,  18, 14, ANIM_LOOP
    #define ANIM_MUMMY_WALK     1,   9,  4, ANIM_LOOP       
    #define ANIM_MUMMY_DEAD     11,   11,  60, ANIM_ONCE

    //enemy states
    enum E_MUMMY_ENEMY_STATES{E_MUMMY_ST_IDLE, E_MUMMY_ST_MOVING, E_MUMMY_ST_WAIT, E_MUMMY_ST_HURT};   

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_MUMMY_ST_HURT;

    tEntity *player = entity_get(entity_get_player_id());
    
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
        case E_MUMMY_ST_IDLE:                        
            this->dir = player->pos.x > this->pos.x;
            //check range of player            
            if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, MUMMY_PLAYER_RANGE))
            {
                this->state++;            
                local->flag = player->pos.x;                        
            }
            
            play_animation(&this->anim, ANIM_MUMMY_IDLE);
            
        break;
        case E_MUMMY_ST_MOVING:            
            if ((this->dir && this->pos.x >= local->flag) ||
                (!this->dir && this->pos.x <= local->flag))
            {
                this->state++;      
                this->fixVel.x = 0;                              
            }
            else
            {
                this->fixVel.x = this->dir == E_ENT_DIR_LEFT ? -ftofix(MUMMY_VELOCITY) : ftofix(MUMMY_VELOCITY);
            }            

            play_animation(&this->anim, ANIM_MUMMY_WALK);            
        break;     
        case E_MUMMY_ST_WAIT:
            this->dir = player->pos.x > this->pos.x;    
            if (local->timer >= MUMMY_WAIT_TIME)
            {
                this->state = E_MUMMY_ST_IDLE;
                local->timer = 0;
            }
            else   
                local->timer += clock_tick_get();

            play_animation(&this->anim, ANIM_MUMMY_IDLE);
        break;
        case E_MUMMY_ST_HURT:
            //blink
            entity_blink(this);
            //play dead sfx
            if (this->state != this->prevState)
            {
                sfx_play(enemySfx[E_SFX_ENEMY_DEAD], E_SFX_ENEMY_VOICE);
                game.score += SCORE_POINT_HURT_ENEMY;
            }
            if (play_animation(&this->anim, ANIM_MUMMY_DEAD))
            {
                this->signal = E_ENT_SIGNAL_NONE;
                this->state = E_MUMMY_ST_IDLE;
            }
            
        break;        
    }   
    show_debug("state %i", this->state);    
}

void enemy_hitbox_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    //enemy states
    enum E_HITBOX_ENEMY_STATES{E_HITBOX_ST_IDLE};   

    switch (this->state)
    {
        case E_HITBOX_ST_IDLE:         
            //hitbox lives the spare time   
            if (local->timer >= this->spare)
                this->dead = true;
            else
                local->timer += clock_tick_get();
        break;
    }       
}

void enemy_vulture_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    //enemy definitions
    #define VULTURE_PATROL_VELOCITY     0.8
    #define VULTURE_PATROL_RANGE        50
        
    //enemy animations
    #define ANIM_VULTURE_FLY     0,   6, 10,  ANIM_LOOP
    #define ANIM_VULTURE_TURN    8,  10, 5, ANIM_ONCE
    #define ANIM_VULTURE_HURT    7,  7, ENEMY_DEFAULT_DEAD_TIME, ANIM_ONCE

    //enemy states
    enum E_VULTURE_ENEMY_STATES{E_VULTURE_ST_IDLE, E_VULTURE_ST_FLY, E_VULTURE_ST_TURN, E_VULTURE_ST_HURT};   

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_VULTURE_ST_HURT;

    switch (this->state)
    {
        case E_VULTURE_ST_IDLE:
            //use local flag to store direction
            local->flag = this->dir;
            this->state++;
        break;
        case E_VULTURE_ST_FLY:        
            enemy_patrol_ia(this, ftofix(VULTURE_PATROL_VELOCITY), VULTURE_PATROL_RANGE);
            //if direction changes, do turn animation
            if (this->dir != local->flag)
                this->state = E_VULTURE_ST_TURN;

            play_animation(&this->anim, ANIM_VULTURE_FLY);
        break;                
        case E_VULTURE_ST_TURN:
            //set the new direction
            local->flag = this->dir;
            //play turn animation
            if (play_animation(&this->anim, ANIM_VULTURE_TURN))
                this->state = E_VULTURE_ST_FLY;
        break;
        case E_VULTURE_ST_HURT:
            enemy_dead(this, ANIM_VULTURE_HURT);            
        break;
    }       
}

void enemy_anubis_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    #define ANUBIS_PLAYER_RANGE               50
    #define ANUBIS_ATTACK_FRAME               6
    #define ANUBIS_HITBOX_X_OFFSET_LEFT       22
    #define ANUBIS_HITBOX_X_OFFSET_RIGHT      6
    #define ANUBIS_HITBOX_Y_OFFSET            6
    #define ANUBIS_HITBOX_DURATION            20
    #define ANUBIS_WAIT_ATTACK                50
    
    //enemy animations
    #define ANIM_ANUBIS_IDLE   0,   0,  10, ANIM_LOOP
    #define ANIM_ANUBIS_ATACK  1,   7,  8, ANIM_PING_PONG_ONCE    
    #define ANIM_ANUBIS_DEAD   8,   15,  8, ANIM_ONCE

    //enemy states
    enum E_ANUBIS_ENEMY_STATES{E_ANUBIS_ST_IDLE, E_ANUBIS_ST_ATTACK, E_ANUBIS_ST_WAIT, E_ANUBIS_ST_HURT};   

    tEntity *player;

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_ANUBIS_ST_HURT;    
    
    switch (this->state)
    {
        case E_ANUBIS_ST_IDLE:                        
            this->dir = E_ENT_DIR_LEFT;    
            
            //check range of player
            player = entity_get(entity_get_player_id());
            if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, ANUBIS_PLAYER_RANGE))
                this->state = E_ANUBIS_ST_ATTACK;

            play_animation(&this->anim, ANIM_ANUBIS_IDLE);
        break;        
        case E_ANUBIS_ST_ATTACK:
            if (this->anim.frame >= ANUBIS_ATTACK_FRAME)
                SET_FLAG(this->properties, E_ENT_PROP_NO_HURT);

            player = entity_get(entity_get_player_id());
            this->dir = player->pos.x > this->pos.x;

            if (this->anim.frame == ANUBIS_ATTACK_FRAME)
            {
                if (!local->flag)
                {
                    local->flag = true;
                    int16_t hitX = this->dir == E_ENT_DIR_LEFT ? -ANUBIS_HITBOX_X_OFFSET_LEFT : this->size.x + ANUBIS_HITBOX_X_OFFSET_RIGHT; 
                    sfx_play(enemySfx[E_SFX_ENEMY_WIP], E_SFX_ENEMY_VOICE);
                    entity_create(E_ENT_CLASS_ENEMY, E_HITBOX_ENEMY_TYPE, (tVector){this->pos.x + hitX, this->pos.y + ANUBIS_HITBOX_Y_OFFSET}, this->dir, ANUBIS_HITBOX_DURATION);
                }               
            }
            else
                local->flag = false;

            if (play_animation(&this->anim, ANIM_ANUBIS_ATACK))
            {
                this->state = E_ANUBIS_ST_WAIT;
                CLEAR_FLAG(this->properties, E_ENT_PROP_NO_HURT);
            }
        break;   
        case E_ANUBIS_ST_WAIT:        
            if (local->timer >= ANUBIS_WAIT_ATTACK)
            {
                this->state = E_ANUBIS_ST_IDLE;                
                local->timer = 0;
            }
            else
                local->timer += clock_tick_get();
            
            play_animation(&this->anim, ANIM_ANUBIS_IDLE);
        break;
        case E_ANUBIS_ST_HURT:
            enemy_dead(this, ANIM_ANUBIS_DEAD);            
        break;
    }       
}

void enemy_ghost_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    //enemy definitions
    #define GHOST_VELOCITY            1.4
    #define GHOST_PATROL_VELOCITY     0.8
    #define GHOST_PLAYER_RANGE        50
    #define GHOST_WAIT_TIME           200
    #define GHOST_PATROL_RANGE        100
        
    //enemy animations
    #define ANIM_GHOST_APPEAR        0,   3, 10,  ANIM_ONCE
    #define ANIM_GHOST_FLY           4,   7, 10,  ANIM_LOOP
    #define ANIM_GHOST_DISAPPEAR     8,   11, 10,  ANIM_ONCE
    
    //enemy states
    enum E_GHOST_ENEMY_STATES{E_GHOST_ST_IDLE, E_GHOST_ST_APPEAR, E_GHOST_ST_FLY, E_GHOST_ST_DISSAPEAR, E_GHOST_ST_WAIT, E_GHOST_ST_PATROL};   

    tEntity *player = entity_get(entity_get_player_id());

    switch (this->state)
    {
        case E_GHOST_ST_IDLE:
            if (this->spare)    //patrol?
            {
                this->state = E_GHOST_ST_PATROL;
            }
            else
            {
                SET_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
                //check range of player            
                if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, MUMMY_PLAYER_RANGE))
                {
                    this->state++;
                    this->visible = true;            
                    
                }
                else
                    this->visible = false;
            }
        break;
        case E_GHOST_ST_APPEAR:
            if (play_animation(&this->anim, ANIM_GHOST_APPEAR))
            {
                this->state++;
                local->flag = player->pos.x;  
                this->dir = player->pos.x > this->pos.x;                      
            }
        break;
        case E_GHOST_ST_FLY:     
            CLEAR_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);   
            if ((this->dir && this->pos.x >= local->flag) ||
                (!this->dir && this->pos.x <= local->flag))
            {
                this->state++;      
                this->fixVel.x = 0;                              
            }
            else
            {
                this->fixVel.x = this->dir == E_ENT_DIR_LEFT ? -ftofix(GHOST_VELOCITY) : ftofix(GHOST_VELOCITY);
            }

            play_animation(&this->anim, ANIM_GHOST_FLY);
        break;         
        case E_GHOST_ST_DISSAPEAR:
            SET_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            if (play_animation(&this->anim, ANIM_GHOST_DISAPPEAR))
                this->state++;
        break;       
        case E_GHOST_ST_WAIT:
            this->visible = false;
            if (local->timer >= GHOST_WAIT_TIME)
            {
                this->state = E_GHOST_ST_IDLE;
                local->timer = 0;
                this->state = E_GHOST_ST_IDLE;
            }
            else   
                local->timer += clock_tick_get();
        break;
        case E_GHOST_ST_PATROL:
            enemy_patrol_ia(this, ftofix(GHOST_PATROL_VELOCITY), GHOST_PATROL_RANGE);
                                    
            play_animation(&this->anim, ANIM_GHOST_FLY);
        break;
    }       
}

void enemy_knight_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    #define KNIGHT_VELOCITY                   0.6
    #define KNIGHT_RANGE_PATROL               50    
    #define KNIGHT_PLAYER_RANGE               50
    #define KNIGHT_ATTACK_FRAME               29
    #define KNIGHT_HITBOX_X_OFFSET_LEFT       22
    #define KNIGHT_HITBOX_X_OFFSET_RIGHT      6
    #define KNIGHT_HITBOX_DURATION            20
    
    //enemy animations
    #define ANIM_KNIGHT_WALK   0,   7,  10, ANIM_LOOP
    //#define ANIM_KNIGHT_ATACK  32,   40,  10, ANIM_ONCE
    #define ANIM_KNIGHT_ATACK  24,   31,  8, ANIM_ONCE
    #define ANIM_KNIGHT_BLOCK   8,   15,  5, ANIM_ONCE
    #define ANIM_KNIGHT_DEAD   16,   23,  10, ANIM_ONCE

    //enemy states
    enum E_KNIGHT_ENEMY_STATES{E_KNIGHT_ST_IDLE, E_KNIGHT_ST_MOVING, E_KNIGHT_ST_ATTACK, E_KNIGHT_ST_BLOCK, E_KNIGHT_ST_HURT};   

    tEntity *player = entity_get(entity_get_player_id());

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_KNIGHT_ST_HURT;
    
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
        case E_KNIGHT_ST_IDLE:            
            this->state++;
            CLEAR_FLAG(this->properties, E_ENT_PROP_HURTLESS);
            this->dir = player->pos.x > this->pos.x;
        break;
        case E_KNIGHT_ST_MOVING:            
            local->flag = false;

            if (this->spare)
            {
                enemy_patrol_ia(this, ftofix(KNIGHT_VELOCITY), KNIGHT_RANGE_PATROL);
                play_animation(&this->anim, ANIM_KNIGHT_WALK);
            }
            else
                this->anim.frame = 0;
            
            //check range of player
            if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, KNIGHT_PLAYER_RANGE))
                this->state = E_KNIGHT_ST_ATTACK;

            
        break;     
        case E_KNIGHT_ST_ATTACK:
            if (this->anim.frame == KNIGHT_ATTACK_FRAME)
            {
                if (!local->flag)
                {
                    local->flag = true;
                    int16_t hitX = this->dir == E_ENT_DIR_LEFT ? -KNIGHT_HITBOX_X_OFFSET_LEFT : this->size.x + KNIGHT_HITBOX_X_OFFSET_RIGHT; 
                    sfx_play(enemySfx[E_SFX_ENEMY_SWORD], E_SFX_ENEMY_VOICE);
                    entity_create(E_ENT_CLASS_ENEMY, E_HITBOX_ENEMY_TYPE, (tVector){this->pos.x + hitX, this->pos.y}, this->dir, KNIGHT_HITBOX_DURATION);
                }               
            }
            else{
                local->flag = false;
                if (player->fixVel.y < 0 && in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, this->size.x))
                    this->state = E_KNIGHT_ST_BLOCK;
            }                

            if (play_animation(&this->anim, ANIM_KNIGHT_ATACK))
            {
                this->state = E_KNIGHT_ST_MOVING;
            }
        break;   
        case E_KNIGHT_ST_BLOCK:
            SET_FLAG(this->properties, E_ENT_PROP_HURTLESS);
            if (play_animation(&this->anim, ANIM_KNIGHT_BLOCK))
            {
                this->state = E_KNIGHT_ST_IDLE;
            }
        break;
        case E_KNIGHT_ST_HURT:
            enemy_dead(this, ANIM_KNIGHT_DEAD);            
        break;
    }       
}

void enemy_skeleton_update(tEntity *this, tDefaultEnemyLocalData *local)
{              
    #define SKELETON_VELOCITY                   0.6
    #define SKELETON_RANGE_PATROL               50    
    #define SKELETON_PLAYER_RANGE               50
    #define SKELETON_ATTACK_FRAME               15
    #define SKELETON_HITBOX_X_OFFSET_LEFT       22
    #define SKELETON_HITBOX_X_OFFSET_RIGHT      6
    #define SKELETON_HITBOX_DURATION            20
    #define SKELETON_JUMP_VEL                   4.6
    
    //enemy animations
    #define ANIM_SKELETON_WALK   1,   9,  6, ANIM_LOOP
    #define ANIM_SKELETON_ATACK  10,   17,  8, ANIM_ONCE
    #define ANIM_SKELETON_JUMP   26,   39,  5, ANIM_ONCE
    #define ANIM_SKELETON_DEAD   18,   25,  10, ANIM_ONCE

    //enemy states
    enum E_SKELETON_ENEMY_STATES{E_SKELETON_ST_IDLE, E_SKELETON_ST_MOVING, E_SKELETON_ST_ATTACK, E_SKELETON_ST_JUMP, E_SKELETON_ST_HURT};   

    tEntity *player = entity_get(entity_get_player_id());

    //hurt signal
    if (this->signal == E_ENT_SIGNAL_HURT)
        this->state = E_SKELETON_ST_HURT;
    
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
        case E_SKELETON_ST_IDLE:            
            this->state++;
            this->dir = player->pos.x > this->pos.x;
        break;
        case E_SKELETON_ST_MOVING:            
            local->flag = false;

            if (this->spare)
            {
                enemy_patrol_ia(this, ftofix(SKELETON_VELOCITY), SKELETON_RANGE_PATROL);
                play_animation(&this->anim, ANIM_SKELETON_WALK);
            }
            else
                this->anim.frame = 0;
            
            //check range of player for attack
            if (in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, SKELETON_PLAYER_RANGE))
                this->state = E_SKELETON_ST_ATTACK;
            
            //check player jump for jump
            if (player->fixVel.y < 0 && in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, this->size.x))
            {
                this->ground = false;
                this->fixVel.y = -ftofix(SKELETON_JUMP_VEL);
                this->state = E_SKELETON_ST_JUMP;
            }            
        break;     
        case E_SKELETON_ST_ATTACK:
            if (play_animation(&this->anim, ANIM_SKELETON_ATACK))
            {
                this->state = E_SKELETON_ST_MOVING;
            }    
            
            //attack
            if (this->anim.frame == SKELETON_ATTACK_FRAME)
            {
                if (!local->flag)
                {
                    local->flag = true;
                    int16_t hitX = this->dir == E_ENT_DIR_LEFT ? -SKELETON_HITBOX_X_OFFSET_LEFT : this->size.x + SKELETON_HITBOX_X_OFFSET_RIGHT; 
                    sfx_play(enemySfx[E_SFX_ENEMY_SWORD], E_SFX_ENEMY_VOICE);
                    entity_create(E_ENT_CLASS_ENEMY, E_HITBOX_ENEMY_TYPE, (tVector){this->pos.x + hitX, this->pos.y}, this->dir, SKELETON_HITBOX_DURATION);
                }               
            }
            else
            {
                local->flag = false;
                //check player jump for jump
                if (player->fixVel.y < 0 && in_range(this->pos.x + (this->size.x * this->dir), player->pos.x, this->size.x))
                {
                    this->ground = false;
                    this->fixVel.y = -ftofix(SKELETON_JUMP_VEL);
                    this->state = E_SKELETON_ST_JUMP;
                }
            } 
        break;   
        case E_SKELETON_ST_JUMP:
            if (this->ground)
            {
                this->state = E_SKELETON_ST_IDLE;
            }
            play_animation(&this->anim, ANIM_SKELETON_JUMP);
        break;
        case E_SKELETON_ST_HURT:
            enemy_dead(this, ANIM_SKELETON_DEAD);            
        break;
    }
}

void enemy_spike_ball_update(tEntity *this, tDefaultEnemyLocalData *local)
{
    //enemy defines
    #define SPIKE_BALL_VELOCITY                  0.6
    #define SPIKE_BALL_DEFAULT_RANGE_PATROL      50

    //enemy states
    enum E_SPIKE_BALL_ENEMY_STATE{E_SPIKE_BALL_ST_MOVE};

    //check state
    switch (this->state)
    {
        case E_SPIKE_BALL_ST_MOVE:
            enemy_patrol_ia(this, ftofix(SPIKE_BALL_VELOCITY), this->spare != 0 ? this->spare : SPIKE_BALL_DEFAULT_RANGE_PATROL);
        break;
    }
}