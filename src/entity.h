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

#define ENTITY_MAX_NUM      255     //Max. number of entities

#define DEBUG_POINT_ENABLED_COLOR       53
#define DEBUG_POINT_DISABLED_COLOR      66
#define DEBUG_SHOW_ALL_LAYER            1
#define DEBUG_SHOW_COLL_LAYER           2

//game entity classes
enum E_ENTITY_CLASS
{
    E_ENT_CLASS_PLAYER,
    E_ENT_CLASS_OBJECT,
    E_ENT_CLASS_ENEMY,
    E_ENT_CLASS_PLATFORM,
};

enum E_ENTITY_PROPERTIES
{
    E_ENT_PROP_PHYSICS_ON    = 0x01,
    E_ENT_PROP_NO_COLLISION  = 0x02,
//    E_PERSISTENT                = 0x04,
};

//entities direction (for drawing)
enum E_ENTITY_DIR
{
    E_ENT_DIR_RIGHT,
    E_ENT_DIR_LEFT,
};

//entities alignment size axis (for drawing)
enum E_ENTITY_AXIS
{
    E_ENT_AXIS_CENTER,
    E_ENT_AXIS_UP,
    E_ENT_AXIS_DOWN,
};

//entity object
typedef struct tEntity
{
    uint16_t id;                //entity id number
    tVector pos;                //entity position
    tVector size;               //entity size
    tVector spriteSize;         //entity sprite size
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
    enum E_ENTITY_DIR initDir;  //entity initial direction
    enum E_ENTITY_DIR dir;      //entity direction
    bool dead;                  //dead flag
    bool visible;               //visible flag
    bool ground;                //ground flag
    //function pointers
    void (*entity_init)(struct tEntity *entity);
    void (*entity_create)(struct tEntity *entity);
    void (*entity_update)(struct tEntity *entity);
} tEntity;

//inits entity system
void entity_system_init();
//destroys entity system
void entity_system_destroy();

//creates a new entity. Returns entity number
int16_t entity_create(uint8_t entityClass, uint8_t entityType, tVector initPos, enum E_ENTITY_DIR initDir);
//calls the init function on all entitites
void entities_init();
//calls the update function on all entities
void entities_update(tScroll *scroll);
//draws all entities on passed buffer
void entities_draw(BITMAP *buffer, tScroll *scroll);
//returns entity based on entity number
tEntity* entity_get(uint16_t numEntity);
//returns number of entities
uint8_t entities_get_num();
//destroy all entities
void entity_destroy_all();

#endif
