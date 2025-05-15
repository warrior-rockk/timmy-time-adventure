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

#include "globals.h"
#include "entity.h"

#define ENTITY_MAX_NUM_COLLISION_POINTS     256

//Puntos de colision
//los puntos laterales deben estar primero de los inferiores/superiores
//para el buen funcionamiento de la deteccion de obstaculos
enum colPoints {
    RIGHT_UP_POINT,
    RIGHT_DOWN_POINT,
    LEFT_UP_POINT,
    LEFT_DOWN_POINT,
    DOWN_L_POINT,
    DOWN_R_POINT,
    UP_L_POINT,
    UP_R_POINT,
    CENTER_POINT,
    CENTER_DOWN_POINT,
    NUM_COL_POINTS  //counter
};

//collision direction codes
enum colCodes{
    NOCOL,
    COLUP,
    COLDOWN,
    COLIZQ,
    COLDER,  
    COLCENTER, 
};

//collision point
typedef struct tColPoint
{
    tVector offset;                 //Vector de offset a sumar a la posicion del objeto
    enum colCodes colCode;			//Codigo del punto de colision
	bool enabled;			        //Habilitacion del punto de colision
} tColPoint;

//entity collision points
typedef struct tEntColPoints
{
    tColPoint colPoint[NUM_COL_POINTS];
} tEntColPoints;


//inits collision system
void collision_system_init();
//destroys collision system
void collision_system_destroy();
//function to create entity collision points for an entity
void collision_create_entity_points(tEntity *entity);



//Funcion de colision con tile segun mapa de durezas segun su punto de colision
//Posiciona el objeto en el borde del tile y devuelve un int con el sentido de la colision o 0 si no lo hay
int16_t collision_check_tile(tEntity *idEntity,int i);


#endif