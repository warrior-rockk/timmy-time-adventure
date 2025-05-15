/********************************************************************
* Entity system
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#include <stdlib.h>
#include "entity.h"

static tEntity *entityList;     //dynamic list of entities
static uint16_t numEntities;    //number of entities

//private function to draw one entity
static void entity_draw(BITMAP *buffer, tEntity *entity, tScroll *scroll)
{
    if (entity->img)
        draw_sprite(buffer, entity->img, entity->pos.x - scroll->pos.x, entity->pos.y - scroll->pos.y);
}

//public functions
//================

//inits entity system
void entity_system_init()
{
    //free entity memory allocation
    free(entityList);
    //clear num entities
    numEntities = 0;
}

//destroys entity system
void entity_system_destroy()
{
    //free entity memory allocation
    free(entityList);
    //clear num entities
    numEntities = 0;
}

//creates new entity based on passed values
int16_t entity_create(tVector initPos, BITMAP *img, uint8_t entType, void (*entity_init)(tEntity *entity), void (*entity_create)(tEntity *entity), void (*entity_update)(tEntity *entity))
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

        entityList[newEntity].initPos         = initPos;
        entityList[newEntity].pos             = initPos;
        entityList[newEntity].fixPos.x        = itofix(initPos.x);
        entityList[newEntity].fixPos.y        = itofix(initPos.y);
        entityList[newEntity].fixVel.x        = 0;
        entityList[newEntity].fixVel.y        = 0;
        entityList[newEntity].img             = img;
        entityList[newEntity].size.x          = img->w;
        entityList[newEntity].size.y          = img->h;
        entityList[newEntity].state           = 0;
        entityList[newEntity].prevState       = 0;
        entityList[newEntity].entType         = entType;
        entityList[newEntity].entInstance     = 0;
        entityList[newEntity].dead            = false;
        entityList[newEntity].visible         = true;
        entityList[newEntity].entity_create   = entity_create;
        entityList[newEntity].entity_init     = entity_init;
        entityList[newEntity].entity_update   = entity_update;

        //call create function pointer of entity
        if (entityList[newEntity].entity_create) 
            entityList[newEntity].entity_create(&entityList[newEntity]);
        
        return newEntity;
    }
    else
        abort_on_error("ERROR: Reached max. number of entities\n");
        return -1;
}

//function to return a entity
tEntity* get_entity(uint16_t numEntity)
{
    if (entityList)
        return &entityList[numEntity];
    else   
        return NULL;
}

//function to destroy entity by index entity number
void entity_destroy(uint16_t entityIndex)
{
    //copies last entity to deleted entity position
    entityList[entityIndex] = entityList[numEntities - 1];
    //decrement entity number
    numEntities--;
    if (numEntities == 0)
        //free entity list
        free(entityList);
    else
        //reallocates the array with decremented entity number    
        entityList = realloc(entityList, numEntities * sizeof(tEntity));
}

//function to init the entities 
void entities_init()
{
    for (int i=0; i < numEntities; i++)
    {
        //init entity data
        entityList[i].pos       = entityList[i].initPos;
        entityList[i].fixPos.x  = itofix(entityList[i].pos.x);
        entityList[i].fixPos.y  = itofix(entityList[i].pos.y);
        entityList[i].fixVel    = (tFixVector){0, 0};
        entityList[i].state     = 0;
        entityList[i].visible   = true;

        //call custom entity entity
        if (entityList[i].entity_init)
        {
            entityList[i].entity_init(&entityList[i]);          
        }
    }   
}

//function to update entities
void entities_update(tScroll *scroll)
{
    int visibleEntities = 0;

    for (int i=0; i < numEntities; i++)
    {
        //destroy the entity if dead
        if (entityList[i].dead)
            entity_destroy(i);
        //check entity on region
        else if (!scroll_rect_on_region((tRectangle){entityList[i].pos, entityList[i].size}, scroll))
        {
            entityList[i].visible = false;
        }
        else
        {
            //only update entity if visible
            entityList[i].visible = true;        
            visibleEntities++;
            if (entityList[i].entity_update)
            {
                entityList[i].entity_update(&entityList[i]);
                          
            }
        }
    }   

    show_debug("Num entities: %d, Visibles: %d", numEntities, visibleEntities);
    
}

//funtion to draws entities
void entities_draw(BITMAP *buffer, tScroll *scroll)
{
    for (int i=numEntities - 1; i >= 0; i--)
    {
        //only draws if visible
        if (entityList[i].visible)
            entity_draw(buffer, &entityList[i], scroll);
    }    
}

//funtion to destroy all entities
void entity_destroy_all()
{
    for (int i=numEntities - 1; i >= 0; i--)
    {
        entityList[i].dead = true;
    }    
}
