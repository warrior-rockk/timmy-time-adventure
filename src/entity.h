/********************************************************************
* Entity system
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#ifndef _H_ENTITY_
#define _H_ENTITY_

#include <stdbool.h>

#include "allegro.h"
#include "utils.h"
#include "scroll.h"
#include "anim.h"

#define ENTITY_MAX_NUM      255     //max. number of entities
#define ENTITY_BLINK_SPEED  2       //blink entity speed

#define ENTITY_GRAVITY      0.18    //entity gravity acceleration
#define ENTITY_MIN_VEL_X    0.1     //min entity horizontal velocity
#define ENTITY_MAX_VEL_Y    4.0 //6 //max entity vertical velocity
#define ENTITY_FRICTION     0.92    //horizontal friction constant

#ifdef DEBUGMODE
    #define DEBUG_POINT_ENABLED_COLOR       53  //color for debug collision point enabled
    #define DEBUG_POINT_DISABLED_COLOR      66  //color for debug collision point disabled
#endif

//entity classes
enum E_ENTITY_CLASS
{
    E_ENT_CLASS_TRIGGER,    //trigger has priority order from player    
    E_ENT_CLASS_PLATFORM,   //platfrom has priority order from player
    E_ENT_CLASS_PLAYER,
    E_ENT_CLASS_OBJECT,
    E_ENT_CLASS_ENEMY,    
};

//entity properties
enum E_ENTITY_PROPERTIES
{
    E_ENT_PROP_PHYSICS_ON       = 0x01,     //entity not affected by gravity or friction
    E_ENT_PROP_NO_COLLISION     = 0x02,     //entity not collidable
    E_ENT_PROP_PERSISTENT       = 0x04,     //entity doesn't sleep when out of region and doesn't init when in region
    E_ENT_PROP_NO_BREAKABLE     = 0x08,     //entity not breakable
    E_ENT_PROP_NO_PICKABLE      = 0x10,     //entity not pickable by player
    E_ENT_PROP_AUTO_DESTROY     = 0x20,     //entity destroys when out region (no awake)
    E_ENT_PROP_NO_HURT          = 0x40,     //player can't hurt, but enemy entity can hurt player
    E_ENT_PROP_NO_SPAWN         = 0x80,     //entity doesn't spawn when dead (one pick items)
    E_ENT_PROP_HURTLESS         = 0x100,    //enemy entity can't hurt player and player can't hurt entity
    E_ENT_PROP_NO_FLIP          = 0x200,    //don't flip entity on draw based on direction
    E_ENT_PROP_DEAD_OUT_SCREEN  = 0x400,    //entity dead when out screen (instead of sleep)
};

//entity direction (for drawing)
enum E_ENTITY_DIR
{
    E_ENT_DIR_LEFT,
    E_ENT_DIR_RIGHT,    
    E_ENT_DIR_DOWN,
    E_ENT_DIR_UP,
};

//entity alignment axis (for drawing)
enum E_ENTITY_AXIS
{
    E_ENT_AXIS_CENTER,
    E_ENT_AXIS_UP,
    E_ENT_AXIS_DOWN,
    E_ENT_AXIS_LEFT,
    E_ENT_AXIS_RIGHT,
    E_ENT_AXIS_LEFT_DOWN,
    E_ENT_AXIS_RIGHT_DOWN,
};

//entity signals
enum E_ENTITY_SIGNAL
{
    E_ENT_SIGNAL_NONE,
    E_ENT_SIGNAL_HURT,
    E_ENT_SIGNAL_KILL,
    E_ENT_SIGNAL_PICKING,
    E_ENT_SIGNAL_THROW,
    E_ENT_SIGNAL_SHORT_THROW,
    E_ENT_SIGNAL_AWAKE,

    E_ENT_SIGNAL_NUM,
};

//entity object
typedef struct tEntity
{
    uint16_t id;                //entity id number
    tVector pos;                //entity position
    tVector size;               //entity size (for position and collision. It's better to be multiple to tileSize to easy snap on Tiled)
    tVector spriteSize;         //entity sprite size (size of one sprite frame)
    tFixVector fixPos;          //entity fixed position
    tFixVector fixVel;          //entity fixed velocities
    tVector initPos;            //entity initial position
    uint8_t axis;               //entity drawing axis
    BITMAP *img;                //entity spriteSheet
    tAnimation anim;            //entity animation structure
    uint8_t state;              //entity state
    uint8_t prevState;          //previous entity state
    uint8_t entType;            //entity type
    uint8_t entClass;           //entity class
    uint8_t entInstance;        //entity instance of type
    uint16_t properties;        //entity properties
    uint8_t initDir;            //entity initial direction
    uint8_t dir;                //entity direction
    uint8_t signal;             //entity signal to comunicate between entities
    bool dead;                  //dead flag: no entity update and no entity draw
    //bool freeze;              //freeze flag: no entity update
    bool sleep;                 //sleep flag: no entity update and no entity draw
    bool visible;               //visible flag: no entity draw
    bool ground;                //ground flag    
    int16_t spare;              //spare data for general porpouse
    //function pointers
    void (*entity_init)(struct tEntity *entity);        //function pointer to entity init function
    void (*entity_create)(struct tEntity *entity);      //function pointer to entity create function
    void (*entity_update)(struct tEntity *entity);      //function pointer to entity update function
    void (*entity_destroy)(struct tEntity *entity);     //function pointer to entity destroy function
} tEntity;

//inits entity system
void entity_system_init();
//destroys entity system
void entity_system_destroy();

//calls the init function on all entitites
void entities_init();
//calls the update function on all entities
void entities_update();
//draws all entities on passed buffer
void entities_draw(BITMAP *buffer);
//destroy all entities
void entities_destroy_all();

//creates a new entity. Returns entity number
int16_t entity_create(uint8_t entityClass, uint8_t entityType, tVector initPos, enum E_ENTITY_DIR initDir, int16_t spare);
//function to update actual vel and position of an entity
void entity_update_vel_pos(tEntity *entity);

//returns number of entities
uint8_t entities_get_num();
//returns entity based on entity number
tEntity* entity_get(uint16_t numEntity);
//function to return a entity by his entity class and instance. NULL if not found
tEntity* entity_get_by_instance(uint8_t entityClass, uint16_t entityInstance);
//blink entity
void entity_blink(tEntity *entity);
//returns the center position x of an entity
int16_t entity_center_x(tEntity *entity);
//returns the center position y of an entity
int16_t entity_center_y(tEntity *entity);
//sets the id of the player on entities array
void entity_set_player_id(uint8_t playerId);
//gets the id of the player on entities array
uint8_t entity_get_player_id();

//traces entity information
void entity_trace(tEntity *entity);
#endif
