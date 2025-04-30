#include <stdbool.h>
#include "entity.h"

//collision direction codes
enum colCodes{
    NOCOL,
    COLUP,
    COLDOWN,
    COLIZQ,
    COLDER,  
    COLCENTER, 
};

//collision point of object
typedef struct tColPoint
{
    int16_t x;						//Offset X a sumar a la posicion del objeto
	int16_t y;						//Offset Y a sumar a la posicion del objeto
	enum colCodes colCode;			//Codigo del punto de colision
	bool enabled;			        //Habilitacion del punto de colision
} tColPoint;

//Funcion de colision con tile segun mapa de durezas segun su punto de colision
//Posiciona el objeto en el borde del tile y devuelve un int con el sentido de la colision o 0 si no lo hay
int16_t colCheckTileTerrain(tEntity idEntity,int i);
