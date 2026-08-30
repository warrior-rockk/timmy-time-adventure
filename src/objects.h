/********************************************************************
* Objects entities (test)
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#ifndef _H_OBJECTS_
#define _H_OBJECTS_

#include "entity.h"

//general object defines
#define OBJECT_THROW_VEL_X          2
#define OBJECT_THROW_VEL_Y          -2

//general object animations
#define ANIM_OBJECT_BREAK                1,  2, 10, ANIM_ONCE

//egypt puzzle definitions
#define PUZZLE1_NUM_DOOR_TILES      8
#define PUZZLE1_START_DOOR_X_TILE   242
#define PUZZLE1_START_DOOR_Y_TILE   28
#define PUZZLE2_NUM_DOOR_TILES      7
#define PUZZLE2_START_DOOR_X_TILE   214
#define PUZZLE2_START_DOOR_Y_TILE   17
#define PUZZLE2_SYMBOL_NUM          3
#define PUZZLE_DOOR_TILE_ID         34
#define PUZZLE_NO_DOOR_TILE_ID      131


//object types
enum E_OBJECT_TYPE
{
    E_SOLID_OBJECT_TYPE,
    E_EGG_OBJECT_TYPE,
    E_END_LEVEL_OBJECT_TYPE,
    E_CHECKPOINT_OBJECT_TYPE,
    E_ITEM_OBJECT_TYPE,
    E_STOP_SCROLL_OBJECT_TYPE,
    E_WAGON_OBJECT_TYPE,
    E_DYNAMITE_OBJECT_TYPE,
    E_BACKCOLOR_OBJECT_TYPE,
    E_BRIDGE_OBJECT_TYPE,
    E_ROCK_EXPLOSION_OBJECT_TYPE,
    E_GAME_OVER_OBJECT_TYPE,
    E_ROCK_FALL_OBJECT_TYPE,
    E_SPIKE_FALL_OBJECT_TYPE,
    E_BOX_OBJECT_TYPE,
    E_HIDDEN_OBJECT_TYPE,
    E_QUICKSAND_OBJECT_TYPE,
    E_TRAP_ARROW_OBJECT_TYPE,
    E_EGYPT_SYMBOL_OBJECT_TYPE,
    E_SYMBOL_HOLE_OBJECT_TYPE,
    E_EGYPT_PLATFORM_OBJECT_TYPE,
    E_SPIKE_TRAP_OBJECT_TYPE,
    E_LANCE_OBJECT_TYPE,
    E_SCROLLMODE_OBJECT_TYPE,
    E_DOOR_IN_OBJECT_TYPE,
    E_DOOR_OUT_OBJECT_TYPE,
    E_SPIKE_TRAP_2_OBJECT_TYPE,
    E_TRAP_FIRE_OBJECT_TYPE,
    E_DEBUG_START_OBJECT_TYPE,
    E_MEDIEVAL_PLATFORM_TYPE,
    E_MEDIEVAL_PATH_TYPE,
    E_PATH_OBJECT_TYPE,
    E_CANNON_OBJECT_TYPE,
    E_TORCH_DROP_OBJECT_TYPE,
    E_LAVA_DROP_OBJECT_TYPE,

    E_OBJECTS_TYPE_NUM,
};

//types of object local data structures
enum E_OBJECT_LOCAL_DATA_TYPE
{
    E_OBJECT_DEFAULT_LOCAL_DATA_TYPE,
    E_OBJECT_PATH_PLATFORM_LOCAL_DATA_TYPE,
};

//item type uses spare to set type
enum E_ITEMS_TYPE
{
    E_ITEM_EXTRA_LIFE,
    E_ITEM_FULL_LIFE,
    E_ITEMS_NUM,
};

//default object local data
typedef struct 
{
    uint16_t timer;  
    int16_t flag;  
} tDefaultObjectLocalData;

//path platform object data
typedef struct 
{
    uint8_t currentPoint;
    tVector pathPos;    
} tPathPlatformLocalData;

//object local metadata structure
typedef struct {
    uint8_t structureType;  //stores the type of the structure (need for realloc)
    void *data;             //pointer to the local data structure
} tObjectLocalData;

enum E_OBJECT_SFX
{
    E_SFX_OBJECT_FULL_LIFE,
    E_SFX_OBJECT_EXTRA_LIVE,   
    E_SFX_WAGON,     
    E_SFX_EXPLOSION,
    E_SFX_EXPLOSION_COUNTER,
    E_SFX_OBJECT_FALL,
    E_SFX_OBJECT_ARROW,
    E_SFX_PUZZLE_NO,
    E_SFX_PUZZLE_OK,
    E_SFX_LANCE,
    E_SFX_END_OBJECT,
    E_SFX_SYMBOL,
    E_SFX_CANNON,
    
    E_SFX_OBJECT_NUM,
};

//status of egypt symbols puzzle
enum E_EGYPT_SYMBOLS_STATUS
{
    E_EGYPT_SYMBOL_STATUS_INIT      = 0,
    E_EGYPT_SYMBOL_STATUS_OK        = 1,
    E_EGYPT_SYMBOL_STATUS_NOT_OK    = -1,    
};

//types of platforms
enum E_PLATFORM_TYPES
{
    E_PLATFORM_TYPE_MOVE_PATROL       = 0x01,     //moves on dir axis platform x num of tiles and change direction on collision
    E_PLATFORM_TYPE_MOVE_DISAPPEAR    = 0x02,     //moves on dir axis platform on dir without collision until disappear
    E_PLATFORM_TYPE_WAIT_PLAYER       = 0x04,     //flag for wait the player before move
    E_PLATFORM_TYPE_TILE_RANGE        = 0x08,     //patrol range moves to fixed tile range
};

//debug trace entity object info
void object_trace(tEntity *this);

//initializes the object systen
void object_system_init();
//destroys the object system
void object_system_destroy();
//inits an entity passing his entity structure
void object_init(tEntity *entity);
//creates an entity passing his entity structure
void object_create(tEntity *entity);
//updates an entity passing his entity structure
void object_update(tEntity *entity);
//destroy object instance of list
void object_destroy(tEntity *entity);

//custom update function for objects

//object solid (pickable or not and breakable or not)
void object_solid_update(tEntity *this, tDefaultObjectLocalData *local);
//items object (extra live, full life)
void object_item_update(tEntity *this, tDefaultObjectLocalData *local);
//special trigger objects (checkpoints, end level, stop scroll...)
void object_trigger_update(tEntity *this, tDefaultObjectLocalData *local);
//platform wagon
void object_wagon_update(tEntity *this, tDefaultObjectLocalData *local);
//dynamite object
void object_dynamite_update(tEntity *this, tDefaultObjectLocalData *local);
//platform bridge
void object_bridge_update(tEntity *this, tDefaultObjectLocalData *local);
//explosion rock
void object_rock_explosion_update(tEntity *this, tDefaultObjectLocalData *local);
//object fall
void object_fall_update(tEntity *this, tDefaultObjectLocalData *local);
//platform quicksand
void object_quick_sand_update(tEntity *this, tDefaultObjectLocalData *local);
//trap arrow
void object_trap_arrow_update(tEntity *this, tDefaultObjectLocalData *local);
//egypt symbol puzzle hole
void object_symbol_hole_update(tEntity *this, tDefaultObjectLocalData *local);
//general platform object
void object_platform_update(tEntity *this, tDefaultObjectLocalData *local);
//lance object
void object_lance_update(tEntity *this, tDefaultObjectLocalData *local);
//spike trap 2
void object_spike_trap_2_update(tEntity *this, tDefaultObjectLocalData *local);
//path platform object
void object_path_platform_update(tEntity *this, tPathPlatformLocalData *local);
//cannon
void object_cannon_update(tEntity *this, tDefaultObjectLocalData *local);
//lava drop
void object_lava_drop_update(tEntity *this, tDefaultObjectLocalData *local);
#endif