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
    
    E_SFX_OBJECT_NUM,
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
#endif