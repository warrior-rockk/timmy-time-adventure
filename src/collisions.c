/********************************************************************
* Collisions System
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/

#include <stdint.h>
#include "collisions.h"
#include "map.h"

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

//gets collision point list index by entity id
uint16_t get_collision_point_index_by_entId(uint16_t entityId)
{
    //find entity id on collision points list
    for (int i = 0; i < numEntitiesColPoints; i++)
    {
        if (entColPointsList[i].entId == entityId)
            return i;
    }
}

//Funcion que devuelve,dado un vector, el numero de pixeles en x hasta la colision, o -1 si no hay
//dado una entidad, un vector de comprobacion y el punto de colision a chequear
int16_t colCheckVectorX(tEntity *entity, tLinePath *linePath, uint16_t colCode)
{
    int16_t dist = 0;   //distance to collision
    int16_t inc;        //increment

	//sets increment direction
    inc = linePath->end.x >= linePath->start.x ? 1 : -1;
	
    //run path line searching pixel collision
    do
    {		
		//if tile exists on path point position
        if (map_tile_exists(&linePath->start))
        {
			//check if tile is solid
            if (map_get_tile_code(&linePath->start) != 0 ) //TODO: == E_SOLID_TILE_CODE)
            {
				//TODO: comprobar el codigo del tile para contarlo como colision o no
				//if (checkTileCode(idEntity,colCode,linePath.vStart.y/cTileSize,linePath.vStart.x/cTileSize))
				//{
                    //if(map_get_pixel(0,mapBox,(linePath.vStart.x%cTileSize),(linePath.vStart.y%cTileSize)) <> 0)
						return dist;
                //}
            }
        }
        //TODO: map limits collision
        /*
        else
        {
			
            //si no existe, se considera solido si es limite del mapeado
			if (linePath->start.x <= 0 || linePath->start.x >= level.numTilesX*cTileSize)
				return dist;
			end;
		}*/
				
		//add collision distance
		dist++;
		//increments vector
		linePath->start.x += inc;
	
    }
    //until reach the line path end
	while (linePath->start.x != (linePath->end.x + inc));
    
	//no collision
	return -1; 	
}

////Funcion que devuelve,dado una entidad,un vector y un punto de colision a comprobar, 
//el numero de pixeles en y hasta la colision, o -1 si no hay
//El byte "mode", determina si la comprobacion es el numero de pixeles hasta llegar a la colision (TOCOLLISION 1)
//o numero de pixeles para salir de la colision (FROMCOLLISION 0)
int16_t colCheckVectorY(tEntity *entity, tLinePath *linePath, uint16_t colCode, enum eCheckVectorYModes mode)
{

    int16_t dist = 0;		//distance to collision
    int16_t inc;			//increment
    uint8_t colPixel = 0;	//color pixel of collision
	
	//sets increment direction
    inc = linePath->end.y >= linePath->start.y ? 1 : -1;

	//number of pixel of entity above of upper collision
    linePath->start.y += 1;
    
	//run vector searching pixel collision
	do
	{		
        //check if tile exists on path position
        if (map_tile_exists(&linePath->start))
        {            
            //check if tile is solid
            if (map_get_tile_code(&linePath->start) != 0) //TODO: == E_SOLID_TILE_CODE)
            {
                colPixel = 100;                
            }
            else
            {                
                //TODO: comprobar el codigo del tile para contarlo como colision o no
                /*if (checkTileCode(idEntity,colCode,colVector.vStart.y/cTileSize,colVector.vStart.x/cTileSize))
                {
                    //Obtenemos el pixel de colision segun el tipo de tile
                    switch (tileMap[colVector.vStart.y/cTileSize][colVector.vStart.x/cTileSize].tileCode)
                        case SLOPE_135:
                            colPixel = map_get_pixel(0,mapTriangle135,(colVector.vStart.x%cTileSize),(colVector.vStart.y%cTileSize));
                        end;
                        case SLOPE_45:
                            colPixel = map_get_pixel(0,mapTriangle45,(colVector.vStart.x%cTileSize),(colVector.vStart.y%cTileSize));
                        end;
                        case TOP_STAIRS,SOLID_ON_FALL:
                            colPixel = map_get_pixel(0,mapSolidOnFall,(colVector.vStart.x%cTileSize),(colVector.vStart.y%cTileSize));
                        end;
                        default:
                            colPixel = map_get_pixel(0,mapBox,(colVector.vStart.x%cTileSize),(colVector.vStart.y%cTileSize));
                        end;
                    end;
                }*/
               colPixel = 0; 
            }	
        }
        
        //check mode
        if (mode == E_CHECK_VECTOR_Y_TO_COLLISION)	
        {
            //if detects collision, return distance
            if (colPixel != 0 )
                return dist;            
        }
        else
            //Mode FROM_COLLISION: if out of collision, return distance
            if (colPixel == 0 )
                return dist;            
                
        //increments distance
        dist++;
        //increments vector
        linePath->start.y += inc;	
    }
	//hasta recorrer todo el vector
	while ((linePath->start.y < linePath->end.y && inc==1) || (linePath->start.y > linePath->end.y && inc == -1));
	
	//no collision
	return -1;
}

int16_t collision_check_tile(tEntity *entity, uint16_t pointNum)
{ 
    tLinePath colLinePath;	//Collision path line to check
    int16_t distColX;		//X collision distance
    int16_t distColY;		//Y collision distance
    int16_t colDir;			//Direction of collision

    colDir = 0;
    
    //gets collision point index
    uint16_t entIndex = get_collision_point_index_by_entId(entity->id);

    //check if collision point is active
    if (!entColPointsList[entIndex].colPoint[pointNum].enabled) 
        return colDir;
    
    //=====================
    //HORIZONTAL COLLISIONS
    //=====================
    
    // TODO: Slopes
    //desactivamos puntos de control inferiores si estamos en rampa
    /*
    if (cSlopesEnabled)
        entity->this.colPoint[LEFT_DOWN_POINT].enabled  = getTileCode(entity,CENTER_DOWN_POINT) <> SLOPE_135;
        entity->this.colPoint[RIGHT_DOWN_POINT].enabled = getTileCode(entity,CENTER_DOWN_POINT) <> SLOPE_45;
    end;*/
    

    //check if collision point is horizontal
    if (entColPointsList[entIndex].colPoint[pointNum].colCode == E_RIGHT_COLLISION || entColPointsList[entIndex].colPoint[pointNum].colCode == E_LEFT_COLLISION)
    {    
        //Set the path collision line to check
        colLinePath.start.x = entity->pos.x + entColPointsList[entIndex].colPoint[pointNum].offset.x;
        colLinePath.end.x   = colLinePath.start.x + fixtoi(entity->fixVel.x);
        colLinePath.start.y = entity->pos.y + entColPointsList[entIndex].colPoint[pointNum].offset.y;
        colLinePath.end.y   = colLinePath.start.y + fixtoi(entity->fixVel.y);
        
        //calls the collision check line path
        distColX = colCheckVectorX(entity,&colLinePath, entColPointsList[entIndex].colPoint[pointNum].colCode);
        
        //if collision
        if (distColX >= 0)
        {
            //Right collision
            if (entColPointsList[entIndex].colPoint[pointNum].colCode == E_RIGHT_COLLISION) 
            {
                //position entity to edge of collision
                entity->fixPos.x += distColX - 1;           	
                colDir = E_RIGHT_COLLISION;
                
            }
            //Left collision
            if (entColPointsList[entIndex].colPoint[pointNum].colCode == E_LEFT_COLLISION) 			
            {
                //position entity to edge of collision
                entity->fixPos.x -= distColX - 1;           	
                colDir = E_LEFT_COLLISION;
            }
        }  
    }
    
    //===============
    //VERTICAL COLLISIONS
    //===============
    
    //check if collision point is vertical
    if (entColPointsList[entIndex].colPoint[pointNum].colCode == E_UP_COLLISION || entColPointsList[entIndex].colPoint[pointNum].colCode == E_DOWN_COLLISION)
    {           
        //set the compare vector
        colLinePath.start.x = entity->pos.x + entColPointsList[entIndex].colPoint[pointNum].offset.x;
        colLinePath.end.x   = colLinePath.start.x;
        colLinePath.start.y = entity->pos.y + entColPointsList[entIndex].colPoint[pointNum].offset.y;
        colLinePath.end.y   = colLinePath.start.y + fixtoi(entity->fixVel.y);
        
        //TRACE("sx: %i ex: %i sy: %i ey: %i \n", colLinePath.start.x, colLinePath.end.x, colLinePath.start.y, colLinePath.end.y);

        //call check vector collision on Y
        distColY = colCheckVectorY(entity, &colLinePath, entColPointsList[entIndex].colPoint[pointNum].colCode, E_CHECK_VECTOR_Y_TO_COLLISION);
                        
        //check if has collided
        if (distColY >= 0) 
        {               
            //down collision
            if (entColPointsList[entIndex].colPoint[pointNum].colCode == E_DOWN_COLLISION && fixtoi(entity->fixVel.y) >=0)
            { 
                //adjust the entity to border of collision
                entity->fixPos.x += itofix(distColY);                
                colDir = E_DOWN_COLLISION;
                
                //TODO: slopes
                /*
                //Deteccion de pendiente,comprobamos si estamos enterrados
                if (cSlopesEnabled)
                                            
                    //Establecemos el vector a comparar (centro/inferior del objeto)
                    colLinePath.vStart.x = entity->this.fX+entity->this.colPoint[CENTER_DOWN_POINT].x;
                    colLinePath.vEnd.x   = colLinePath.vStart.x;
                    colLinePath.vStart.y = entity->this.fY+entity->this.colPoint[CENTER_DOWN_POINT].y;
                    colLinePath.vEnd.y   = colLinePath.vStart.y-cHillHeight; //altura maxima para considerar pendiente
                    
                    //Lanzamos la comprobacion de colision en Y
                    distColY = colCheckVectorY(entity,&colLinePath,E_CENTER_COLLISION,FROMCOLLISION);
                    
                    //Subimos al objeto a la pendiente
                    if (distColY >0)
                        entity->this.fY -= distColY-1;
                    end;
                end;*/
            }                                 
            
            //up collision
            if (entColPointsList[entIndex].colPoint[pointNum].colCode == E_UP_COLLISION && fixtoi(entity->fixVel.y) < 0)
            {
                //adjust the entity to border of collision
                entity->fixPos.y -= itofix(distColY);                
                colDir = E_UP_COLLISION;
            }
        }
        //TODO: Slopes
        /*
        else 
        {
            
            //si no hay colision, comprobamos si pendiente hacia abajo
            if (cSlopesEnabled)
                //lo comprobamos si no estamos en escalera para despegarnos del suelo
                if (entity->this.vY > 0)
                    //Establecemos el vector a comparar (centro/inferior del objeto)
                    colVector.vStart.x = entity->this.fX+entity->this.colPoint[CENTER_DOWN_POINT].x;
                    colLinePath.vEnd.x   = colLinePath.vStart.x;
                    colLinePath.vStart.y = entity->this.fY+entity->this.colPoint[CENTER_DOWN_POINT].y-1;
                    colLinePath.vEnd.y   = colLinePath.vStart.y+entity->this.vY+cHillHeight; //altura maxima para considerar pendiente
                    
                    //Lanzamos la comprobacion de colision en Y
                    distColY = colCheckVectorY(entity,&colLinePath,E_CENTER_COLLISION,TOCOLLISION);
                    
                    //Bajamos al objeto a la pendiente
                    if (distColY >0)
                        entity->this.fY += distColY;
                    end;	
                end;
            end;
        }*/         
    }
    
    //returns collision direction
    return colDir;   
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
        entColPointsList[newEntityColPoints].colPoint[RIGHT_UP_POINT].colCode 	        = E_RIGHT_COLLISION;
        entColPointsList[newEntityColPoints].colPoint[RIGHT_UP_POINT].enabled 	        = true;
        
        entColPointsList[newEntityColPoints].colPoint[RIGHT_DOWN_POINT].offset.x 		= (entity->size.x >> 1) - 1;
        entColPointsList[newEntityColPoints].colPoint[RIGHT_DOWN_POINT].offset.y 	    = (entity->size.y / 4);
        entColPointsList[newEntityColPoints].colPoint[RIGHT_DOWN_POINT].colCode         = E_RIGHT_COLLISION;
        entColPointsList[newEntityColPoints].colPoint[RIGHT_DOWN_POINT].enabled         = true;
        
        entColPointsList[newEntityColPoints].colPoint[LEFT_UP_POINT].offset.x 		    = -(entity->size.x >> 1);
        entColPointsList[newEntityColPoints].colPoint[LEFT_UP_POINT].offset.y 		    = -(entity->size.y / 4);
        entColPointsList[newEntityColPoints].colPoint[LEFT_UP_POINT].colCode            = E_LEFT_COLLISION;
        entColPointsList[newEntityColPoints].colPoint[LEFT_UP_POINT].enabled            = true;
        
        entColPointsList[newEntityColPoints].colPoint[LEFT_DOWN_POINT].offset.x 		= -(entity->size.x >> 1);
        entColPointsList[newEntityColPoints].colPoint[LEFT_DOWN_POINT].offset.y 		= (entity->size.y / 4);
        entColPointsList[newEntityColPoints].colPoint[LEFT_DOWN_POINT].colCode          = E_LEFT_COLLISION;
        entColPointsList[newEntityColPoints].colPoint[LEFT_DOWN_POINT].enabled          = true;
        
        entColPointsList[newEntityColPoints].colPoint[DOWN_R_POINT].offset.x 		    = entity->size.x - (entity->size.x / 4); //(entity->size.x / 4);
        entColPointsList[newEntityColPoints].colPoint[DOWN_R_POINT].offset.y 		    = entity->size.y;
        entColPointsList[newEntityColPoints].colPoint[DOWN_R_POINT].colCode             = E_DOWN_COLLISION;
        entColPointsList[newEntityColPoints].colPoint[DOWN_R_POINT].enabled             = true;
        
        entColPointsList[newEntityColPoints].colPoint[DOWN_L_POINT].offset.x 		    = -(entity->size.x / 4);
        entColPointsList[newEntityColPoints].colPoint[DOWN_L_POINT].offset.y 		    = (entity->size.y >> 1);
        entColPointsList[newEntityColPoints].colPoint[DOWN_L_POINT].colCode             = E_DOWN_COLLISION;
        entColPointsList[newEntityColPoints].colPoint[DOWN_L_POINT].enabled             = true;
        
        entColPointsList[newEntityColPoints].colPoint[UP_R_POINT].offset.x 		        = (entity->size.x / 4);
        entColPointsList[newEntityColPoints].colPoint[UP_R_POINT].offset.y 		        = -(entity->size.y >> 1);
        entColPointsList[newEntityColPoints].colPoint[UP_R_POINT].colCode               = E_UP_COLLISION;
        entColPointsList[newEntityColPoints].colPoint[UP_R_POINT].enabled               = true;
        
        entColPointsList[newEntityColPoints].colPoint[UP_L_POINT].offset.x 		        = -(entity->size.x / 4);
        entColPointsList[newEntityColPoints].colPoint[UP_L_POINT].offset.y 		        = -(entity->size.y >> 1);
        entColPointsList[newEntityColPoints].colPoint[UP_L_POINT].colCode               = E_UP_COLLISION;
        entColPointsList[newEntityColPoints].colPoint[UP_L_POINT].enabled               = true;
        
        entColPointsList[newEntityColPoints].colPoint[CENTER_POINT].offset.x 		    = 0;
        entColPointsList[newEntityColPoints].colPoint[CENTER_POINT].offset.y 		    = 0;
        entColPointsList[newEntityColPoints].colPoint[CENTER_POINT].colCode             = E_CENTER_COLLISION;
        entColPointsList[newEntityColPoints].colPoint[CENTER_POINT].enabled             = false;
        
        entColPointsList[newEntityColPoints].colPoint[CENTER_DOWN_POINT].offset.x 		= 0;
        entColPointsList[newEntityColPoints].colPoint[CENTER_DOWN_POINT].offset.y 		= (entity->size.y >> 1);
        entColPointsList[newEntityColPoints].colPoint[CENTER_DOWN_POINT].colCode        = E_CENTER_COLLISION;
        entColPointsList[newEntityColPoints].colPoint[CENTER_DOWN_POINT].enabled        = false;

        MY_TRACE("[COLLISION SYSTEM]: Created entity collision points on position: %d\n", newEntityColPoints);
        MY_TRACE("[COLLISION SYSTEM]: Total of entity collision points: %d\n", numEntitiesColPoints);
    }
    else
        abort_on_error("ERROR: Reached max. number of entities collision points\n");        
}

void collision_destroy_entity_points(uint16_t entityId)
{
    uint16_t listPosition = get_collision_point_index_by_entId(entityId);

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

    MY_TRACE("[COLLISION SYSTEM]: Deleted entity collision points entId:%d on position: %d\n", entityId, listPosition);
    MY_TRACE("[COLLISION SYSTEM]: Total of entity collision points: %d\n", numEntitiesColPoints);
}

//funcion que aplica la direccion de la colision en el objeto
void applyDirCollision(tEntity *entity, int16_t colDir)
{
	//actions by collision
	if (colDir == E_LEFT_COLLISION || colDir == E_RIGHT_COLLISION) 
    {
		entity->fixVel.x = 0;        
    }
    else if (colDir == E_DOWN_COLLISION) 
    {
		//TODO: types
        /*
        //si es un solidItem
		if (isType(idEntity,TYPE solidItem))
			idEntity.this.vY *= -cBouncyObjectVel;
			//cantidad de rebote
			if ( abs(idEntity.this.vY) < cBouncyObjectVel )
				*objGrounded = true;		
			end;
		//si es un item
		elseif (isType(idEntity,TYPE Item))
			idEntity.this.vY *= -cBouncyItemVel;
			//cantidad de rebote
			if ( abs(idEntity.this.vY) < cBouncyItemVel )
				*objGrounded = true;		
			end;			
		else
        */
			entity->ground = true;            
        //}
    }
    else if (colDir == E_UP_COLLISION) 
    {
		entity->fixVel.y = 0;           //floats for ceiling        
		//idEntity.this.vY *= -1;		//Rebota hacia abajo con la velocida que subia
		//idEntity.this.vY = 2;		//Rebota hacia abajo con valor fijo
	}
}