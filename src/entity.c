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

//private function to draw one entity
static void entity_draw(BITMAP *buffer, tEntity *entity, tScroll *scroll)
{
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
        if (debugOptions.showDebugInfo == DEBUG_SHOW_ALL_LAYER || !debugOptions.showDebugInfo)
        {
            if (entity->dir == E_ENT_DIR_RIGHT)   
                draw_sprite(buffer, entitySprite, drawX - scroll->pos.x, drawY - scroll->pos.y);                                
            else
                draw_sprite_h_flip(buffer, entitySprite, drawX - scroll->pos.x, drawY - scroll->pos.y);                        
        }
    }
    
    #ifdef DEBUGMODE        
        if (debugOptions.showDebugInfo >= DEBUG_SHOW_ALL_LAYER)
        {
            //draw debug entity collision box
            rect(buffer, entity->pos.x - scroll->pos.x, entity->pos.y - scroll->pos.y, (entity->pos.x + entity->size.x) - scroll->pos.x , (entity->pos.y + entity->size.y) - scroll->pos.y , 40);            

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
                    putpixel(buffer, entity->pos.x + entPoint->offset.x - scroll->pos.x, entity->pos.y + entPoint->offset.y - scroll->pos.y, entPointColor);
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
int16_t  entity_create(uint8_t entityClass, uint8_t entityType, tVector initPos, enum E_ENTITY_DIR initDir)
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
        //parameters data
        entityList[newEntity].entClass        = entityClass;
        entityList[newEntity].entType         = entityType;
        entityList[newEntity].initPos         = initPos;        
        entityList[newEntity].initDir         = initDir;
        //initialize data
        entityList[newEntity].pos             = initPos;
        entityList[newEntity].fixPos.x        = itofix(initPos.x);
        entityList[newEntity].fixPos.y        = itofix(initPos.y);
        entityList[newEntity].fixVel.x        = 0;
        entityList[newEntity].fixVel.y        = 0;
        entityList[newEntity].state           = 0;
        entityList[newEntity].prevState       = 0;
        entityList[newEntity].dead            = false;
        entityList[newEntity].visible         = true;
        //data based on entity class
        switch (entityList[newEntity].entClass)
        {
            case E_ENT_CLASS_PLAYER:
                entityList[newEntity].img             = load_bmp("res/player/player.bmp",NULL);
                entityList[newEntity].spriteSize      = (tVector){PLAYER_IMG_W,PLAYER_IMG_H};  
                entityList[newEntity].size            = (tVector){PLAYER_W,PLAYER_H};        
                entityList[newEntity].axis            = E_ENT_AXIS_DOWN;
                entityList[newEntity].properties      = E_ENT_PROP_PHYSICS_ON;
                entityList[newEntity].entity_create   = &player_create;
                entityList[newEntity].entity_init     = &player_init;
                entityList[newEntity].entity_update   = &player_update;
                entityList[newEntity].entity_destroy  = &player_destroy;
            break;
            case E_ENT_CLASS_OBJECT:
                entityList[newEntity].properties      = 0x00;
                entityList[newEntity].entity_create   = &object_create;
                entityList[newEntity].entity_init     = &object_init;
                entityList[newEntity].entity_update   = &object_update;
                entityList[newEntity].entity_destroy  = NULL;
                entityList[newEntity].axis            = E_ENT_AXIS_CENTER;
            break;
            case E_ENT_CLASS_ENEMY:
                entityList[newEntity].properties      = 0x00;
                entityList[newEntity].entity_create   = &enemy_create;
                entityList[newEntity].entity_init     = &enemy_init;
                entityList[newEntity].entity_update   = &enemy_update;
                entityList[newEntity].entity_destroy  = NULL;
                entityList[newEntity].axis            = E_ENT_AXIS_CENTER;
            break;
            default:
                abort_on_error("Tipo de entidad desconocida %i", entityList[newEntity].entClass);
            break;
        }
        
        //check entity properties
        if (CHECK_FLAG(entityList[newEntity].properties, E_ENT_PROP_PHYSICS_ON))     
            collision_create_entity_points(&entityList[newEntity]);

        //call create function pointer of entity
        if (entityList[newEntity].entity_create) 
            entityList[newEntity].entity_create(&entityList[newEntity]);
        
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

//function to destroy entity by index entity number
void entity_destroy(uint16_t entityIndex)
{    
    //free entity collision point memory
    //if (CHECK_FLAG(entityList[entityIndex].properties, E_ENT_PROP_PHYSICS_ON))
    collision_destroy_entity_points(entityList[entityIndex].id);

    //unload player entity spriteSheet
    if (entityIndex == PLAYER_ENTITY_ID && entityList[entityIndex].img)
    {        
        destroy_bitmap(entityList[entityIndex].img);
    }

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
}

//function to init a entity
void entity_init(uint8_t entityIndex)
{
    //if the entity doesn't have PERSISTENT flag (PERSISTENT entities don't initialize again)
    if (!CHECK_FLAG(entityList[entityIndex].properties, E_ENT_PROP_PERSISTENT))
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
        entityList[entityIndex].dead      = 0;
        entityList[entityIndex].visible   = true;
        entityList[entityIndex].signal    = E_ENT_SIGNAL_NONE;

        //call custom entity entity
        if (entityList[entityIndex].entity_init)
        {
            entityList[entityIndex].entity_init(&entityList[entityIndex]);          
        }
    }
}

//function to init the entities 
void entities_init()
{
    for (int i=0; i < numEntities; i++)
    {
        entity_init(i);
    }   
}

//function to update entities
void entities_update(tScroll *scroll)
{
    int visibleEntities = 0;

    for (int i=0; i < numEntities; i++)
    {
        //check dead flag
        if (entityList[i].dead)
            entityList[i].sleep = true;
        //check entity on region
        else if (!scroll_rect_on_region((tRectangle){entityList[i].pos, entityList[i].size}, scroll))
        {
            entityList[i].sleep = true;
            entity_init(i);           
        }
        else
        {
            //only update entity if visible
            entityList[i].sleep = false;        
            visibleEntities++;

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

    show_debug("NumEnt: %d, Visib: %d", numEntities, visibleEntities);    
}

//funtion to draws entities
void entities_draw(BITMAP *buffer, tScroll *scroll)
{
    for (int i=numEntities - 1; i >= 0; i--)
    {
        //only draws if visible and not sleep
        if (entityList[i].visible && !entityList[i].sleep)
            entity_draw(buffer, &entityList[i], scroll);
    }    
}

//funtion to destroy all entities
void entity_destroy_all()
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
    //apply velocity
    //entity->fixPos.x += fixmul(entity->fixVel.x, ftofix(deltaTime));
    //entity->fixPos.y += fixmul(entity->fixVel.y, ftofix(deltaTime));

    //update position
    //entity->pos.x = fixtoi(entity->fixPos.x);
    //entity->pos.y = fixtoi(entity->fixPos.y);
    
    
    //if entity has physics flag
    if (CHECK_FLAG(entity->properties, E_ENT_PROP_PHYSICS_ON))
    {
        fixed gravity       = ftofix(0.2);
        fixed max_vel_y     = ftofix(6);

        //apply velocities
        entity->fixPos.x += fixmul(entity->fixVel.x, ftofix(deltaTime));        
        if (entity->ground)
        {
            entity->fixVel.y = 0;  
            //apply friction all entities except player
            if (entity->id != PLAYER_ENTITY_ID)
                entity->fixVel.x = fixmul(entity->fixVel.x, ftofix(0.92));
        }
        else  
        {
            //gravity 
            entity->fixVel.y += entity->fixVel.y >= max_vel_y ? 0 : fixmul(gravity, ftofix(deltaTime));
            entity->fixPos.y += fixmul(entity->fixVel.y, ftofix(deltaTime));
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
    if (get_clock_count(ENTITY_BLINK_SPEED))
        entity->visible = !entity->visible;
}