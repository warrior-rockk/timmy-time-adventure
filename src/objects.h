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
#define PUZZLE1_START_DOOR_X_TILE   226
#define PUZZLE1_START_DOOR_Y_TILE   18
#define PUZZLE2_NUM_DOOR_TILES      7
#define PUZZLE2_START_DOOR_X_TILE   134
#define PUZZLE2_START_DOOR_Y_TILE   17
#define PUZZLE2_SYMBOL_NUM          3
#define PUZZLE_DOOR_TILE_ID         34
#define PUZZLE_NO_DOOR_TILE_ID      131


//object types
enum E_OBJECT_TYPE
{
    E_ROCK_OBJECT_TYPE,
    E_ROCK_2_OBJECT_TYPE,
    E_EGG_OBJECT_TYPE,
    E_END_LEVEL_OBJECT_TYPE,
    E_CHECKPOINT_OBJECT_TYPE,
    E_ITEM_OBJECT_TYPE,
    E_STOP_SCROLL_OBJECT_TYPE,
    E_ROCK_3_OBJECT_TYPE,
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
    E_ROCK_4_OBJECT_TYPE,
    E_EGYPT_PLATFORM_OBJECT_TYPE,
    E_SPIKE_TRAP_OBJECT_TYPE,
    E_LANCE_OBJECT_TYPE,
    E_SCROLLMODE_OBJECT_TYPE,
    E_DOOR_OBJECT_TYPE,

    E_OBJECTS_TYPE_NUM,
};

//item type uses spare to set type
enum E_ITEMS_TYPE
{
    E_ITEM_EXTRA_LIFE,
    E_ITEM_FULL_LIFE,
    E_ITEMS_NUM,
};

typedef struct 
{
    uint16_t timer;  
    int16_t flag;  
} tSolidObjectLocalData;

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
    E_PLATFORM_TYPE_MOVE_X_INI_LEFT,
    E_PLATFORM_TYPE_MOVE_X_INI_RIGHT,
    E_PLATFORM_TYPE_MOVE_Y,
    E_PLATFORM_TYPE_WAIT_PLAYER,
    E_PLATFORM_TYPE_MOVE_X_PATROL_INI_LEFT,
    E_PLATFORM_TYPE_MOVE_X_PATROL_INI_RIGHT,
    E_PLATFORM_TYPE_MOVE_Y_PATROL,
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
void object_solid_update(tEntity *this, tSolidObjectLocalData *objData);
//items object (extra live, full life)
void object_item_update(tEntity *this, tSolidObjectLocalData *objData);
//special trigger objects (checkpoints, end level, stop scroll...)
void object_trigger_update(tEntity *this, tSolidObjectLocalData *objData);
//platform wagon
void object_wagon_update(tEntity *this, tSolidObjectLocalData *objData);
//dynamite object
void object_dynamite_update(tEntity *this, tSolidObjectLocalData *objData);
//platform bridge
void object_bridge_update(tEntity *this, tSolidObjectLocalData *objData);
//explosion rock
void object_rock_explosion_update(tEntity *this, tSolidObjectLocalData *objData);
//object fall
void object_fall_update(tEntity *this, tSolidObjectLocalData *objData);
//game over animation
void object_game_over_update(tEntity *this, tSolidObjectLocalData *objData);
//platform quicksand
void object_quick_sand_update(tEntity *this, tSolidObjectLocalData *objData);
//trap arrow
void object_trap_arrow_update(tEntity *this, tSolidObjectLocalData *objData);
//egypt symbol puzzle hole
void object_symbol_hole_update(tEntity *this, tSolidObjectLocalData *objData);
//general platform object
void object_platform_update(tEntity *this, tSolidObjectLocalData *objData);
//lance object
void object_lance_update(tEntity *this, tSolidObjectLocalData *objData);
#endif