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
#include "input.h"

#include "data/odata.h"

#define TRACE_FLAG  "[OBJECT]"

uint16_t numObjectInstances;                    //num of object instances
tObjectLocalData *objectDataList;               //list of object local data
BITMAP *objectResources[E_OBJECTS_TYPE_NUM];    //array of objects gfx resources
SAMPLE *objectSfx[E_SFX_OBJECT_NUM];            //array of objects sfx resources
DATAFILE_INDEX *objectDataFileIndex;            //object datafile index

tVector objectExplosion;                        //position of a object explosion (dynamite...)
bool egyptPuzzle;                               //status of egypt symbol (puzzle 1)
int8_t egyptPuzzle2[PUZZLE2_SYMBOL_NUM];         //array of combinations of egypt symbols (puzzle 2)

void object_system_init()
{
    //empty object list
    //free(objectDataList);
    objectDataList = NULL;
    //set number of entities
    numObjectInstances = 0;    

    //create data file index
    objectDataFileIndex = create_dat_index("objects.dat");

    //load general object sfx
    objectSfx[E_SFX_OBJECT_FULL_LIFE]   = load_dat_wav_indexed(objectDataFileIndex, POWERUP_WAV);
    objectSfx[E_SFX_OBJECT_EXTRA_LIVE]  = load_dat_wav_indexed(objectDataFileIndex, LIVE_WAV);
    
    //reset puzzle array
    memset(&egyptPuzzle2, E_EGYPT_SYMBOL_STATUS_INIT, sizeof(egyptPuzzle2));    

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

//function to add object to local object data. Returns the new array and increments the size counter
static tObjectLocalData* object_data_add(tObjectLocalData *array, uint16_t *listSize, uint8_t localDataType, void *data) {
    int newSize = *listSize + 1;
    
    //allocates one more element
    tObjectLocalData *temp = realloc(array, newSize * sizeof(tObjectLocalData));
    
    //check allocation
    if (temp == NULL) {
        abort_on_error("Can't assign memory for object entities\n");
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
            case E_OBJECT_DEFAULT_LOCAL_DATA_TYPE:
                MY_TRACE_FLAG("Allocating default object local data\n");
                MY_TRACE_FLAG("Memory address of data %p\n", array[*listSize].data);
            break;
            case E_OBJECT_PATH_PLATFORM_LOCAL_DATA_TYPE:
                MY_TRACE_FLAG("Allocating path platform object local data\n");
                MY_TRACE_FLAG("Memory address of data %p\n", array[*listSize].data);            
            break;
        }
    #endif

    //increment size counter
    (*listSize)++;
    
    return array;
}

//function to delete object local data from array with swap and pop (last array position moves to removed element)
static tObjectLocalData* object_data_remove(tObjectLocalData *array, uint16_t *listSize, uint16_t objectIndex) {
    //check object index bounds
    if (objectIndex < 0 || objectIndex >= *listSize) {
        //abort_on_error("Enemy index %d out of range\n", objectIndex);
        MY_TRACE_FLAG("ERROR: object index %d out of range\n", objectIndex);
        return array;
    }
    
    #if DEBUG_LOCAL_DATA_MEMORY
        switch(array[objectIndex].structureType)
        {
            case E_OBJECT_DEFAULT_LOCAL_DATA_TYPE:
                MY_TRACE_FLAG("Deallocating default object local data\n");
                MY_TRACE_FLAG("Memory address of data %p\n", array[objectIndex].data);
            break;
            case E_OBJECT_PATH_PLATFORM_LOCAL_DATA_TYPE:
                MY_TRACE_FLAG("Deallocating path platform object local data\n");
                MY_TRACE_FLAG("Memory address of data %p\n", array[objectIndex].data);                
            break;
        }
    #endif

    //free the allocated data of the object to delete    
    free(array[objectIndex].data);
    array[objectIndex].data = NULL;
    
    //get last index
    int16_t last_index = *listSize - 1;
    //MY_TRACE_FLAG("objectIndex %i last_index %i numObjectinstances %i\n", objectIndex, last_index, numObjectInstances);
    //if element isn't last, make the swap
    if (objectIndex != last_index) {
        //copy data from last element to remove element position
        array[objectIndex] = array[last_index];
        //MY_TRACE_FLAG("Copied object index %i to %i\n", last_index , objectIndex);        
    }
    
    //decrease the size counter (pop)
    (*listSize)--;

    //reduces memory space of the array
    if (*listSize > 0) 
    {
        tObjectLocalData *temp = realloc(array, (*listSize) * sizeof(tObjectLocalData));
        if (temp != NULL) array = temp;
    } 
    else 
    {
        free(array);
        array = NULL;
    }

    MY_TRACE_FLAG("Num object instances after destroy: %i\n", *listSize);

    return array;
}

//check object entity type to add the local data structure to local data list and increases instances number
void object_create(tEntity *entity)
{
    void *objectLocalData = NULL;
    uint8_t objectLocalDataType = E_OBJECT_DEFAULT_LOCAL_DATA_TYPE;

    //set object properties    
    switch (entity->entType)
    {
        case E_SOLID_OBJECT_TYPE:            
            switch (entity->spare)
            {
                case E_GAME_LEVEL_TUTORIAL:
                    load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, ROCK5_BMP);
                    entity->properties = E_ENT_PROP_NO_BREAKABLE;
                break;
                case E_GAME_LEVEL_JURASSIC:
                    load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, ROCK_BMP);
                break;
                case E_GAME_LEVEL_WEST:
                    load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, ROCK3_BMP);
                break;
                case E_GAME_LEVEL_EGYPT:
                    load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, ROCK4_BMP);
                break;
            }            
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){16, 16};
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
            switch (entity->spare)
            {
                case 0:
                    load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, END_BMP);
                    entity->spriteSize = (tVector){20, 25};
                break;
                case 1:
                    load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, END1_BMP);
                    entity->spriteSize = (tVector){30, 27};
                break;
                case 2:
                    load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, END2_BMP);
                    entity->spriteSize = (tVector){25, 23};
                break;
                case 3:
                    load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, END3_BMP);
                    entity->spriteSize = (tVector){20, 27};
                break;
            }            
            load_entity_wav_resources(&objectSfx[E_SFX_END_OBJECT], objectDataFileIndex, END_WAV);
            entity->img = objectResources[entity->entType];            
            entity->size = (tVector){8, 8};      
            entity->properties = E_ENT_PROP_NO_COLLISION;
        break;
        case E_CHECKPOINT_OBJECT_TYPE: 
        case E_DOOR_IN_OBJECT_TYPE:       
        case E_DOOR_OUT_OBJECT_TYPE:                    
            entity->img = NULL;
            entity->size = (tVector){16, 32};      
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
        case E_SCROLLMODE_OBJECT_TYPE:            
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
            entity->size = (tVector){entity->spare, 32};                                     
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
            load_entity_wav_resources(&objectSfx[E_SFX_SYMBOL], objectDataFileIndex, SYMBOL_WAV);
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
        case E_EGYPT_PLATFORM_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, EGYPTPLA_BMP);    
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){32, 16};
            entity->size = (tVector){32, 16};                                     
            entity->properties =  E_ENT_PROP_NO_PICKABLE | E_ENT_PROP_NO_BREAKABLE | E_ENT_PROP_PERSISTENT | E_ENT_PROP_NO_FLIP;                        
            collision_create_entity_points(entity);            
        break;
        case E_SPIKE_TRAP_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, SPKTRAP_BMP);
            load_entity_wav_resources(&objectSfx[E_SFX_OBJECT_FALL], objectDataFileIndex, ROCKFALL_WAV);            
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){32, 24};
            entity->size = (tVector){32, 8};      
            entity->axis = E_ENT_AXIS_UP;       
            collision_create_entity_points(entity);
            entity->properties = E_ENT_PROP_NO_PICKABLE | E_ENT_PROP_NO_HURT | E_ENT_PROP_NO_COLLISION | E_ENT_PROP_NO_BREAKABLE;
        break;
        case E_LANCE_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, LANCE_BMP);
            load_entity_wav_resources(&objectSfx[E_SFX_LANCE], objectDataFileIndex, LANCE_WAV);            
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){8, 48};
            entity->size = (tVector){8, 48};      
            entity->axis = E_ENT_AXIS_UP;       
            entity->properties = E_ENT_PROP_NO_PICKABLE | E_ENT_PROP_NO_HURT | E_ENT_PROP_NO_BREAKABLE;
        break;
        case E_SPIKE_TRAP_2_OBJECT_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, SPKTRAP2_BMP);
            load_entity_wav_resources(&objectSfx[E_SFX_OBJECT_FALL], objectDataFileIndex, ROCKFALL_WAV);            
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){16, 22};
            entity->size = (tVector){16, 16};      
            entity->axis = E_ENT_AXIS_UP;       
            collision_create_entity_points(entity);
            entity->properties = E_ENT_PROP_NO_PICKABLE | E_ENT_PROP_NO_HURT | E_ENT_PROP_NO_COLLISION | E_ENT_PROP_NO_BREAKABLE;
        break;
        case E_TRAP_FIRE_OBJECT_TYPE:
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, OTRAPA_BMP);
            load_entity_wav_resources(&objectSfx[E_SFX_OBJECT_ARROW], objectDataFileIndex, TRAPA_WAV);
            entity->img = objectResources[entity->entType]; 
            entity->spriteSize = (tVector){16, 16};                          
            entity->size = (tVector){16, 16};                     
        break;
        case E_DEBUG_START_OBJECT_TYPE:
            entity->img = NULL;
            entity->size = (tVector){16, 32};      
            entity->properties = E_ENT_PROP_NO_COLLISION;                        
        break;
        case E_MEDIEVAL_PLATFORM_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, MEDPLAT_BMP);    
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){32, 16};
            entity->size = (tVector){32, 16};                                                 
            entity->properties =  E_ENT_PROP_NO_PICKABLE | E_ENT_PROP_NO_BREAKABLE | E_ENT_PROP_PERSISTENT | E_ENT_PROP_NO_FLIP;                        
            collision_create_min_entity_points(entity);            
        break;
        case E_MEDIEVAL_PATH_TYPE:            
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, MEDPLAT_BMP);    
            entity->img = objectResources[entity->entType];
            entity->spriteSize = (tVector){32, 16};
            entity->size = (tVector){32, 16};                                                 
            entity->properties =  E_ENT_PROP_NO_PICKABLE | E_ENT_PROP_NO_BREAKABLE  | E_ENT_PROP_NO_FLIP;
            //custom local data
            objectLocalData = malloc(sizeof(tPathPlatformLocalData));
            objectLocalDataType = E_OBJECT_PATH_PLATFORM_LOCAL_DATA_TYPE;
        break;
        case E_PATH_OBJECT_TYPE:
            entity->size = (tVector){8, 8};
            entity->properties = E_ENT_PROP_NO_COLLISION;                                                 
        break;
        case E_CANNON_OBJECT_TYPE:
            load_entity_bmp_resources(&objectResources[entity->entType], objectDataFileIndex, CANNON_BMP);            
            load_entity_wav_resources(&objectSfx[E_SFX_CANNON], objectDataFileIndex, CANNON_WAV);
            entity->img = objectResources[entity->entType]; 
            entity->spriteSize = (tVector){50, 58};                          
            entity->size = (tVector){46, 24};
            entity->axis = E_ENT_AXIS_DOWN;         
            entity->properties = E_ENT_PROP_NO_PICKABLE || E_ENT_PROP_NO_BREAKABLE;                 
        break;
        default:
            abort_on_error("Object entity type (%i) not valid", entity->entType);
        break;
    }
    
    //if not custom local data type assigned
    if (objectLocalData == NULL)
        //allocate default object local data
        objectLocalData = malloc(sizeof(tDefaultObjectLocalData));
    
    //adds object local data to list
    objectDataList = object_data_add(objectDataList, &numObjectInstances, objectLocalDataType, objectLocalData);

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
    #if DEBUG_LOCAL_DATA_MEMORY
        MY_TRACE_FLAG("UPDATE Memory address of data %p\n", objectDataList[entity->entInstance].data);
    #endif

    switch (entity->entType)
    {
        case E_DEBUG_START_OBJECT_TYPE:
        break;
        case E_END_LEVEL_OBJECT_TYPE:
        case E_CHECKPOINT_OBJECT_TYPE:
        case E_STOP_SCROLL_OBJECT_TYPE:
        case E_BACKCOLOR_OBJECT_TYPE:
        case E_SYMBOL_HOLE_OBJECT_TYPE:
        case E_SCROLLMODE_OBJECT_TYPE:
        case E_DOOR_IN_OBJECT_TYPE:
        case E_DOOR_OUT_OBJECT_TYPE:                
            object_trigger_update(entity, (tDefaultObjectLocalData*)objectDataList[entity->entInstance].data);           
        break;
        case E_ITEM_OBJECT_TYPE:
            object_item_update(entity, (tDefaultObjectLocalData*)objectDataList[entity->entInstance].data);
        break;
        case E_WAGON_OBJECT_TYPE:
            object_wagon_update(entity, (tDefaultObjectLocalData*)objectDataList[entity->entInstance].data);
        break;
        case E_DYNAMITE_OBJECT_TYPE:
            object_dynamite_update(entity, (tDefaultObjectLocalData*)objectDataList[entity->entInstance].data);
        break;
        case E_BRIDGE_OBJECT_TYPE:
            object_bridge_update(entity, (tDefaultObjectLocalData*)objectDataList[entity->entInstance].data);
        break;
        case E_ROCK_EXPLOSION_OBJECT_TYPE:
            object_rock_explosion_update(entity, (tDefaultObjectLocalData*)objectDataList[entity->entInstance].data);
        break;
        case E_GAME_OVER_OBJECT_TYPE:
            object_game_over_update(entity, (tDefaultObjectLocalData*)objectDataList[entity->entInstance].data);
        break;
        case E_ROCK_FALL_OBJECT_TYPE:
        case E_SPIKE_FALL_OBJECT_TYPE:
        case E_SPIKE_TRAP_OBJECT_TYPE:
            object_fall_update(entity, (tDefaultObjectLocalData*)objectDataList[entity->entInstance].data);
        break;
        case E_QUICKSAND_OBJECT_TYPE:
            object_quick_sand_update(entity, (tDefaultObjectLocalData*)objectDataList[entity->entInstance].data);
        break;
        case E_TRAP_ARROW_OBJECT_TYPE:
        case E_TRAP_FIRE_OBJECT_TYPE:
            object_trap_arrow_update(entity, (tDefaultObjectLocalData*)objectDataList[entity->entInstance].data);
        break;                
        case E_EGYPT_PLATFORM_OBJECT_TYPE:
        case E_MEDIEVAL_PLATFORM_TYPE:
            object_platform_update(entity, (tDefaultObjectLocalData*)objectDataList[entity->entInstance].data);
        break;
        case E_LANCE_OBJECT_TYPE:
            object_lance_update(entity, (tDefaultObjectLocalData*)objectDataList[entity->entInstance].data);
        break;
        case E_SPIKE_TRAP_2_OBJECT_TYPE:
            object_spike_trap_2_update(entity, (tDefaultObjectLocalData*)objectDataList[entity->entInstance].data);
        break;
        case E_MEDIEVAL_PATH_TYPE:
            object_path_platform_update(entity, (tPathPlatformLocalData*)objectDataList[entity->entInstance].data);
        break;
        case E_PATH_OBJECT_TYPE:
        break;
        case E_CANNON_OBJECT_TYPE:             
            object_cannon_update(entity, (tDefaultObjectLocalData*)objectDataList[entity->entInstance].data);
        break;
        default:
            object_solid_update(entity, (tDefaultObjectLocalData*)objectDataList[entity->entInstance].data);
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
            ((tDefaultObjectLocalData*)objectDataList[entity->entInstance].data)->timer = 0;
            ((tDefaultObjectLocalData*)objectDataList[entity->entInstance].data)->flag = 0;

            //check puzzle
            if (entity->dir)
            {
                for (uint8_t i = 0; i < PUZZLE1_NUM_DOOR_TILES; i++)
                {
                    map_change_tile((tVector){PUZZLE1_START_DOOR_X_TILE, PUZZLE1_START_DOOR_Y_TILE - i}, PUZZLE_DOOR_TILE_ID, 0x00);    
                }
                egyptPuzzle = false;
            }
            else
            {
                for (uint8_t i = 0; i < PUZZLE2_NUM_DOOR_TILES; i++)
                {
                    map_change_tile((tVector){PUZZLE2_START_DOOR_X_TILE, PUZZLE2_START_DOOR_Y_TILE - i}, PUZZLE_DOOR_TILE_ID, 0x00);    
                }            
                memset(&egyptPuzzle2, E_EGYPT_SYMBOL_STATUS_INIT, sizeof(egyptPuzzle2));
            }
        break;    
        case E_DEBUG_START_OBJECT_TYPE:
            #if DEBUGMODE
                //set player init position
                entity_get(entity_get_player_id())->initPos = entity->pos;
                entity_get(entity_get_player_id())->initDir = entity->dir;
            #endif
        break;
        case E_MEDIEVAL_PATH_TYPE:
            ((tPathPlatformLocalData*)objectDataList[entity->entInstance].data)->currentPoint = 0;
            ((tPathPlatformLocalData*)objectDataList[entity->entInstance].data)->pathPos.x = 0;
            ((tPathPlatformLocalData*)objectDataList[entity->entInstance].data)->pathPos.y = 0;
        break;
        default:            
            ((tDefaultObjectLocalData*)objectDataList[entity->entInstance].data)->timer = 0;
            ((tDefaultObjectLocalData*)objectDataList[entity->entInstance].data)->flag = 0;    
        break;        
    }
}

void object_destroy(tEntity *entity)
{
    MY_TRACE_FLAG("Destroying object id: %i, instance:%i\n", entity->id, entity->entInstance);

    //remove object instance from list
    objectDataList = object_data_remove(objectDataList, &numObjectInstances, entity->entInstance);        
}

void object_solid_update(tEntity *this, tDefaultObjectLocalData *local)
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
                
            if (this->entType == E_EGYPT_SYMBOL_OBJECT_TYPE && this->anim.frame < PUZZLE2_SYMBOL_NUM)
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
            {
                 egyptPuzzle2[this->spare] = 0;
                 this->anim.frame = this->spare;
            }

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
                                collision_apply_dir(this, colDir, E_COLLISION_NO_BOUNCE);

                                //if the collision is between egypt symbol and a symbol hole object
                                if (this->entType == E_EGYPT_SYMBOL_OBJECT_TYPE && checkEntity->entClass == E_ENT_CLASS_TRIGGER && checkEntity->entType == E_SYMBOL_HOLE_OBJECT_TYPE && colDir && this->ground && abs(this->fixVel.x) < ftofix(0.1))
                                {
                                    //check number of puzzle
                                    if (this->dir)
                                        egyptPuzzle = this->spare == checkEntity->spare ? true : false;
                                    else
                                    {    
                                        //check if combination is correct
                                        if (this->spare == checkEntity->spare)
                                            egyptPuzzle2[this->spare] = E_EGYPT_SYMBOL_STATUS_OK;                            
                                        else
                                            egyptPuzzle2[this->spare] = E_EGYPT_SYMBOL_STATUS_NOT_OK;
                                    }
                                    //highlight symbol
                                    this->anim.frame = this->spare + PUZZLE2_SYMBOL_NUM;
                                }                                
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
            {
                this->state = E_SOLID_ST_IDLE;            
                //if egypt symbol, play sound if it's on hole    
                if (this->entType == E_EGYPT_SYMBOL_OBJECT_TYPE && this->anim.frame >= PUZZLE2_SYMBOL_NUM)
                    sfx_play(objectSfx[E_SFX_SYMBOL], E_SFX_OBJECT_VOICE);
            }
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

void object_item_update(tEntity *this, tDefaultObjectLocalData *local)
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

void object_trigger_update(tEntity *this, tDefaultObjectLocalData *local)
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
                sfx_play(objectSfx[E_SFX_END_OBJECT], E_SFX_OBJECT_VOICE);
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
        case E_SCROLLMODE_OBJECT_TYPE:
            scroll_set_scroll_mode(this->spare);            
        break;
        case E_DOOR_IN_OBJECT_TYPE:
            //if collision with player and press UP
            if (collision_check_entity(this, entity_get(entity_get_player_id()), E_CHECK_PROCESS_INFOONLY) && input_key_down(E_G_KEY_UP))
            {
                game.doorId = this->spare;
            }    
        break;
        case E_SYMBOL_HOLE_OBJECT_TYPE:
            //object definitions
            #define SYMBOL_HOLE_CORRECT_DELAY       80
            #define SYMBOL_HOLE_OPEN_DOOR_CADENCE   20
        
            //object states
            enum E_SYMBOL_HOLE_OBJECT_STATES{E_SYMBOL_HOLE_ST_IDLE, E_SYMBOL_HOLE_ST_OK, E_SYMBOL_HOLE_ST_OPEN_DOOR};    

            switch (this->state)
            {
                case E_SYMBOL_HOLE_ST_IDLE:
                    //check puzzle number    
                    if (this->dir)
                    {
                        if (egyptPuzzle)
                        {
                            this->state = E_SYMBOL_HOLE_ST_OK;    
                            sfx_play(objectSfx[E_SFX_PUZZLE_OK], E_SFX_OBJECT_VOICE);        
                        }
                    }
                    else
                    {
                        //show_debug("puzzle %i:%i", this->spare,egyptPuzzle2[this->spare]);
                        uint8_t symbolsOK = 0;
                        uint8_t symbolsUsed = 0;                    
                        //check status of symbols
                        for (uint8_t i = 0; i < PUZZLE2_SYMBOL_NUM; i++)
                        {
                            if (egyptPuzzle2[i] != E_EGYPT_SYMBOL_STATUS_INIT)
                                symbolsUsed++;
                            if (egyptPuzzle2[i] == E_EGYPT_SYMBOL_STATUS_OK)                        
                                symbolsOK++;                            
                        }                    
                        //check puzzle completed
                        if (symbolsOK == PUZZLE2_SYMBOL_NUM)
                        {
                            this->state = E_SYMBOL_HOLE_ST_OK;    
                            sfx_play(objectSfx[E_SFX_PUZZLE_OK], E_SFX_OBJECT_VOICE);                    
                        }
                        else if (symbolsUsed == PUZZLE2_SYMBOL_NUM)
                        {                                    
                            sfx_play(objectSfx[E_SFX_PUZZLE_NO], E_SFX_OBJECT_VOICE);                                    
                            scroll_shake_camera();          
                            //reinit the incorrect combinations
                            for (uint8_t i = 0; i < PUZZLE2_SYMBOL_NUM; i++)
                            {                         
                                egyptPuzzle2[i] = egyptPuzzle2[i] == E_EGYPT_SYMBOL_STATUS_OK ? E_EGYPT_SYMBOL_STATUS_OK : E_EGYPT_SYMBOL_STATUS_INIT;
                            }
                        }
                    }
                break;                
                case E_SYMBOL_HOLE_ST_OK:
                    if (local->timer >= SYMBOL_HOLE_CORRECT_DELAY)
                    {
                        local->timer = 0;
                        this->state++;
                    }
                    else
                        local->timer += clock_tick_get();
                break;
                case E_SYMBOL_HOLE_ST_OPEN_DOOR:
                    //check puzzle
                    if (this->dir)
                    {
                        if (clock_counter_check(SYMBOL_HOLE_OPEN_DOOR_CADENCE))
                        {
                            sfx_play(objectSfx[E_SFX_WAGON], E_SFX_OBJECT_VOICE);
                            map_change_tile((tVector){PUZZLE1_START_DOOR_X_TILE, PUZZLE1_START_DOOR_Y_TILE - local->timer}, PUZZLE_NO_DOOR_TILE_ID, E_TILE_PROP_NO_SOLID);
                            local->timer++;
                        }

                        if (local->timer >= PUZZLE1_NUM_DOOR_TILES)
                            this->state++;  //go to state not defined (do nothing)
                    }   
                    else
                    { 
                        if (clock_counter_check(SYMBOL_HOLE_OPEN_DOOR_CADENCE))
                        {
                            sfx_play(objectSfx[E_SFX_WAGON], E_SFX_OBJECT_VOICE);
                            map_change_tile((tVector){PUZZLE2_START_DOOR_X_TILE, PUZZLE2_START_DOOR_Y_TILE - local->timer}, PUZZLE_NO_DOOR_TILE_ID, E_TILE_PROP_NO_SOLID);
                            local->timer++;
                        }

                        if (local->timer >= PUZZLE2_NUM_DOOR_TILES)
                            this->state++;  //go to state not defined (do nothing)
                    }
                break;                
            }
        break;
    }
}

void object_wagon_update(tEntity *this, tDefaultObjectLocalData *local)
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

void object_dynamite_update(tEntity *this, tDefaultObjectLocalData *local)
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

void object_bridge_update(tEntity *this, tDefaultObjectLocalData *local)
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

void object_rock_explosion_update(tEntity *this, tDefaultObjectLocalData *local)
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


void object_fall_update(tEntity *this, tDefaultObjectLocalData *local)
{
    //object defines
    #define OBJECT_FALL_FALL_VEL_Y       3
    #define OBJECT_FALL_PLAYER_RANGE_X   30
    
    //object states
    enum E_OBJECT_FALL_OBJECT_STATES{E_OBJECT_FALL_ST_IDLE, E_OBJECT_FALL_ST_FALL, E_OBJECT_FALL_ST_BREAK, E_OBJECT_FALL_ST_STILL};
    
    //object animations
    #define ANIM_OBJECT_FALL_BREAK                1,  2, 10, ANIM_ONCE

    //get player
    tEntity *player = entity_get(entity_get_player_id());

    if (this->signal == E_ENT_SIGNAL_AWAKE)
    {
        this->signal = E_ENT_SIGNAL_NONE;
        this->pos = this->initPos;
        this->fixPos = vector2fixvector(this->pos);
        this->state = 0;        
    }

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
            {
                if (CHECK_FLAG(this->properties, E_ENT_PROP_NO_BREAKABLE))
                    this->state = E_OBJECT_FALL_ST_STILL;
                else
                    this->state = E_OBJECT_FALL_ST_BREAK;            
            }
            else if (collision_check_entity(this, player, E_CHECK_PROCESS_INFOONLY))
            {
                //hurt player if collided
                player->signal = E_ENT_SIGNAL_HURT;
                if (!CHECK_FLAG(this->properties, E_ENT_PROP_NO_BREAKABLE))
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
        case E_OBJECT_FALL_ST_STILL:
            //stop object
            this->fixVel.x = 0;
            this->fixVel.y = 0;                                                        
        break;
    }
}

void object_quick_sand_update(tEntity *this, tDefaultObjectLocalData *local)
{
    //object defines
    #define QUICK_SAND_VELOCITY              0.6 //0.36
    
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

void object_trap_arrow_update(tEntity *this, tDefaultObjectLocalData *local)
{
    //object defines
    #define TRAP_ARROW_DEFAULT_TIMER    200   

    //object animations
    #define ANIM_TRAP_ARROW_IDLE_FRAME   0
    #define ANIM_TRAP_ARROW_SHOOT        1,   1, 20,  ANIM_ONCE
    
    //object states
    enum E_TRAP_ARROW_OBJECT_STATES{E_TRAP_ARROW_INIT_DELAY, E_TRAP_ARROW_ST_IDLE, E_TRAP_ARROW_ST_SHOOT};

    switch (this->state)
    {
        case E_TRAP_ARROW_INIT_DELAY:            
            if (local->timer >= this->spare)
            {
                this->state++;                
                local->timer = 0;
                local->flag = false;
            }
            else if (scroll_position_on_region(this->pos))
                local->timer += clock_tick_get();
            
            this->anim.frame = ANIM_TRAP_ARROW_IDLE_FRAME;
        break;
        case E_TRAP_ARROW_ST_IDLE:            
            if (local->timer >= TRAP_ARROW_DEFAULT_TIMER)
            {
                this->state++;                
                local->timer = 0;
                local->flag = false;
            }
            else if (scroll_position_on_region(this->pos))
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
                switch (this->entType)
                {
                    case E_TRAP_ARROW_OBJECT_TYPE:
                        entity_create(E_ENT_CLASS_ENEMY, E_TRAP_ARROW_ENEMY_TYPE, (tVector){this->pos.x, this->pos.y + 8}, this->dir, this->spare);
                    break;
                    case E_TRAP_FIRE_OBJECT_TYPE:
                        entity_create(E_ENT_CLASS_ENEMY, E_TRAP_FIRE_ENEMY_TYPE, (tVector){this->pos.x, this->pos.y + 0}, this->dir, this->spare);
                    break;
                }
                
            }
        break;
    }    
}

void object_game_over_update(tEntity *this, tDefaultObjectLocalData *local)
{
    //object animation
    #define ANIM_GAME_OVER     0,   9, 10,  ANIM_LOOP
    
    play_animation(&this->anim, ANIM_GAME_OVER);    
    
}

void object_platform_update(tEntity *this, tDefaultObjectLocalData *local)
{
    //object defines
    #define PLATFORM_VELOCITY               0.6
    #define PLATFORM_MOVE_TILES_X           3 * 16
    #define PLATFORM_MOVE_TILES_Y           5 * 16
    
    //object states
    enum E_PLATFORM_OBJECT_STATES{E_PLATFORM_ST_IDLE, E_PLATFORM_ST_MOVE};

    //only check collisions on move patrol
    uint8_t colDir = 0;
    if (CHECK_FLAG(this->spare,E_PLATFORM_TYPE_MOVE_PATROL))
    {
        //terrain collisions
        this->ground = false;
        //check all the entity collision points    
        for (uint8_t i = 0; i < E_NUM_COL_POINTS; i++)
        {                
            //check collision tile for collision point
            colDir = collision_check_tile(this, i);        
            //apply collision direction
            collision_apply_dir(this, colDir, E_COLLISION_NO_BOUNCE);       
            
            //change direction on collision
            if (this->dir == E_ENT_DIR_LEFT || this->dir == E_ENT_DIR_RIGHT)
            {            
                //change direction if horizontal collision
                if (colDir == E_COLLISION_DIR_RIGHT || colDir == E_COLLISION_DIR_LEFT)
                {    
                    this->dir = !this->dir;
                    break;
                }
            }
            if (this->dir == E_ENT_DIR_DOWN || this->dir == E_ENT_DIR_UP)
            {            
                //change direction if vertical collision
                if (colDir == E_COLLISION_DIR_UP || colDir == E_COLLISION_DIR_DOWN)
                {
                    this->dir = this->dir == E_ENT_DIR_DOWN ? E_ENT_DIR_UP : E_ENT_DIR_DOWN;
                    break;
                }
            }                    
        }
    }

    switch (this->state)
    {
        case E_PLATFORM_ST_IDLE:
            //reset velocity
            this->fixVel.y = itofix(0); 

            //if player on this platform  or not wait player platform type          
            if (collision_get_player_platform_id() == this->id || !CHECK_FLAG(this->spare, E_PLATFORM_TYPE_WAIT_PLAYER))
            {
                this->state++;                
            }
        break;
        case E_PLATFORM_ST_MOVE:            
            //check patrol tile range to change direction
            if (CHECK_FLAG(this->spare,E_PLATFORM_TYPE_TILE_RANGE) && CHECK_FLAG(this->spare,E_PLATFORM_TYPE_MOVE_PATROL) && colDir == E_COLLISION_DIR_NONE)
            {
                //horizontal patrol
                if (this->dir == E_ENT_DIR_LEFT || this->dir == E_ENT_DIR_RIGHT)
                {
                    if ((this->dir && this->pos.x > (this->initPos.x + PLATFORM_MOVE_TILES_X)) || (!this->dir && this->pos.x < (this->initPos.x - PLATFORM_MOVE_TILES_X)))
                        this->dir = !this->dir; 
                        
                }
                //vertical patrol
                if (this->dir == E_ENT_DIR_DOWN || this->dir == E_ENT_DIR_UP)
                {
                    if ((this->dir == E_ENT_DIR_DOWN && this->pos.y > (this->initPos.y + PLATFORM_MOVE_TILES_Y)) || (this->dir == E_ENT_DIR_UP && this->pos.y < (this->initPos.y - PLATFORM_MOVE_TILES_Y)))
                        this->dir = this->dir == E_ENT_DIR_DOWN ? E_ENT_DIR_UP : E_ENT_DIR_DOWN;
                }
            }

            //apply linear movement
            int16_t nextPos;

            //horizontal
            if (this->dir == E_ENT_DIR_LEFT || this->dir == E_ENT_DIR_RIGHT)
            {
                this->fixVel.x = this->dir ? ftofix(PLATFORM_VELOCITY) : -ftofix(PLATFORM_VELOCITY);
                //calculate next integer position (entity update do this)
                nextPos = fixtoi(this->fixPos.x + fixmul(this->fixVel.x, ftofix(deltaTime)));
                //adds to player x position the integer part of platform delta movement                
                if (collision_get_player_platform_id() == this->id)            
                    entity_get(entity_get_player_id())->fixPos.x += itofix((nextPos - this->pos.x));
            }
            //vertical
            if (this->dir == E_ENT_DIR_DOWN || this->dir == E_ENT_DIR_UP)
            {
                this->fixVel.y = this->dir == E_ENT_DIR_DOWN ? ftofix(PLATFORM_VELOCITY) : -ftofix(PLATFORM_VELOCITY);
                //calculate next integer position (entity update do this)
                nextPos = fixtoi(this->fixPos.y + fixmul(this->fixVel.y, ftofix(deltaTime)));
                //adds to player x position the integer part of platform delta movement
                if (collision_get_player_platform_id() == this->id)            
                    entity_get(entity_get_player_id())->fixPos.y += itofix((nextPos - this->pos.y) + 1);
            }
        break;
    }
}

void object_lance_update(tEntity *this, tDefaultObjectLocalData *local)
{
    //object defines
    #define LANCE_MOVE_VEL_Y       -4.2
    #define LANCE_RETURN_VEL_Y     0.6
    #define LANCE_DEFAULT_WAIT     200
    #define LANCE_WAIT_RETURN      200
    #define LANCE_SHOW_TIP_Y       4 

    //object states
    enum E_LANCE_OBJECT_STATES{E_LANCE_ST_INIT_DELAY, E_LANCE_ST_IDLE, E_LANCE_ST_MOVE, E_LANCE_ST_WAIT, E_LANCE_ST_RETURN};
    
    //get player
    tEntity *player = entity_get(entity_get_player_id());

    switch (this->state)
    {
        case E_LANCE_ST_INIT_DELAY:
            SET_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            //show tip of lance
            this->fixPos.y = this->dir ? itofix(this->initPos.y + LANCE_SHOW_TIP_Y) : itofix(this->initPos.y - LANCE_SHOW_TIP_Y);    
            this->anim.frame = this->dir;
            
            //wait spare time
            if (local->timer >= this->spare)
            {
                this->state++;                
                local->timer = 0;                
            }
            else if (scroll_position_on_region(this->pos))
                local->timer += clock_tick_get();
        break;
        case E_LANCE_ST_IDLE:
            //stop object
            this->fixVel.x = 0;
            this->fixVel.y = 0;
            //show tip of lance
            this->fixPos.y = this->dir ? itofix(this->initPos.y + LANCE_SHOW_TIP_Y) : itofix(this->initPos.y - LANCE_SHOW_TIP_Y);

            SET_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);

            if (local->timer >= LANCE_DEFAULT_WAIT)
            {
                this->state++;
                local->timer = 0;
                CLEAR_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
                this->fixPos = vector2fixvector(this->initPos);
            }
            else
                local->timer += clock_tick_get();

            this->anim.frame = this->dir;
        break;
        case E_LANCE_ST_MOVE:
            this->fixVel.y = this->dir ? -ftofix(LANCE_MOVE_VEL_Y) : ftofix(LANCE_MOVE_VEL_Y);
            
            if (collision_check_entity(this, player, E_CHECK_PROCESS_INFOONLY))
            {
                //hurt player if collided
                player->signal = E_ENT_SIGNAL_HURT;
                //to prevent lance crush player, disable collision
                SET_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            }

            if ((this->pos.y <= (this->initPos.y - this->size.y) && !this->dir) ||
                (this->pos.y >= (this->initPos.y + this->size.y) && this->dir))
            {
                sfx_play(objectSfx[E_SFX_LANCE], E_SFX_OBJECT_VOICE);
                this->state++;
            }
        break;
        case E_LANCE_ST_WAIT:
            //stop object
            this->fixVel.x = 0;
            this->fixVel.y = 0; 
            this->fixPos.y = this->dir ? itofix(this->initPos.y + this->size.y) : itofix(this->initPos.y - this->size.y);

            //restore collision when player touch ground after hurt
            if (player->ground)
                CLEAR_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);

            if (local->timer >= LANCE_WAIT_RETURN)
            {
                this->state++;
                local->timer = 0;
                CLEAR_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            }
            else
                local->timer += clock_tick_get();

        break;
        case E_LANCE_ST_RETURN:
            this->fixVel.y = this->dir ? -ftofix(LANCE_RETURN_VEL_Y) : ftofix(LANCE_RETURN_VEL_Y);
            
            if ((this->pos.y >= this->initPos.y - LANCE_SHOW_TIP_Y && !this->dir) ||
                (this->pos.y <= this->initPos.y + LANCE_SHOW_TIP_Y && this->dir))
                this->state = E_LANCE_ST_IDLE;
        break;
    }
}

void object_spike_trap_2_update(tEntity *this, tDefaultObjectLocalData *local)
{
    //object defines
    #define SPIKE_TRAP_2_CADENCE          120
    #define SPIKE_TRAP_2_Y_MOVE           16

    //object states
    enum E_SPIKE_TRAP_2_OBJECT_STATES{E_SPIKE_TRAP_2_ST_IDLE, E_SPIKE_TRAP_2_ST_MOVE, E_SPIKE_TRAP_2_ST_STOP};
    
    //get player
    tEntity *player = entity_get(entity_get_player_id());   

    switch (this->state)
    {
        case E_SPIKE_TRAP_2_ST_IDLE:
            if (clock_counter_check(SPIKE_TRAP_2_CADENCE) && (scroll_position_on_region(this->pos)))
            {
                this->state++;
            }

            //check only down point    
            if (collision_check_tile(this, E_COLPOINT_DOWN_L) || collision_check_tile(this, E_COLPOINT_DOWN_R))            
            {
                    this->state = E_SPIKE_TRAP_2_ST_STOP;
                
            }
            if (collision_check_entity(this, player, E_CHECK_PROCESS_INFOONLY))
            {
                //hurt player if collided
                player->signal = E_ENT_SIGNAL_HURT;
            }
            
            this->anim.frame = 0;        
        break;
        case E_SPIKE_TRAP_2_ST_MOVE:
            this->fixPos.y += itofix(SPIKE_TRAP_2_Y_MOVE);
            this->state = E_SPIKE_TRAP_2_ST_IDLE;
            sfx_play(objectSfx[E_SFX_WAGON], E_SFX_OBJECT_VOICE);
        break;
        case E_SPIKE_TRAP_2_ST_STOP:
            if (collision_check_entity(this, player, E_CHECK_PROCESS_INFOONLY))
            {
                //hurt player if collided
                player->signal = E_ENT_SIGNAL_HURT;
            }    
        break;
    }
}

void object_path_platform_update(tEntity *this, tPathPlatformLocalData *local)
{
    //object defines
    #define PLATFORM_VELOCITY               0.6
    #ifdef DEBUGMODE
        #define DEBUG_PLATFORM_PATH         0
    #endif

    //object states
    enum E_PLATFORM_OBJECT_STATES{E_PLATFORM_ST_IDLE, E_PLATFORM_ST_GET_POINT, E_PLATFORM_ST_MOVE_TO_POINT, E_PLATFORM_ST_STOP};

    switch (this->state)
    {
        case E_PLATFORM_ST_IDLE:
            
            //reset velocity
            this->fixVel.x = itofix(0); 
            this->fixVel.y = itofix(0); 

            //wait for player
            if (collision_get_player_platform_id() == this->id)
            {
                this->state++;                
            }
        break;     
        case E_PLATFORM_ST_GET_POINT:
            //reset velocity
            this->fixVel.x = itofix(0); 
            this->fixVel.y = itofix(0);         
            
            //get the next path point (spare is the platform id and flag is current point number (checking with path dir attribute))                
            uint8_t numEntities = entities_get_num();
            tEntity *checkEntity;
            bool foundPoint = false;
            for (uint8_t i = 0; i < numEntities; i++)
            {
                checkEntity = entity_get(i);
                if (checkEntity->entClass == E_ENT_CLASS_PLATFORM && checkEntity->entType == E_PATH_OBJECT_TYPE && checkEntity->spare == this->spare && checkEntity->dir == local->currentPoint)
                {
                    foundPoint = true;
                    //save the actual point position
                    local->pathPos.x = checkEntity->pos.x;
                    local->pathPos.y = checkEntity->pos.y;
                    //check point direction (don't allow diagonal movement)
                    if (this->pos.x > checkEntity->pos.x) 
                        this->dir = E_ENT_DIR_LEFT;
                    else if (this->pos.x < checkEntity->pos.x) 
                        this->dir = E_ENT_DIR_RIGHT;
                    else if (this->pos.y < checkEntity->pos.y) 
                        this->dir = E_ENT_DIR_DOWN;
                    else if (this->pos.y > checkEntity->pos.y) 
                        this->dir = E_ENT_DIR_UP;
                }
            }

            if (foundPoint)
                this->state++;
            else
                this->state = E_PLATFORM_ST_STOP;

            #if DEBUG_PLATFORM_PATH
                MY_TRACE_FLAG("Path checkpoint x: %i y: %i dir: %i\n", local->pathPos.x, local->pathPos.y, this->dir);
            #endif                
        break;   
        case E_PLATFORM_ST_MOVE_TO_POINT:
            int16_t nextPos;    
            
            //horizontal
            if (this->dir == E_ENT_DIR_LEFT || this->dir == E_ENT_DIR_RIGHT)
            {
                //apply linear velocity
                this->fixVel.x = this->dir ? ftofix(PLATFORM_VELOCITY) : -ftofix(PLATFORM_VELOCITY);
                
                //calculate next integer position (entity update do this)
                nextPos = fixtoi(this->fixPos.x + fixmul(this->fixVel.x, ftofix(deltaTime)));
                //adds to player x position the integer part of platform delta movement                
                if (collision_get_player_platform_id() == this->id)            
                    entity_get(entity_get_player_id())->fixPos.x += itofix((nextPos - this->pos.x));
            }
            //vertical
            if (this->dir == E_ENT_DIR_DOWN || this->dir == E_ENT_DIR_UP)
            {
                //apply linear velocity
                this->fixVel.y = this->dir == E_ENT_DIR_DOWN ? ftofix(PLATFORM_VELOCITY) : -ftofix(PLATFORM_VELOCITY);
                
                //calculate next integer position (entity update do this)
                nextPos = fixtoi(this->fixPos.y + fixmul(this->fixVel.y, ftofix(deltaTime)));
                //adds to player x position the integer part of platform delta movement
                if (collision_get_player_platform_id() == this->id)            
                    entity_get(entity_get_player_id())->fixPos.y += itofix((nextPos - this->pos.y) + 1);
            }
            
            //check reach position
            switch(this->dir)
            {
                case E_ENT_DIR_LEFT:
                    if (this->pos.x <= local->pathPos.x)
                    {
                        this->fixPos.x = itofix(local->pathPos.x);
                        this->fixVel.x = itofix(0);
                        local->currentPoint++;
                        this->state = E_PLATFORM_ST_GET_POINT;                        
                    }
                break;
                case E_ENT_DIR_RIGHT:
                    if (this->pos.x >= local->pathPos.x)
                    {
                        this->fixPos.x = itofix(local->pathPos.x);
                        this->fixVel.x = itofix(0);
                        local->currentPoint++;
                        this->state = E_PLATFORM_ST_GET_POINT;
                    }
                break;
                case E_ENT_DIR_DOWN:
                    if (this->pos.y >= local->pathPos.y)
                    {
                        this->fixPos.y = itofix(local->pathPos.y);
                        this->fixVel.y = itofix(0);
                        local->currentPoint++;
                        this->state = E_PLATFORM_ST_GET_POINT;
                    }
                break;
                case E_ENT_DIR_UP:
                    if (this->pos.y <= local->pathPos.y)
                    {
                        this->fixPos.y = itofix(local->pathPos.y);
                        this->fixVel.y = itofix(0);
                        local->currentPoint++;
                        this->state = E_PLATFORM_ST_GET_POINT;
                    }
                break;
            }
        break;
        case E_PLATFORM_ST_STOP:
        break;
    }    
    #if DEBUG_PLATFORM_PATH
        show_debug("Num: %i Path x: %i y: %i\n", local->currentPoint, local->pathPos.x, local->pathPos.y);
    #endif
}

void object_cannon_update(tEntity *this, tDefaultObjectLocalData *local)
{
    //object defines
    #define CANNON_DEFAULT_TIMER    200   

    //object animations
    #define ANIM_CANNON_IDLE_FRAME   0
    #define ANIM_CANNON_SHOOT        0,   8, 6,  ANIM_ONCE
    
    //object states
    enum E_CANNON_OBJECT_STATES{E_CANNON_INIT_DELAY, E_CANNON_ST_IDLE, E_CANNON_ST_SHOOT};

    switch (this->state)
    {
        case E_CANNON_INIT_DELAY:            
            if (local->timer >= this->spare)
            {
                this->state++;                
                local->timer = 0;
                local->flag = false;
            }
            else if (scroll_position_on_region(this->pos))
                local->timer += clock_tick_get();
            
            this->anim.frame = ANIM_CANNON_IDLE_FRAME;
        break;
        case E_CANNON_ST_IDLE:            
            if (local->timer >= CANNON_DEFAULT_TIMER)
            {
                this->state++;                
                local->timer = 0;
                local->flag = false;
            }
            else 
                local->timer += clock_tick_get();
            
            this->anim.frame = ANIM_CANNON_IDLE_FRAME;

        break;
        case E_CANNON_ST_SHOOT:
            //TODO: after entity create must not modify any entity structure data in case pointer moves!            
            if (play_animation(&this->anim, ANIM_CANNON_SHOOT))
            {
                this->state--;
            }    
            
            if (!local->flag)
            {
                local->flag = true; //it's important to set the local flag before entity creation in case pointer moves
                sfx_play(objectSfx[E_SFX_CANNON], E_SFX_OBJECT_VOICE);                
                entity_create(E_ENT_CLASS_ENEMY, E_CANNONBALL_ENEMY_TYPE, (tVector){this->pos.x, this->pos.y + 0}, this->dir, this->spare);
            }
        break;
    }    
}

void object_trace(tEntity *this)
{
    entity_trace(this);
}