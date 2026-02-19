/********************************************************************
* Collisions System
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/

#include <stdint.h>
#include "utils.h"
#include "collisions.h"
#include "map.h"

#define TRACE_FLAG  "[COLLISION]"

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
    entColPointsList = NULL;
    //clear num entities collision points
    numEntitiesColPoints = 0;
}

//gets collision point list index by entity id (-1 if not found)
int16_t get_collision_point_index_by_entId(uint16_t entityId)
{
    //find entity id on collision points list
    for (int i = 0; i < numEntitiesColPoints; i++)
    {
        if (entColPointsList[i].entId == entityId)
            return i;
    }

    return -1;
}

//gets a collision point for an entity
tColPoint* collision_get_ent_collision_point(tEntity *entity, uint8_t numPoint)
{
    uint16_t entIndex = get_collision_point_index_by_entId(entity->id);

    return &entColPointsList[entIndex].colPoint[numPoint];
}

//check distance to horizontal collision (or -1 if no collision) on a check vector
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
            if (!CHECK_FLAG(map_get_tile_code(&linePath->start), E_TILE_PROP_NO_SOLID))
            {
				//TODO: comprobar el codigo del tile para contarlo como colision o no
				//if (checkTileCode(idEntity,colCode,linePath.vStart.y/cTileSize,linePath.vStart.x/cTileSize))
				//{
                    //if(map_get_pixel(0,mapBox,(linePath.vStart.x%cTileSize),(linePath.vStart.y%cTileSize)) <> 0)
				
                show_debug("tilecode: %i, dist: %i", map_get_tile_code(&linePath->start), dist);		
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

//check distance to vertical collision (or -1 if no collision) on a check vector
//mode TO_COLLISION returns distance to collision and FROM_COLLISION distante to get out the collision
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
            if (!CHECK_FLAG(map_get_tile_code(&linePath->start), E_TILE_PROP_NO_SOLID))
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
	//check all the vector
	while ((linePath->start.y < linePath->end.y && inc==1) || (linePath->start.y > linePath->end.y && inc == -1));
	
	//no collision
	return -1;
}

//function to check entity collision with tilemap. Returns: direction of collision
uint8_t collision_check_tile(tEntity *entity, uint16_t pointNum)
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
    if (entColPointsList[entIndex].colPoint[pointNum].colCode == E_COLLISION_RIGHT || entColPointsList[entIndex].colPoint[pointNum].colCode == E_COLLISION_LEFT)
    {    
        //Set the path collision line to check
        colLinePath.start.x = entity->pos.x + entColPointsList[entIndex].colPoint[pointNum].offset.x;
        colLinePath.end.x   = fixtoi(fixadd(entity->fixPos.x, entity->fixVel.x)) + entColPointsList[entIndex].colPoint[pointNum].offset.x; //colLinePath.start.x + fixtoi(entity->fixVel.x);
        colLinePath.start.y = entity->pos.y + entColPointsList[entIndex].colPoint[pointNum].offset.y;
        colLinePath.end.y   = fixtoi(fixadd(entity->fixPos.y, entity->fixVel.y)) + entColPointsList[entIndex].colPoint[pointNum].offset.y; //colLinePath.start.y + fixtoi(entity->fixVel.y);
        
        //calls the collision check line path
        distColX = colCheckVectorX(entity,&colLinePath, entColPointsList[entIndex].colPoint[pointNum].colCode);
        
        //if collision
        if (distColX >= 0)
        {
            //Right collision
            if (entColPointsList[entIndex].colPoint[pointNum].colCode == E_COLLISION_RIGHT) 
            {
                //position entity to edge of collision
                entity->fixPos.x += itofix(distColX - 1);                
                colDir = E_COLLISION_RIGHT;
                
            }
            //Left collision
            if (entColPointsList[entIndex].colPoint[pointNum].colCode == E_COLLISION_LEFT) 			
            {
                //position entity to edge of collision
                entity->fixPos.x -= itofix(distColX - 1);           	
                colDir = E_COLLISION_LEFT;
            }
        }  
    }
    
    //===============
    //VERTICAL COLLISIONS
    //===============
    
    //check if collision point is vertical
    if (entColPointsList[entIndex].colPoint[pointNum].colCode == E_COLLISION_UP || entColPointsList[entIndex].colPoint[pointNum].colCode == E_COLLISION_DOWN)
    {           
        //set the compare vector
        colLinePath.start.x = entity->pos.x + entColPointsList[entIndex].colPoint[pointNum].offset.x;
        colLinePath.end.x   = colLinePath.start.x;
        colLinePath.start.y = entity->pos.y + entColPointsList[entIndex].colPoint[pointNum].offset.y;
        colLinePath.end.y   = fixtoi(fixadd(entity->fixPos.y, entity->fixVel.y)) + entColPointsList[entIndex].colPoint[pointNum].offset.y; //colLinePath.start.y + fixtoi(entity->fixVel.y);
        
        //TRACE("sx: %i ex: %i sy: %i ey: %i \n", colLinePath.start.x, colLinePath.end.x, colLinePath.start.y, colLinePath.end.y);

        //call check vector collision on Y
        distColY = colCheckVectorY(entity, &colLinePath, entColPointsList[entIndex].colPoint[pointNum].colCode, E_CHECK_VECTOR_Y_TO_COLLISION);
                        
        //check if has collided
        if (distColY >= 0) 
        {               
            //down collision
            if (entColPointsList[entIndex].colPoint[pointNum].colCode == E_COLLISION_DOWN && fixtoi(entity->fixVel.y) >=0)
            { 
                //adjust the entity to border of collision
                entity->fixPos.y += itofix(distColY);                
                colDir = E_COLLISION_DOWN;
                
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
            if (entColPointsList[entIndex].colPoint[pointNum].colCode == E_COLLISION_UP && fixtoi(entity->fixVel.y) < 0)
            {
                //adjust the entity to border of collision
                entity->fixPos.y -= itofix(distColY);                
                colDir = E_COLLISION_UP;
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

//function to create collision points to an entity
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

        //precalculate positions offsets for collision points
        //uint8_t halfImgWidth    = entity->img->w>>1;
        uint8_t halfSizeX       = entity->size.x>>1;
        uint8_t halfSizeY       = entity->size.y>>1;
        uint8_t dividedSizeX    = entity->size.x / 4; 
        uint8_t dividedSizeY    = entity->size.y / 6;
        
        entColPointsList[newEntityColPoints].entId = entity->id;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_RIGHT_UP].offset.x 			= entity->size.x; //halfImgWidth + halfSizeX - 1;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_RIGHT_UP].offset.y 			= dividedSizeY;  
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_RIGHT_UP].colCode 	        = E_COLLISION_RIGHT;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_RIGHT_UP].enabled 	        = true;
        
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_RIGHT_DOWN].offset.x 		= entColPointsList[newEntityColPoints].colPoint[COLPOINT_RIGHT_UP].offset.x;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_RIGHT_DOWN].offset.y 	    = entity->size.y - dividedSizeY;  
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_RIGHT_DOWN].colCode          = E_COLLISION_RIGHT;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_RIGHT_DOWN].enabled          = true;
        
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_LEFT_UP].offset.x 		    = 0; //halfImgWidth - halfSizeX + 1;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_LEFT_UP].offset.y 		    = entColPointsList[newEntityColPoints].colPoint[COLPOINT_RIGHT_UP].offset.y;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_LEFT_UP].colCode             = E_COLLISION_LEFT;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_LEFT_UP].enabled             = true;
        
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_LEFT_DOWN].offset.x 		    = entColPointsList[newEntityColPoints].colPoint[COLPOINT_LEFT_UP].offset.x;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_LEFT_DOWN].offset.y 		    = entColPointsList[newEntityColPoints].colPoint[COLPOINT_RIGHT_DOWN].offset.y;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_LEFT_DOWN].colCode           = E_COLLISION_LEFT;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_LEFT_DOWN].enabled           = true;
        
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_DOWN_R].offset.x 		    = entity->size.x - dividedSizeX; //halfImgWidth + dividedSizeX; 
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_DOWN_R].offset.y 		    = entity->size.y;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_DOWN_R].colCode              = E_COLLISION_DOWN;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_DOWN_R].enabled              = true;
        
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_DOWN_L].offset.x 		    = dividedSizeX; //halfImgWidth - dividedSizeX;  
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_DOWN_L].offset.y 		    = entColPointsList[newEntityColPoints].colPoint[COLPOINT_DOWN_R].offset.y;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_DOWN_L].colCode              = E_COLLISION_DOWN;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_DOWN_L].enabled              = true;
        
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_UP_R].offset.x 		        = entColPointsList[newEntityColPoints].colPoint[COLPOINT_DOWN_R].offset.x;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_UP_R].offset.y 		        = 0;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_UP_R].colCode                = E_COLLISION_UP;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_UP_R].enabled                = true;
        
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_UP_L].offset.x 		        = entColPointsList[newEntityColPoints].colPoint[COLPOINT_DOWN_L].offset.x;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_UP_L].offset.y 		        = entColPointsList[newEntityColPoints].colPoint[COLPOINT_UP_R].offset.y;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_UP_L].colCode                = E_COLLISION_UP;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_UP_L].enabled                = true;
        
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_CENTER].offset.x 		    = halfSizeX; //halfImgWidth;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_CENTER].offset.y 		    = halfSizeY;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_CENTER].colCode              = E_COLLISION_CENTER;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_CENTER].enabled              = false; //<- false by default
        
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_CENTER_DOWN].offset.x 		= entColPointsList[newEntityColPoints].colPoint[COLPOINT_CENTER].offset.x;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_CENTER_DOWN].offset.y 		= entity->size.y;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_CENTER_DOWN].colCode         = E_COLLISION_CENTER;
        entColPointsList[newEntityColPoints].colPoint[COLPOINT_CENTER_DOWN].enabled         = false; //<- false by default

        #if DEBUG_TRACE_COLL_POINTS_ARRAY
            MY_TRACE_FLAG("Created entity id: %i collision points on position: %d\n", entity->id, newEntityColPoints);
            MY_TRACE_FLAG("Total of entity collision points: %d\n", numEntitiesColPoints);
        #endif
    }
    else
        abort_on_error("ERROR: Reached max. number of entities collision points\n");        
}

//funcion to destroy collision point on a entity
void collision_destroy_entity_points(uint16_t entityId)
{
    int16_t listPosition = get_collision_point_index_by_entId(entityId);

    //if entity has collision points
    if (listPosition >= 0)
    {
        //copies last entity col points to deleted entity position
        entColPointsList[listPosition] = entColPointsList[numEntitiesColPoints - 1];
        //decrement entity col points number
        numEntitiesColPoints--;
        if (numEntitiesColPoints == 0)
        {
            //free entity list
            free(entColPointsList);
            entColPointsList = NULL;
        }
        else
            //reallocates the array with decremented entity number    
            entColPointsList = realloc(entColPointsList, numEntitiesColPoints * sizeof(tEntColPoints));    

        #if DEBUG_TRACE_COLL_POINTS_ARRAY
            MY_TRACE_FLAG("Deleted entity collision points entId:%d on position: %d\n", entityId, listPosition);
            MY_TRACE_FLAG("Total of entity collision points: %d\n", numEntitiesColPoints);
        #endif
    }    
    #if DEBUG_TRACE_COLL_POINTS_ARRAY
        else
            MY_TRACE_FLAG("Entity id:%i has no collision points to free\n", entityId);       
    #endif
}

//function to apply the direction of the collision to an entity
void collision_apply_dir(tEntity *entity, int16_t colDir)
{
	//actions by collision
	if (colDir == E_COLLISION_LEFT || colDir == E_COLLISION_RIGHT) 
    {
		entity->fixVel.x = 0;        
    }
    else if (colDir == E_COLLISION_DOWN) 
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
    else if (colDir == E_COLLISION_UP) 
    {
		entity->fixVel.y = 0;           //floats for ceiling        
		//idEntity.this.vY *= -1;		//Rebota hacia abajo con la velocida que subia
		//idEntity.this.vY = 2;		//Rebota hacia abajo con valor fijo
	}
}

//Function to check collision between processes. Mode selects horizontal, vertical or both collisions
//Position the entityA to edge of collision (if not mode INFOONLY) and returns collision direction or 0 if not collided
uint8_t collision_check_entity(tEntity *entityA, tEntity *entityB, enum eCheckProcessModes mode)
{
    fixed vcX, vcY, addHW, addHH, oX, oY, hWA, hHA, hWB, hHB;
    uint8_t colDir = 0;

	//check if entity is no collidable
    if (CHECK_FLAG(entityB->properties, E_ENT_PROP_NO_COLLISION) || entityB->dead || entityB->sleep)
    {    
        return 0;
    }
    //get half of size of entities
    hWA = itofix(entityA->size.x>>1);
    hHA = itofix(entityA->size.y>>1);
    hWB = itofix(entityB->size.x>>1);
    hHB = itofix(entityB->size.y>>1);

	//Obtains the center horizontal position vectors with the velocities
    if (mode == E_CHECK_PROCESS_BOTHAXIS || mode == E_CHECK_PROCESS_HORIZONTALAXIS || mode == E_CHECK_PROCESS_INFOONLY )
		vcX = (entityA->fixPos.x + hWA + entityA->fixVel.x) - (entityB->fixPos.x + hWB);
	else
		vcX = (entityA->fixPos.x + hWA) - (entityB->fixPos.x + hWB);
    //Obtains the center vertical position vectors with the velocities
	if (mode == E_CHECK_PROCESS_BOTHAXIS || mode == E_CHECK_PROCESS_VERTICALAXIS || mode == E_CHECK_PROCESS_INFOONLY )
		vcY = (entityA->fixPos.y + hHA + entityA->fixVel.y) - (entityB->fixPos.y + hHB);
	else
		vcY = (entityA->fixPos.y + hHA) - (entityB->fixPos.y + hHB);
	
	//add half of size of entities
	addHW = hWA + hWB;
	addHH = hHA + hHB;
	
    //if the x and y vectors are minus than half of sizes, there's collision
    if (abs(vcX) < addHW && abs(vcY) < addHH) 
    {    
		//calculate the collision direction
        oX = addHW - abs(vcX);
        oY = addHH - abs(vcY);
        
		if (oX >= oY)
        { 
            if (mode==E_CHECK_PROCESS_BOTHAXIS || mode==E_CHECK_PROCESS_VERTICALAXIS || mode==E_CHECK_PROCESS_INFOONLY )
            {
                if (vcY > 0) 			
                {
					colDir =  E_COLLISION_UP;
					if (mode != E_CHECK_PROCESS_INFOONLY)
                        entityA->fixPos.y += oY + entityA->fixVel.y;
                }    
				else
                { 
					colDir = E_COLLISION_DOWN;	
					if (mode != E_CHECK_PROCESS_INFOONLY)
					    entityA->fixPos.y -= oY - entityA->fixVel.y - 1;
					
				}
            }
        }
        else
        {
			if (mode == E_CHECK_PROCESS_BOTHAXIS || mode == E_CHECK_PROCESS_HORIZONTALAXIS || mode == E_CHECK_PROCESS_INFOONLY)
			{	
                if (vcX > 0) 
                {
					colDir = E_COLLISION_LEFT;
					if (mode != E_CHECK_PROCESS_INFOONLY)
					    entityA->fixPos.x += oX + entityA->fixVel.x;
					
                }
                else
                { 
					colDir = E_COLLISION_RIGHT;
					if (mode != E_CHECK_PROCESS_INFOONLY)
						entityA->fixPos.x -= oX - entityA->fixVel.x;
				}
            }
        }
	}
        
    //returns the collision dir
    return colDir;
}

//funcion que engloba la gestion de las fisicas de un proceso
//devuelve si hubo alguna colision hortizontal con el terreno, ya que las verticales
//las devuelve en el flag grounded
/*
uint8_t collision_simple_tile_physics(tEntity *entity, fixed friction)
{

	int i;					//Var auxiliar
	int collided = false; 	//flag de colision
	int colDir;				//Direccion colision
	
				
	if (!isBitSet(idEntity.this.props,NO_PHYSICS))
    {
		if (entity->ground)
            entity->fixVel.x *= friction;			
				
		entity->fixVel.y += gravity;
        idEntity.this.vY += gravity;
		
		entity->ground = false;
		collided = false;		
		
		//COLISION TERRENO
		//Recorremos la lista de puntos a comprobar
		for (i=0;i<cNumColPoints;i++)					
			//obtenemos la direccion de la colision
			colDir = colCheckTileTerrain(idEntity,i);
			//aplicamos la direccion de la colision
			applyDirCollision(idEntity,colDir,&grounded);
			//seteamos flag de colisionado
			if (colDir == COLDER || colDir == COLIZQ || colDir == COLUP)
				collided = true;
			end;
		end;
    }
    else
		entity->ground = true;
		
	
	return collided;	//devolvemos flag colision	
}
*/