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

//Funcion de colision con tile segun mapa de durezas segun su punto de colision
//Posiciona el objeto en el borde del tile y devuelve un int con el sentido de la colision o 0 si no lo hay
int16_t collision_check_tile(tEntity *idEntity,int i);
//function to init entity collision points
void collision_init_entity_points(tEntity *entity, tColPoint *entColPoint);

#endif