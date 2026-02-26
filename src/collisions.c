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
BITMAP *collisionMapSlope45;
BITMAP *collisionMapSlope135;

//inits collision system
void collision_system_init()
{
    //free collision memory allocation
    free(entColPointsList);
    entColPointsList = NULL;
    //clear num entities collision points
    numEntitiesColPoints = 0;

    collisionMapSlope135    = load_bmp("res/tiles/slope135.bmp", NULL);
    collisionMapSlope45     = load_bmp("res/tiles/slope45.bmp", NULL);
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

bool checkTileCode(tEntity *entity, uint8_t colDir, uint8_t tileProperty)
{
	//TODO://comprobamos si el tile es visible en la pantalla, asi, los tiles fuera de region no ser�n solidos
    //if (checkTileVisible(idEntity,posX,posY))
		switch(colDir)
        {
			//Colisiones superiores
			case E_COLLISION_UP: 
            case E_COLLISION_LEFT:
            case E_COLLISION_RIGHT:
				return !CHECK_FLAG(tileProperty, E_TILE_PROP_NO_SOLID);
                        //TODO: no scroll collision?
                       /*  ||
					   tileProperty == NO_SCROLL_L ||
					   tileProperty == NO_SCROLL_R;*/
			break;
			//Colisiones inferiores
		    case E_COLLISION_DOWN: 
            case E_COLLISION_CENTER:
				return !CHECK_FLAG(tileProperty, E_TILE_PROP_NO_SOLID)      ||
					    CHECK_FLAG(tileProperty, E_TILE_PROP_SLOPE_135)     ||
					    CHECK_FLAG(tileProperty, E_TILE_PROP_SLOPE_45);
					   //TODO: rest of collisions
                       /*||
                       tileProperty == NO_SCROLL_L ||
					   tileProperty == NO_SCROLL_R ||
					  (tileMap[posY][posX].tileCode == SOLID_ON_FALL && ( idEntity.this.vY>0 || isType(idEntity,TYPE player)) )||
					  (tileMap[posY][posX].tileCode == TOP_STAIRS && (idEntity.this.vY>0 || isType(idEntity,TYPE player)) );*/
			break;
            default:
                return 0;
            break;			
		}
	/*else
		return 0; 
	*/
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
            if (!CHECK_FLAG(map_get_tile_property(linePath->start), E_TILE_PROP_NO_SOLID))
            {
				//check tile propertyu to count as collision or not
				if (checkTileCode(entity, colCode, map_get_tile_property(linePath->start)))
                {
                    if (CHECK_FLAG(map_get_tile_property(linePath->start), E_TILE_PROP_SLOPE_45))
                    {
                        if (getpixel(collisionMapSlope45, (linePath->start.x % 16), (linePath->start.y % 16)) == 255)
                        {
                            return dist;
                        }
                    }
                    else if (CHECK_FLAG(map_get_tile_property(linePath->start), E_TILE_PROP_SLOPE_135))
                    {
                        if (getpixel(collisionMapSlope135, (linePath->start.x % 16), (linePath->start.y % 16)) == 255)
                        {
                            return dist;
                        }
                    }
                    else
                    {                        
                        show_debug("tilecode: %i, dist: %i", map_get_tile_property(linePath->start), dist);		
                        return dist;
                    }                    
                }
                //if (checkTileCode(idEntity,colCode,linePath.vStart.y/cTileSize,linePath.vStart.x/cTileSize))
				//{
                    //if(map_get_pixel(0,mapBox,(linePath.vStart.x%cTileSize),(linePath.vStart.y%cTileSize)) <> 0)
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
//fixed point for precision
fixed colCheckVectorY(tEntity *entity, tFixLinePath *linePath, uint16_t colCode, enum E_CHECKVECTORMODES mode)
{

    fixed dist = 0;		    //distance to collision
    fixed inc;			    //increment
    uint8_t colPixel = 0;	//color pixel of collision
	tVector checkPosition;  //convert to tVector for map_tile_exits

	//sets increment direction
    inc = linePath->end.y >= linePath->start.y ? ftofix(1) : ftofix(-1);

	//x component of vector doesn't change
    checkPosition.x = fixtoi(linePath->start.x);
        
    //line to set how many pxs the entity will be above florr (with 1, will be just 1 px above ground line)
	//linePath->start.y = linePath->start.y + itofix(1);

	//run vector searching pixel collision
	do
	{		        
        //y component of vector change. Refresh
        checkPosition.y = fixtoi(linePath->start.y);

        //check if tile exists on path position
        if (map_tile_exists(&checkPosition))
        {   
            //check if tile is not solid
            if (CHECK_FLAG(map_get_tile_property(checkPosition), E_TILE_PROP_NO_SOLID))
            {
                colPixel = 0;                
            }
            else
            {                
                //check tile property to count as collision or not
                if (checkTileCode(entity, colCode, map_get_tile_property(checkPosition)))
                {
                    if (CHECK_FLAG(map_get_tile_property(checkPosition), E_TILE_PROP_SLOPE_45))
                    {
                        colPixel = getpixel(collisionMapSlope45, (checkPosition.x % 16), (checkPosition.y % 16));                        
                    }
                    else if (CHECK_FLAG(map_get_tile_property(checkPosition), E_TILE_PROP_SLOPE_135))
                    {
                        colPixel = getpixel(collisionMapSlope135, (checkPosition.x % 16), (checkPosition.y % 16));                        
                    }
                    else
                    {                        
                        colPixel = 100;
                    }
                }
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
               //colPixel = 0; 
            }	
        }
        
        //check mode
        if (mode == E_CHECK_VECTOR_Y_TO_COLLISION)	
        {
            //if detects collision, return distance
            if (colPixel != 0)            
                return dist;                        
        }
        else
            //Mode FROM_COLLISION: if out of collision, return distance
            if (colPixel == 0 )
                return dist;            
                
        //increments distance
        dist = dist + itofix(1);
        //increments vector
        linePath->start.y = linePath->start.y + inc;	
    }
	//check all the vector
	while ((linePath->start.y < linePath->end.y && inc==itofix(1)) || (linePath->start.y > linePath->end.y && inc == itofix(-1)));
	
	//no collision
	return itofix(-1);
}

//function to check entity collision with tilemap. Returns: direction of collision
uint8_t collision_check_tile(tEntity *entity, uint16_t pointNum)
{ 
    tLinePath colLinePath;	    //Collision path line to check
    tFixLinePath fColLinePath;  //Collision path line to check Y (fixed)
    int16_t distColX;		    //X collision distance
    fixed distColY;		        //Y collision distance
    int16_t colDir;			    //Direction of collision

    colDir = 0;
    
    //gets collision point index
    uint16_t entIndex = get_collision_point_index_by_entId(entity->id);

    //check if collision point is active
    if (!entColPointsList[entIndex].colPoint[pointNum].enabled) 
        return colDir;
    
    //=====================
    //HORIZONTAL COLLISIONS
    //=====================
    
    #if USE_SLOPE_COLLISION 
        //deactivate down collision points if entity on slope        
        entColPointsList[entIndex].colPoint[E_COLPOINT_LEFT_DOWN].enabled = !CHECK_FLAG(map_get_tile_property((tVector){entity->pos.x + entColPointsList[entIndex].colPoint[E_COLPOINT_CENTER_DOWN].offset.x, entity->pos.y + entColPointsList[entIndex].colPoint[E_COLPOINT_CENTER_DOWN].offset.y}), E_TILE_PROP_SLOPE_135);
        entColPointsList[entIndex].colPoint[E_COLPOINT_RIGHT_DOWN].enabled = !CHECK_FLAG(map_get_tile_property((tVector){entity->pos.x + entColPointsList[entIndex].colPoint[E_COLPOINT_CENTER_DOWN].offset.x, entity->pos.y + entColPointsList[entIndex].colPoint[E_COLPOINT_CENTER_DOWN].offset.y}), E_TILE_PROP_SLOPE_45);
    #endif
    
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
    //VERTICAL COLLISIONS (with fixed point for precision)
    //===============
    
    //check if collision point is vertical
    if (entColPointsList[entIndex].colPoint[pointNum].colCode == E_COLLISION_UP || entColPointsList[entIndex].colPoint[pointNum].colCode == E_COLLISION_DOWN)
    {           
        //set the compare vector
        fColLinePath.start.x = entity->fixPos.x + itofix(entColPointsList[entIndex].colPoint[pointNum].offset.x);
        fColLinePath.end.x   = fColLinePath.start.x;
        fColLinePath.start.y = entity->fixPos.y + itofix(entColPointsList[entIndex].colPoint[pointNum].offset.y);
        fColLinePath.end.y   = fixadd(entity->fixPos.y, entity->fixVel.y) + itofix(entColPointsList[entIndex].colPoint[pointNum].offset.y);
        
        //TRACE("sx: %f ex: %f sy: %f ey: %f \n", fixtof(fColLinePath.start.x), fixtof(fColLinePath.end.x), fixtof(fColLinePath.start.y), fixtof(fColLinePath.end.y));
        //show_debug("sx: %f ex: %f sy: %f ey: %f \n", fixtof(fColLinePath.start.x), fixtof(fColLinePath.end.x), fixtof(fColLinePath.start.y), fixtof(fColLinePath.end.y));

        //call check vector collision on Y
        distColY = colCheckVectorY(entity, &fColLinePath, entColPointsList[entIndex].colPoint[pointNum].colCode, E_CHECK_VECTOR_Y_TO_COLLISION);
        
        //check if has collided
        if (distColY >= 0) 
        {               
            //down collision
            if (entColPointsList[entIndex].colPoint[pointNum].colCode == E_COLLISION_DOWN && entity->fixVel.y >= 0)
            { 
                //adjust the entity to border of collision
                entity->fixPos.y += distColY;                
                colDir = E_COLLISION_DOWN;
                
                #if USE_SLOPE_COLLISION
                    //Slope detection: check if buried
                                        
                    //define line path to check (center_down of entity)
                    fColLinePath.start.x = entity->fixPos.x + itofix(entColPointsList[entIndex].colPoint[E_COLPOINT_CENTER_DOWN].offset.x);       
                    fColLinePath.end.x   = fColLinePath.start.x;
                    fColLinePath.start.y = entity->fixPos.y + itofix(entColPointsList[entIndex].colPoint[E_COLPOINT_CENTER_DOWN].offset.y);
                    fColLinePath.end.y   = fColLinePath.start.y - itofix(SLOPE_MAX_HEIGHT);
                    
                    //check collision path Y
                    distColY = colCheckVectorY(entity, &fColLinePath, E_COLLISION_CENTER, E_CHECK_VECTOR_Y_FROM_COLLISION);                    
                    
                    //get up entity to slope
                    if (distColY > 0)
                        entity->fixPos.y = entity->fixPos.y - (distColY - itofix(1));
                #endif
            }                                 
            
            //up collision
            if (entColPointsList[entIndex].colPoint[pointNum].colCode == E_COLLISION_UP && entity->fixVel.y < 0)
            {
                //adjust the entity to border of collision
                entity->fixPos.y -= distColY;                
                colDir = E_COLLISION_UP;
            }
        }
        else 
        {            
            #if USE_SLOPE_COLLISION
                //If not collision (point on air), check if has slope down (< SLOPE_MAX_HEIGHT)
                                
                if (entity->fixVel.y >= 0)
                {
                    //define line path to check (center_down of entity)
                    fColLinePath.start.x = entity->fixPos.x + itofix(entColPointsList[entIndex].colPoint[E_COLPOINT_CENTER_DOWN].offset.x);       
                    fColLinePath.end.x   = fColLinePath.start.x;
                    fColLinePath.start.y = entity->fixPos.y + itofix(entColPointsList[entIndex].colPoint[E_COLPOINT_CENTER_DOWN].offset.y) - itofix(1);
                    fColLinePath.end.y   = fColLinePath.start.y + entity->fixVel.y + itofix(SLOPE_MAX_HEIGHT);

                    //check collision path Y                    
                    distColY = colCheckVectorY(entity, &fColLinePath, E_COLLISION_CENTER, E_CHECK_VECTOR_Y_TO_COLLISION);
                                        
                    //Down entity to hill
                    if (distColY > 0)
                        entity->fixPos.y = entity->fixPos.y + distColY;                    
                }                
            #endif
        }        
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
        uint8_t dividedSizeY    = entity->size.y / 3; //6; //TODO: adjust this point to climb slopes. The point can't be higher than 1 tile (16px)
        
        entColPointsList[newEntityColPoints].entId = entity->id;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_RIGHT_UP].offset.x 			= entity->size.x; //halfImgWidth + halfSizeX - 1;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_RIGHT_UP].offset.y 			= dividedSizeY;  
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_RIGHT_UP].colCode 	        = E_COLLISION_RIGHT;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_RIGHT_UP].enabled 	        = true;
        
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_RIGHT_DOWN].offset.x 		= entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_RIGHT_UP].offset.x;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_RIGHT_DOWN].offset.y 	    = entity->size.y - dividedSizeY;  
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_RIGHT_DOWN].colCode          = E_COLLISION_RIGHT;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_RIGHT_DOWN].enabled          = true;
        
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_LEFT_UP].offset.x 		    = 0; //halfImgWidth - halfSizeX + 1;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_LEFT_UP].offset.y 		    = entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_RIGHT_UP].offset.y;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_LEFT_UP].colCode             = E_COLLISION_LEFT;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_LEFT_UP].enabled             = true;
        
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_LEFT_DOWN].offset.x 		    = entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_LEFT_UP].offset.x;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_LEFT_DOWN].offset.y 		    = entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_RIGHT_DOWN].offset.y;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_LEFT_DOWN].colCode           = E_COLLISION_LEFT;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_LEFT_DOWN].enabled           = true;
        
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_DOWN_R].offset.x 		    = entity->size.x - dividedSizeX; //halfImgWidth + dividedSizeX; 
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_DOWN_R].offset.y 		    = entity->size.y;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_DOWN_R].colCode              = E_COLLISION_DOWN;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_DOWN_R].enabled              = true;
        
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_DOWN_L].offset.x 		    = dividedSizeX; //halfImgWidth - dividedSizeX;  
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_DOWN_L].offset.y 		    = entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_DOWN_R].offset.y;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_DOWN_L].colCode              = E_COLLISION_DOWN;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_DOWN_L].enabled              = true;
        
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_UP_R].offset.x 		        = entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_DOWN_R].offset.x;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_UP_R].offset.y 		        = 0;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_UP_R].colCode                = E_COLLISION_UP;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_UP_R].enabled                = true;
        
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_UP_L].offset.x 		        = entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_DOWN_L].offset.x;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_UP_L].offset.y 		        = entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_UP_R].offset.y;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_UP_L].colCode                = E_COLLISION_UP;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_UP_L].enabled                = true;
        
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_CENTER].offset.x 		    = halfSizeX; //halfImgWidth;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_CENTER].offset.y 		    = halfSizeY;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_CENTER].colCode              = E_COLLISION_CENTER;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_CENTER].enabled              = false; //<- false by default
        
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_CENTER_DOWN].offset.x 		= entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_CENTER].offset.x;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_CENTER_DOWN].offset.y 		= entity->size.y;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_CENTER_DOWN].colCode         = E_COLLISION_CENTER;
        entColPointsList[newEntityColPoints].colPoint[E_COLPOINT_CENTER_DOWN].enabled         = false; //<- false by default

        #if DEBUG_TRACE_COLL_POINTS_ARRAY
            MY_TRACE_FLAG("Created entity id: %i collision points on position: %d\n", entity->id, newEntityColPoints);
            MY_TRACE_FLAG("Total of entity collision points: %d\n", numEntitiesColPoints);
        #endif
    }
    else
        abort_on_error("ERROR: Reached max. number of entities collision points\n");        
}

void collision_disable_points_except(uint16_t entityId, uint8_t numPoint)
{
    //get collisions id by entity id
    uint16_t entIndex = get_collision_point_index_by_entId(entityId);   

    //disable all collision points
    for (uint8_t i = 0; i < E_NUM_COL_POINTS; i++)
        entColPointsList[entIndex].colPoint[numPoint].enabled = false;
    
    //enables the exception
    entColPointsList[entIndex].colPoint[numPoint].enabled = true;
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
void collision_apply_dir(tEntity *entity, int16_t colDir, uint8_t bounceMode)
{
	#define VEL_BOUNCE_SOFT     0.4
    #define VEL_BOUNCE_HARD     0.8

    //actions by collision
	if (colDir == E_COLLISION_LEFT || colDir == E_COLLISION_RIGHT) 
    {
		entity->fixVel.x = 0;        
    }
    else if (colDir == E_COLLISION_DOWN) 
    {
		fixed bounceVel;
        //sets bounce velocity or set ground if no bounce
        switch (bounceMode)
        {            
            case E_COLLISION_BOUNCE_SOFT:
                bounceVel = ftofix(VEL_BOUNCE_SOFT);
            break;
            case E_COLLISION_BOUNCE_HARD:
                bounceVel = ftofix(VEL_BOUNCE_HARD);
            break;
            case E_COLLISION_NO_BOUNCE:
            default:
                entity->ground = true;
            break;
        }
        //if bounce mode
        if (bounceMode == E_COLLISION_BOUNCE_SOFT || bounceMode == E_COLLISION_BOUNCE_HARD)
        {
            //bounces the entity
            entity->fixVel.y = fixmul(entity->fixVel.y, -bounceVel);            
            //until reaches the bounce velocity
            if ( abs(entity->fixVel.y) < bounceVel)
                entity->ground = true;
        }
    }
    else if (colDir == E_COLLISION_UP) 
    {
		entity->fixVel.y = 0;                                       //floats on ceiling        
		//entity->fixVel.y = fixmul(entity->fixVel.y, itofix(-1));  //bounces down with same velocity
        //entity->fixVel.y = itofix(2);                             //bounces down with fixed value
	}
}

//Function to check collision between processes. Mode selects horizontal, vertical or both collisions
//Position the entityA to edge of collision (if not mode INFOONLY) and returns collision direction or 0 if not collided
uint8_t collision_check_entity(tEntity *entityA, tEntity *entityB, enum E_CHECK_PROCESS_MODE mode)
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

bool collision_check_entity_col_points(uint16_t entityId)
{
    //find entity id on collision points list
    for (int i = 0; i < numEntitiesColPoints; i++)
    {
        if (entColPointsList[i].entId == entityId)
            return true;
    }

    return false;
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