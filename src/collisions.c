/********************************************************************
* Collisions System
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/

#include <stdint.h>
#include "collisions.h"

static tEntColPoints *entColPointsList;     //dynamic list of entities collision points
static uint16_t numEntitiesColPoints;       //number of entities collision points

//inits collision system
void collision_system_init()
{
    collision_system_destroy();
}

//destroys collision system
void collision_system_destroy()
{
    //free collision memory allocation
    free(entColPointsList);
    //clear num entities collision points
    numEntitiesColPoints = 0;
}

int16_t collision_check_tile(tEntity *idEntity,int i)
{ 

    /*
    tVector colVector;	//Vector de comprobacion colision
    int distColX;		//Distancia con la colision en X
    int distColY;		//Distancia con la colision en Y
    int colDir;			//Sentido de la colision

    
    colDir = 0;
            
    //comprobamos si el punto de control esta activo
    if (!identity->this.colPoint[i].enabled) return colDir; end;
    
    //===============
    //COLISIONES EN X
    //===============
    
    //desactivamos puntos de control inferiores si estamos en rampa
    if (cSlopesEnabled)
        identity->this.colPoint[LEFT_DOWN_POINT].enabled  = getTileCode(idEntity,CENTER_DOWN_POINT) <> SLOPE_135;
        identity->this.colPoint[RIGHT_DOWN_POINT].enabled = getTileCode(idEntity,CENTER_DOWN_POINT) <> SLOPE_45;
    end;
    
    //si el punto de deteccion es lateral (X)
    if (identity->this.colPoint[i].colCode == COLDER || identity->this.colPoint[i].colCode == COLIZQ )
        
        //Establecemos el vector a chequear
        colVector.vStart.x = identity->this.fX+identity->this.colPoint[i].x;
        colVector.vEnd.x   = colVector.vStart.x+identity->this.vX;
        colVector.vStart.y = identity->this.fY+identity->this.colPoint[i].y;
        colVector.vEnd.y   = colVector.vStart.y;
            
        //lanzamos la comprobacion de colision en X
        distColX = colCheckVectorX(idEntity,&colVector,identity->this.colPoint[i].colCode);
        
        //Si hay colision
        If (distColX>=0)
            //Colision Derecha
            if (identity->this.colPoint[i].colCode == COLDER) 
                //situamos el objeto al borde de la colision	
                identity->this.fX+= distColX-1;
                colDir = COLDER;
                
            end;
            //Colision Izquierda
            if (identity->this.colPoint[i].colCode == COLIZQ) 			
                //situamos el objeto al borde de la colision
                identity->this.fX-= distColX-1;
                colDir = COLIZQ;
            end;
        end;  
    end;
    
    //===============
    //COLISIONES EN Y
    //===============
    
    //Si el punto de deteccion es uno de los superiores/inferiores
    if (identity->this.colPoint[i].colCode == COLUP || identity->this.colPoint[i].colCode == COLDOWN)
        
        //Establecemos el vector a comparar
        colVector.vStart.x = identity->this.fX+identity->this.colPoint[i].x;
        colVector.vEnd.x   = colVector.vStart.x;
        colVector.vStart.y = identity->this.fY+identity->this.colPoint[i].y;
        colVector.vEnd.y   = colVector.vStart.y+identity->this.vY;
        
        //Lanzamos la comprobacion de colision en Y
        distColY = colCheckVectorY(idEntity,&colVector,identity->this.colPoint[i].colCode,TOCOLLISION);
        
        //Si hay colision
        If (distColY>=0) 
            //Colision inferior
            if (identity->this.colPoint[i].colCode == COLDOWN && identity->this.vY>=0)
                //Situamos al objeto en el borde de la colision
                identity->this.fY += distColY;
                colDir = COLDOWN;
                
                //Deteccion de pendiente,comprobamos si estamos enterrados
                if (cSlopesEnabled)
                                            
                    //Establecemos el vector a comparar (centro/inferior del objeto)
                    colVector.vStart.x = identity->this.fX+identity->this.colPoint[CENTER_DOWN_POINT].x;
                    colVector.vEnd.x   = colVector.vStart.x;
                    colVector.vStart.y = identity->this.fY+identity->this.colPoint[CENTER_DOWN_POINT].y;
                    colVector.vEnd.y   = colVector.vStart.y-cHillHeight; //altura maxima para considerar pendiente
                    
                    //Lanzamos la comprobacion de colision en Y
                    distColY = colCheckVectorY(idEntity,&colVector,COLCENTER,FROMCOLLISION);
                    
                    //Subimos al objeto a la pendiente
                    if (distColY >0)
                        identity->this.fY -= distColY-1;
                    end;
                end;
            End;                                 
            
            //Colision superior
            if (identity->this.colPoint[i].colCode == COLUP && identity->this.vY<0)
                //Situamos al objeto en el borde de la colision
                identity->this.fY -= distColY;
                colDir = COLUP;
            End;
        
        else 
            //si no hay colision, comprobamos si pendiente hacia abajo
            if (cSlopesEnabled)
                //lo comprobamos si no estamos en escalera para despegarnos del suelo
                if (identity->this.vY > 0)
                    //Establecemos el vector a comparar (centro/inferior del objeto)
                    colVector.vStart.x = identity->this.fX+identity->this.colPoint[CENTER_DOWN_POINT].x;
                    colVector.vEnd.x   = colVector.vStart.x;
                    colVector.vStart.y = identity->this.fY+identity->this.colPoint[CENTER_DOWN_POINT].y-1;
                    colVector.vEnd.y   = colVector.vStart.y+identity->this.vY+cHillHeight; //altura maxima para considerar pendiente
                    
                    //Lanzamos la comprobacion de colision en Y
                    distColY = colCheckVectorY(idEntity,&colVector,COLCENTER,TOCOLLISION);
                    
                    //Bajamos al objeto a la pendiente
                    if (distColY >0)
                        identity->this.fY += distColY;
                    end;	
                end;
            end;
        end; 
        
    end;
    
    //Devolvemos el sentido de la colision
    return colDir;
    */
   return 1;
}

void collision_create_entity_points(tEntity *entity)
{	
    //inc number on entities with collision points
    numEntitiesColPoints++;
    
    if (numEntitiesColPoints <= ENTITY_MAX_NUM_COLLISION_POINTS)
    {
        //allocate memory for entity collision points
        entColPointsList = realloc(entColPointsList, numEntitiesColPoints * sizeof(tEntColPoints));
        
        //test memory allocation
        ASSERT(entColPointsList);

        //initialize new entity collision point data
        uint16_t newEntityColPoints = numEntitiesColPoints - 1;
        
        entColPointsList[newEntityColPoints].entId = entity->id;
        entColPointsList[newEntityColPoints].colPoint[RIGHT_UP_POINT].offset.x 			= (entity->size.x >> 1) - 1;
        entColPointsList[newEntityColPoints].colPoint[RIGHT_UP_POINT].offset.y 			= -(entity->size.y / 4);
        entColPointsList[newEntityColPoints].colPoint[RIGHT_UP_POINT].colCode 	        = COLDER;
        entColPointsList[newEntityColPoints].colPoint[RIGHT_UP_POINT].enabled 	        = true;
        
        entColPointsList[newEntityColPoints].colPoint[RIGHT_DOWN_POINT].offset.x 		= (entity->size.x >> 1) - 1;
        entColPointsList[newEntityColPoints].colPoint[RIGHT_DOWN_POINT].offset.y 	    = (entity->size.y / 4);
        entColPointsList[newEntityColPoints].colPoint[RIGHT_DOWN_POINT].colCode         = COLDER;
        entColPointsList[newEntityColPoints].colPoint[RIGHT_DOWN_POINT].enabled         = true;
        
        entColPointsList[newEntityColPoints].colPoint[LEFT_UP_POINT].offset.x 		    = -(entity->size.x >> 1);
        entColPointsList[newEntityColPoints].colPoint[LEFT_UP_POINT].offset.y 		    = -(entity->size.y / 4);
        entColPointsList[newEntityColPoints].colPoint[LEFT_UP_POINT].colCode            = COLIZQ;
        entColPointsList[newEntityColPoints].colPoint[LEFT_UP_POINT].enabled            = true;
        
        entColPointsList[newEntityColPoints].colPoint[LEFT_DOWN_POINT].offset.x 		= -(entity->size.x >> 1);
        entColPointsList[newEntityColPoints].colPoint[LEFT_DOWN_POINT].offset.y 		= (entity->size.y / 4);
        entColPointsList[newEntityColPoints].colPoint[LEFT_DOWN_POINT].colCode          = COLIZQ;
        entColPointsList[newEntityColPoints].colPoint[LEFT_DOWN_POINT].enabled          = true;
        
        entColPointsList[newEntityColPoints].colPoint[DOWN_R_POINT].offset.x 		    = (entity->size.x / 4);
        entColPointsList[newEntityColPoints].colPoint[DOWN_R_POINT].offset.y 		    = (entity->size.y >> 1);
        entColPointsList[newEntityColPoints].colPoint[DOWN_R_POINT].colCode             = COLDOWN;
        entColPointsList[newEntityColPoints].colPoint[DOWN_R_POINT].enabled             = true;
        
        entColPointsList[newEntityColPoints].colPoint[DOWN_L_POINT].offset.x 		    = -(entity->size.x / 4);
        entColPointsList[newEntityColPoints].colPoint[DOWN_L_POINT].offset.y 		    = (entity->size.y >> 1);
        entColPointsList[newEntityColPoints].colPoint[DOWN_L_POINT].colCode             = COLDOWN;
        entColPointsList[newEntityColPoints].colPoint[DOWN_L_POINT].enabled             = true;
        
        entColPointsList[newEntityColPoints].colPoint[UP_R_POINT].offset.x 		        = (entity->size.x / 4);
        entColPointsList[newEntityColPoints].colPoint[UP_R_POINT].offset.y 		        = -(entity->size.y >> 1);
        entColPointsList[newEntityColPoints].colPoint[UP_R_POINT].colCode               = COLUP;
        entColPointsList[newEntityColPoints].colPoint[UP_R_POINT].enabled               = true;
        
        entColPointsList[newEntityColPoints].colPoint[UP_L_POINT].offset.x 		        = -(entity->size.x / 4);
        entColPointsList[newEntityColPoints].colPoint[UP_L_POINT].offset.y 		        = -(entity->size.y >> 1);
        entColPointsList[newEntityColPoints].colPoint[UP_L_POINT].colCode               = COLUP;
        entColPointsList[newEntityColPoints].colPoint[UP_L_POINT].enabled               = true;
        
        entColPointsList[newEntityColPoints].colPoint[CENTER_POINT].offset.x 		    = 0;
        entColPointsList[newEntityColPoints].colPoint[CENTER_POINT].offset.y 		    = 0;
        entColPointsList[newEntityColPoints].colPoint[CENTER_POINT].colCode             = COLCENTER;
        entColPointsList[newEntityColPoints].colPoint[CENTER_POINT].enabled             = false;
        
        entColPointsList[newEntityColPoints].colPoint[CENTER_DOWN_POINT].offset.x 		= 0;
        entColPointsList[newEntityColPoints].colPoint[CENTER_DOWN_POINT].offset.y 		= (entity->size.y >> 1);
        entColPointsList[newEntityColPoints].colPoint[CENTER_DOWN_POINT].colCode        = COLCENTER;
        entColPointsList[newEntityColPoints].colPoint[CENTER_DOWN_POINT].enabled        = false;

        MY_TRACE("[COLLISION SYSTEM]: Created entity collision points on position: %d\n", newEntityColPoints);
        MY_TRACE("[COLLISION SYSTEM]: Total of entity collision points: %d\n", numEntitiesColPoints);
    }
    else
        abort_on_error("ERROR: Reached max. number of entities collision points\n");        
}

void collision_destroy_entity_points(uint16_t entityId)
{
    uint16_t listPosition;

    //find entity id on collision points list
    for (int i = 0; i < numEntitiesColPoints; i++)
    {
        if (entColPointsList[i].entId == entityId)
        {
            listPosition = i;
            break;
        }
    }

    //copies last entity col points to deleted entity position
    entColPointsList[listPosition] = entColPointsList[numEntitiesColPoints - 1];
    //decrement entity col points number
    numEntitiesColPoints--;
    if (numEntitiesColPoints == 0)
        //free entity list
        free(entColPointsList);
    else
        //reallocates the array with decremented entity number    
        entColPointsList = realloc(entColPointsList, numEntitiesColPoints * sizeof(tEntColPoints));    

    MY_TRACE("[COLLISION SYSTEM]: Deleted entity collision points on position: %d\n", listPosition);
    MY_TRACE("[COLLISION SYSTEM]: Total of entity collision points: %d\n", numEntitiesColPoints);
}