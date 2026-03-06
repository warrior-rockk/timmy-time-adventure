/********************************************************************
* Collision System
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#ifndef _H_COLLISIONS_
#define _H_COLLISIONS_

#include <stdbool.h>
#include "allegro.h"

#include "utils.h"
#include "entity.h"

#define USE_SLOPE_COLLISION                 1
#define SLOPE_MAX_HEIGHT                    8   //max height to consider slope and adjust position to it

#define ENTITY_MAX_NUM_COLLISION_POINTS     256


//collision points (horizontal point must be first than uppers/lowers for good detection)
enum E_COL_POINTS {
    E_COLPOINT_RIGHT_UP,
    E_COLPOINT_RIGHT_DOWN,
    E_COLPOINT_LEFT_UP,
    E_COLPOINT_LEFT_DOWN,
    E_COLPOINT_DOWN_L,
    E_COLPOINT_DOWN_R,
    E_COLPOINT_UP_L,
    E_COLPOINT_UP_R,
    E_COLPOINT_CENTER,
    E_COLPOINT_CENTER_DOWN,
    E_NUM_COL_POINTS  //counter
};

//collision direction codes
enum E_COL_DIR_CODES{
    E_COLLISION_DIR_NONE,
    E_COLLISION_DIR_UP,
    E_COLLISION_DIR_DOWN,
    E_COLLISION_DIR_LEFT,
    E_COLLISION_DIR_RIGHT,  
    E_COLLISION_DIR_CENTER, 
};

//collision bounce modes
enum E_COLLISION_BOUNCE_MODE
{
    E_COLLISION_NO_BOUNCE,
    E_COLLISION_BOUNCE_SOFT,
    E_COLLISION_BOUNCE_HARD,
};

//checkVectorY modes
enum E_CHECKVECTORMODES{
    E_CHECK_VECTOR_Y_FROM_COLLISION,        //check pixels to exit collision
    E_CHECK_VECTOR_Y_TO_COLLISION,          //check pixel to reach collision
};

//checkProcess modes
enum E_CHECK_PROCESS_MODE{
    E_CHECK_PROCESS_BOTHAXIS,			
    E_CHECK_PROCESS_HORIZONTALAXIS,		
    E_CHECK_PROCESS_VERTICALAXIS,		
    E_CHECK_PROCESS_INFOONLY,			
};

//collision point
typedef struct
{
    tVector offset;                 //offsect vector to add entity position
    enum E_COL_DIR_CODES colCode;	//collision point dir code
	bool enabled;			        //enable/disable collision point
} tColPoint;

//entity collision points
typedef struct
{
    uint16_t entId;                         //id of entity
    tColPoint colPoint[E_NUM_COL_POINTS];   //array of collision points
} tEntColPoints;

//inits collision system
void collision_system_init();
//destroys collision system
void collision_system_destroy();

//function to create entity collision points for an entity
void collision_create_entity_points(tEntity *entity);
//function to destroy entity collision points by id
void collision_destroy_entity_points(uint16_t entityId);
//function to set the position of collision points
void collision_set_collision_points(tEntity *entity, uint8_t colPointIndex);
//function to get a collision point
tColPoint* collision_get_ent_collision_point(tEntity *entity, uint8_t numPoint);
//gets collision point list index by entity id (-1 if not found)
int16_t collision_get_point_index_by_entId(uint16_t entityId);
//function to disable all collision points except one
void collision_disable_points_except(uint16_t entityId, uint8_t numPoint);
//checks if an entity id has collision points created
bool collision_check_entity_col_points(uint16_t entityId);

//function to check entity collision with tilemap. Returns: direction of collision
uint8_t collision_check_tile(tEntity *idEntity, uint16_t pointNum);
//function to check collision between entities
uint8_t collision_check_entity(tEntity *entityA, tEntity *entityB, enum E_CHECK_PROCESS_MODE mode);
//check the tile property to determine the collision behaviour based on colDir (return 1 if collided or 0 no collided)
bool collision_check_by_direction(tEntity *entity, uint8_t colDir, uint8_t tileProperty);
//return the property of tile on collision point number
uint8_t collision_get_tile(tEntity *entity, uint16_t pointNum);

//function to apply the direction of the collision to an entity
void collision_apply_dir(tEntity *entity, int16_t colDir, uint8_t bounceMode);

#endif