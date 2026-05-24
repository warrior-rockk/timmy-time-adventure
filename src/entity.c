/********************************************************************
* Entity system
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#include <stdlib.h>
#include "utils.h"
#include "entity.h"
#include "collisions.h"
#include "player.h"
#include "objects.h"
#include "game.h"
#include "enemies.h"
#include "timer.h"

#define TRACE_FLAG  "[ENTITY]"

static tEntity *entityList;     //dynamic list of entities
static uint16_t numEntities;    //number of entities
static BITMAP *entitySprite;    //pointer to sub-bitmap of entity frame
uint8_t playerEntityId = 0;     //id of the player on array of entities

//private function to draw one entity
static void entity_draw(BITMAP *buffer, tEntity *entity)
{
    tVector scrollPos = scroll_get_position();

    if (entity->img)
    {
        int16_t drawX, drawY;
        
        //assign current frame sub-bitmap of entity        
        entitySprite = create_sub_bitmap(entity->img, entity->anim.frame * entity->spriteSize.x, 0, entity->spriteSize.x, entity->spriteSize.y);

        //check alignment axis
        switch (entity->axis)
        {
            case E_ENT_AXIS_DOWN:
                drawX = entity->pos.x - ((entity->spriteSize.x - entity->size.x) >>1);
                drawY = entity->pos.y - ((entity->spriteSize.y - entity->size.y) );
            break;
            case E_ENT_AXIS_UP:
                drawX = entity->pos.x - ((entity->spriteSize.x - entity->size.x) >>1);
                drawY = entity->pos.y;
            break;
            case E_ENT_AXIS_CENTER:
            default:
                drawX = entity->pos.x - ((entity->spriteSize.x - entity->size.x) >>1);
                drawY = entity->pos.y - ((entity->spriteSize.y - entity->size.y) >>1);
            break;
        }

        //draw entity sprite
        #ifdef DEBUGMODE
        if (debugOptions.showDebugInfo == DEBUG_SHOW_ALL_LAYER || !debugOptions.showDebugInfo)
        {
        #endif    
            if (entity->dir == E_ENT_DIR_RIGHT)   
                draw_sprite(buffer, entitySprite, drawX - scrollPos.x, drawY - scrollPos.y);                                
            else
                draw_sprite_h_flip(buffer, entitySprite, drawX - scrollPos.x, drawY - scrollPos.y);                        
        #ifdef DEBUGMODE
        }
        #endif
    }
    
    #ifdef DEBUGMODE        
        if (debugOptions.showDebugInfo >= DEBUG_SHOW_ALL_LAYER)
        {
            //draw debug entity collision box
            rect(buffer, entity->pos.x - scrollPos.x, entity->pos.y - scrollPos.y, (entity->pos.x + entity->size.x) - scrollPos.x , (entity->pos.y + entity->size.y) - scrollPos.y , 40);            

            //draw debug entity collision points
            if (collision_check_entity_col_points(entity->id))
            {
                tColPoint *entPoint;
                uint8_t entPointColor;
                for (uint8_t i = 0; i < E_NUM_COL_POINTS; i++)
                {
                    entPoint = collision_get_ent_collision_point(entity, i);
                    if (entPoint->enabled)
                        entPointColor = DEBUG_POINT_ENABLED_COLOR;
                    else
                        entPointColor = DEBUG_POINT_DISABLED_COLOR;                    
                    putpixel(buffer, entity->pos.x + entPoint->offset.x - scrollPos.x, entity->pos.y + entPoint->offset.y - scrollPos.y, entPointColor);
                }
            }            
        }
    #endif
}

//public functions
//================

//inits entity system
void entity_system_init()
{
    //free entity memory allocation
    free(entityList);
    entityList = NULL;
    //clear num entities
    numEntities = 0;    
    MY_TRACE_FLAG("Initialized entity system\n");
}

//destroys entity system
void entity_system_destroy()
{
    //free entity memory allocation
    free(entityList);
    entityList = NULL;
    //clear num entities
    numEntities = 0;
    MY_TRACE_FLAG("[ENTITY SYSTEM]: Destroyed entity system\n");
}

//creates new entity based on passed values
int16_t entity_create(uint8_t entityClass, uint8_t entityType, tVector initPos, enum E_ENTITY_DIR initDir, int16_t spare)
//, tVector size, BITMAP *img, uint8_t entType, uint16_t properties, void (*entity_init)(tEntity *entity), void (*entity_create)(tEntity *entity), void (*entity_update)(tEntity *entity))
{
    //inc num of entities
    numEntities++;
    
    if (numEntities <= ENTITY_MAX_NUM)
    {
        //allocate memory for entity
        entityList = realloc(entityList, numEntities * sizeof(tEntity));
        
        //test memory allocation
        ASSERT(entityList);

        //assign entity data
        uint16_t newEntity = numEntities - 1;

        //identification data
        entityList[newEntity].id              = newEntity;
        entityList[newEntity].entInstance     = 0;
        entityList[newEntity].img             = NULL;
        //parameters data
        entityList[newEntity].entClass        = entityClass;
        entityList[newEntity].entType         = entityType;
        entityList[newEntity].initPos         = initPos;        
        entityList[newEntity].initDir         = initDir;
        entityList[newEntity].spare           = spare;
        //initialize data
        entityList[newEntity].pos             = initPos;
        entityList[newEntity].dir             = initDir;
        entityList[newEntity].fixPos.x        = itofix(initPos.x);
        entityList[newEntity].fixPos.y        = itofix(initPos.y);
        entityList[newEntity].fixVel.x        = 0;
        entityList[newEntity].fixVel.y        = 0;
        entityList[newEntity].state           = 0;
        entityList[newEntity].prevState       = 0;
        entityList[newEntity].dead            = false;
        entityList[newEntity].visible         = true;
        entityList[newEntity].noGravity       = false;
        entityList[newEntity].anim.frameTime  = 0;

        //if entity class is player, set the player id
        if (entityClass == E_ENT_CLASS_PLAYER)
            playerEntityId = entityList[newEntity].id;
        
        //data based on entity class
        switch (entityList[newEntity].entClass)
        {
            case E_ENT_CLASS_PLAYER:                
                entityList[newEntity].spriteSize      = (tVector){PLAYER_IMG_W,PLAYER_IMG_H};  
                entityList[newEntity].size            = (tVector){PLAYER_SIZE_W,PLAYER_SIZE_H};        
                entityList[newEntity].axis            = E_ENT_AXIS_DOWN;
                entityList[newEntity].properties      = E_ENT_PROP_PHYSICS_ON;
                entityList[newEntity].entity_create   = &player_create;
                entityList[newEntity].entity_init     = &player_init;
                entityList[newEntity].entity_update   = &player_update;
                entityList[newEntity].entity_destroy  = &player_destroy;
            break;
            case E_ENT_CLASS_OBJECT:
            case E_ENT_CLASS_TRIGGER:       
            case E_ENT_CLASS_PLATFORM:            
                entityList[newEntity].properties      = 0x00;
                entityList[newEntity].entity_create   = &object_create;
                entityList[newEntity].entity_init     = &object_init;
                entityList[newEntity].entity_update   = &object_update;
                entityList[newEntity].entity_destroy  = &object_destroy;
                entityList[newEntity].entity_destroy  = NULL;
                entityList[newEntity].axis            = E_ENT_AXIS_CENTER;
            break;
            case E_ENT_CLASS_ENEMY:                   
                entityList[newEntity].properties      = 0x00;
                entityList[newEntity].entity_create   = &enemy_create;
                entityList[newEntity].entity_init     = &enemy_init;
                entityList[newEntity].entity_update   = &enemy_update;
                entityList[newEntity].entity_destroy  = &enemy_destroy;
                entityList[newEntity].axis            = E_ENT_AXIS_CENTER;
            break;
            default:
                abort_on_error("Entity type not valid %i", entityList[newEntity].entClass);
            break;
        }
        
        #if DEBUG_TRACE_CREATE_ENTITIES
            MY_TRACE_FLAG("Created entity ID:%i Class:%i Type:%i\n", entityList[newEntity].id, entityList[newEntity].entClass, entityList[newEntity].entType);
        #endif
        
        //check entity properties
        if (CHECK_FLAG(entityList[newEntity].properties, E_ENT_PROP_PHYSICS_ON))     
            collision_create_entity_points(&entityList[newEntity]);

        //call create function pointer of entity
        if (entityList[newEntity].entity_create) 
            entityList[newEntity].entity_create(&entityList[newEntity]);

        //call init function pointer of entity
        if (entityList[newEntity].entity_init) 
            entityList[newEntity].entity_init(&entityList[newEntity]);
        
        return newEntity;
    }
    else
    {
        abort_on_error("ERROR: Reached max. number of entities\n");
        return -1;
    }
}

//function to return a entity
tEntity* entity_get(uint16_t numEntity)
{
    if (entityList)
        return &entityList[numEntity];
    else   
        return NULL;
}

//function to return a entity by his entity class and instance. NULL if not found
tEntity* entity_get_by_instance(uint8_t entityClass, uint16_t entityInstance)
{
    //optimized running the list of entities backwards because the use of this function
    //is when remove a element from list and there's more probability to found on last positions
    for (uint16_t i = numEntities; i > 0; i--)
    {
        if (entityList[i].entClass == entityClass && entityList[i].entInstance == entityInstance)
            return &entityList[i];
    }

    return NULL;
}

//function to destroy entity by index entity number
void entity_destroy(uint16_t entityIndex)
{    
    //free entity collision point memory
    collision_destroy_entity_points(entityList[entityIndex].id);

    //call custom destroy entity
    if (entityList[entityIndex].entity_destroy)
    {
        entityList[entityIndex].entity_destroy(&entityList[entityIndex]);         
    }

    //copies last entity to deleted entity position
    entityList[entityIndex] = entityList[numEntities - 1];
    //decrement entity number
    numEntities--;
    if (numEntities == 0)
    {
        //free entity list
        free(entityList);
        entityList = NULL;
    }
    else
        //reallocates the array with decremented entity number    
        entityList = realloc(entityList, numEntities * sizeof(tEntity));
    
    MY_TRACE_FLAG("Destroyed entity ID:%i\n", entityIndex);
}

//function to init a entity
void entity_init(uint8_t entityIndex)
{
    //init entity data
    entityList[entityIndex].pos       = entityList[entityIndex].initPos;
    entityList[entityIndex].fixPos.x  = itofix(entityList[entityIndex].pos.x);
    entityList[entityIndex].fixPos.y  = itofix(entityList[entityIndex].pos.y);
    entityList[entityIndex].dir       = entityList[entityIndex].initDir;
    entityList[entityIndex].fixVel    = (tFixVector){0, 0};
    entityList[entityIndex].state     = 0;
    entityList[entityIndex].prevState = 0;
    entityList[entityIndex].anim.frame= 0;
    entityList[entityIndex].visible   = true;
    entityList[entityIndex].signal    = E_ENT_SIGNAL_NONE;
    //only reset dead if not NO_SPAWN property
    if (!CHECK_FLAG(entityList[entityIndex].properties, E_ENT_PROP_NO_SPAWN))
        entityList[entityIndex].dead      = 0;

    //call custom entity entity
    if (entityList[entityIndex].entity_init)
    {
        entityList[entityIndex].entity_init(&entityList[entityIndex]);          
    }
    
}

//function to init the entities 
void entities_init()
{
    for (int i=0; i < numEntities; i++)
    {
        //auto-destroy entities doesn't initialize
        if (CHECK_FLAG(entityList[i].properties, E_ENT_PROP_AUTO_DESTROY))
            entity_destroy(i);
        else
            entity_init(i);
    }   

    MY_TRACE_FLAG("All entities initialized\n");
}

//function to update entities
void entities_update()
{
    #ifdef DEBUGMODE
        struct entityCounter
        {
            uint8_t visibles;
            uint8_t sleeps;
            uint8_t deads;
        } entityCounter = {0};        
    #endif

    for (uint8_t i = 0; i < numEntities; i++)
    {
        //check dead flag
        if (entityList[i].dead)            
        {
            //reset dead flag if object out of region of initial position
            if (!scroll_rect_on_region((tRectangle){entityList[i].initPos, entityList[i].size}))
            {
                entityList[i].sleep = true;
                //if the entity doesn't is persistent, init the entity
                if (!CHECK_FLAG(entityList[i].properties, E_ENT_PROP_PERSISTENT))
                    entity_init(i);  
            }            
            #ifdef DEBUGMODE 
                entityCounter.deads++; 
                entityCounter.sleeps++; 
                entityList[i].sleep = true;
            #endif
        }
        //check entity (non-player) out of region (and no persistent property)
        else if (!scroll_rect_on_region((tRectangle){entityList[i].pos, entityList[i].size}) && entityList[i].id != entity_get_player_id() && !CHECK_FLAG(entityList[i].properties, E_ENT_PROP_PERSISTENT))
        {
            if (!entityList[i].sleep)
            {                
                MY_TRACE_FLAG("Entity: %i Instance: %i set to sleep for out of region\n", entityList[i].id, entityList[i].entInstance);                
                //sleep the entity
                entityList[i].sleep = true;
            }
            //check autodestroy flag to destroy entity
            if (CHECK_FLAG(entityList[i].properties, E_ENT_PROP_AUTO_DESTROY))
            {
                entity_destroy(i);                    
            }
            #ifdef DEBUGMODE                 
                entityCounter.sleeps++;             
            #endif
        }
        //check player out of region (only bottom)            
        else if (!scroll_rect_on_region((tRectangle){entityList[i].pos, entityList[i].size}) && entityList[i].id == entity_get_player_id() && entityList[i].pos.y > scroll_get_position().y)
        {
            //if is player, lose live (fall on edges)
            game.loseLive = true;
            MY_TRACE_FLAG("Lose player live because out region\n");            
        }        
        else
        {
            #ifdef DEBUGMODE 
                if (entityList[i].visible)
                    entityCounter.visibles++;
            #endif

            //if entity was sleeping, send awake signal 
            if (entityList[i].sleep)
                entityList[i].signal = E_ENT_SIGNAL_AWAKE;

            //clear flag
            entityList[i].sleep = false;                    
            //update previous state
            entityList[i].prevState = entityList[i].state;
            
            //call entity pointer update function
            if (entityList[i].entity_update)
            {
                entityList[i].entity_update(&entityList[i]);                          
            }
            
            //update vel and pos
            entity_update_vel_pos(&entityList[i]);
            
            //trace debug
            #ifdef DEBUGMODE
                #if DEBUG_TRACE_ENTITIES
                    if (entityList[i].state != entityList[i].prevState)
                        //TRACE("Entity %i changes from state %i to state %i\n", i, entityList[i].prevState, entityList[i].state);
                        MY_TRACE_FLAG("Entity %i changes from state %i to state %i\n", i, entityList[i].prevState, entityList[i].state);
                #endif
            #endif
        }
    }   

    #ifdef DEBUGMODE
        show_debug("NumEnt:%i, V:%i, S:%i, D:%i", numEntities, entityCounter.visibles, entityCounter.sleeps, entityCounter.deads);    
    #endif
}

//funtion to draws entities
void entities_draw(BITMAP *buffer)
{
    for (int i=numEntities - 1; i >= 0; i--)
    {
        //only draws if visible and not sleep
        if (entityList[i].visible && !entityList[i].sleep)
            entity_draw(buffer, &entityList[i]);
    }    
}

//funtion to destroy all entities
void entities_destroy_all()
{
    //destroy each entity
    for (int i=numEntities - 1; i >= 0; i--)
    {
        //entityList[i].dead = true;
        entity_destroy(i);
    }    
    //free current draw entity sprite
    free(entitySprite);
    entitySprite = NULL;
}

uint8_t entities_get_num()
{
    return numEntities;
}

void entity_update_vel_pos(tEntity *entity)
{
    //if entity has physics flag
    if (CHECK_FLAG(entity->properties, E_ENT_PROP_PHYSICS_ON))
    {
        //apply X velocity
        entity->fixPos.x += abs(entity->fixVel.x) < ftofix(ENTITY_MIN_VEL_X) ? 0 : fixmul(entity->fixVel.x, ftofix(deltaTime));        
        
        //apply Y velocity
        if (entity->ground)
        {
            entity->fixVel.y = 0;  
            //apply friction all entities except player (has is own friction logic)
            if (entity->id != entity_get_player_id())
                entity->fixVel.x = fixmul(entity->fixVel.x, ftofix(ENTITY_FRICTION));
        }
        else  
        {
            //gravity             
            if (!entity->noGravity)
            {
                entity->fixVel.y += entity->fixVel.y >= ftofix(ENTITY_MAX_VEL_Y) ? 0 : fixmul(ftofix(ENTITY_GRAVITY), ftofix(deltaTime));
                entity->fixPos.y += fixmul(entity->fixVel.y, ftofix(deltaTime));            
            }
        }
    }
    else
    {
        //apply velocity without gravity and friction
        entity->fixPos.x += fixmul(entity->fixVel.x, ftofix(deltaTime));
        entity->fixPos.y += fixmul(entity->fixVel.y, ftofix(deltaTime));
    }

    //update position
    entity->pos.x = fixtoi(entity->fixPos.x);
    entity->pos.y = fixtoi(entity->fixPos.y);    
}

void entity_blink(tEntity *entity)
{
    if (clock_counter_check(ENTITY_BLINK_SPEED))
        entity->visible = !entity->visible;
}

int16_t entity_center_x(tEntity *entity)
{
    return entity->pos.x + (entity->size.x >> 1);
}

int16_t entity_center_y(tEntity *entity)
{
    return entity->pos.y + (entity->size.y >> 1);
}

void entity_set_player_id(uint8_t playerId)
{
    playerEntityId = playerId;
}

uint8_t entity_get_player_id()
{
    return playerEntityId;
}

void entity_trace(tEntity *entity)
{
    #ifdef DEBUGMODE
        MY_TRACE_FLAG("Trace Entity id: %i:\n", entity->id);
        
        MY_TRACE_FLAG("\tEntity Type: %i\n", entity->entType);      
        MY_TRACE_FLAG("\tEntity Class: %i\n", entity->entClass);     
        MY_TRACE_FLAG("\tEntity Instance: %i\n", entity->entInstance);  
        MY_TRACE_FLAG("\tProperties: %i\n", entity->properties);   
        MY_TRACE_FLAG("\tInit pos x: %i Init pos y: %i\n", entity->initPos.x, entity->initPos.y);      
        MY_TRACE_FLAG("\tPos x: %i Pos y:%i\n", entity->pos.x , entity->pos.y);          
        MY_TRACE_FLAG("\tFix Pos x: %f Fix Pos y:%f\n", fixtof(entity->fixPos.x), fixtof(entity->fixPos.y));       
        MY_TRACE_FLAG("\tFix Vel x: %f Fix Vel y:%f\n", fixtof(entity->fixVel.x), fixtof(entity->fixVel.y));       
        MY_TRACE_FLAG("\tSize x: %i Size y:%i\n", entity->size.x, entity->size.y);         
        MY_TRACE_FLAG("\tSprite size x: %i Sprite size y:%i\n", entity->spriteSize.x, entity->spriteSize.y);   
        MY_TRACE_FLAG("\tInit Dir: %i\n", entity->initDir);  		
        MY_TRACE_FLAG("\tDir: %i\n", entity->dir);      		    
        MY_TRACE_FLAG("\tAxis: %i\n", entity->axis);         
        MY_TRACE_FLAG("\tState: %i\n", entity->state);        
        MY_TRACE_FLAG("\tPrevState: %i\n", entity->prevState);    
        MY_TRACE_FLAG("\tSignal: %i\n", entity->signal);       
        MY_TRACE_FLAG("\tDead: %i\n", entity->dead);         
        MY_TRACE_FLAG("\tSleep: %i\n", entity->sleep);        
        MY_TRACE_FLAG("\tVisible: %i\n", entity->visible);      
        MY_TRACE_FLAG("\tGround: %i\n", entity->ground);       
        MY_TRACE_FLAG("\tNo Gravity: %i\n", entity->noGravity);    
        MY_TRACE_FLAG("\tSpare: %i\n", entity->spare);     
        
        MY_TRACE_FLAG("End Trace Entity\n");
    #endif
}