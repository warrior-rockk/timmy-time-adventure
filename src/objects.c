/********************************************************************
* Object entities (test)
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#include "allegro.h"
#include "utils.h"
#include "game.h"
#include "objects.h"
#include "collisions.h"

#define TRACE_FLAG  "[OBJECT]"

uint16_t numObjectInstances;        //num of object instances
static void *objectDataList;        //list of object local data
BITMAP *objectResources[E_OBJECTS_TYPE_NUM];

void object_system_init()
{
    //empty object list
    free(objectDataList);
    objectDataList = NULL;
    //set number of entities
    numObjectInstances = 0;    

    MY_TRACE_FLAG("Initialized object system\n");
}

void object_system_destroy()
{
    //empty object list
    free(objectDataList);
    objectDataList = NULL;
    //set number of entities
    numObjectInstances = 0;

    //free resources
    for (uint8_t i = 0; i < E_OBJECTS_TYPE_NUM; i++)
    {
        if (objectResources[i])
        {
            destroy_bitmap(objectResources[i]);
            objectResources[i] = NULL;
        }
    }
    MY_TRACE_FLAG("Destroyed object system\n");
}

//check object entity type to add the local data structure to local data list and increases instances number
void object_create(tEntity *entity)
{
    //inc num instances
    numObjectInstances++;

    //allocate memory for general solid object
    objectDataList = realloc(objectDataList, numObjectInstances * sizeof(tSolidObjectLocalData));

    //set object properties    
    switch (entity->entType)
    {
        case E_GEM_OBJECT_TYPE:            
            if (!objectResources[E_GEM_OBJECT_TYPE])
                objectResources[E_GEM_OBJECT_TYPE] = load_bmp("res/objects/object.bmp", NULL);

            entity->img = objectResources[E_GEM_OBJECT_TYPE];
            entity->spriteSize = (tVector){entity->img->w, entity->img->h};
            entity->size = (tVector){14, 16};                
        break;
        case E_STONE_OBJECT_TYPE:            
            if (!objectResources[E_STONE_OBJECT_TYPE])
                objectResources[E_STONE_OBJECT_TYPE] = load_bmp("res/objects/stone.bmp", NULL);

            entity->img = objectResources[E_STONE_OBJECT_TYPE];
            entity->spriteSize = (tVector){16, 16};
            entity->size = (tVector){16, 16};              
            collision_create_entity_points(entity);                    
        break;
        case E_ROCK_OBJECT_TYPE:            
            if (!objectResources[E_ROCK_OBJECT_TYPE])
                objectResources[E_ROCK_OBJECT_TYPE] = load_bmp("res/objects/rock.bmp", NULL);

            entity->img = objectResources[E_ROCK_OBJECT_TYPE];
            entity->spriteSize = (tVector){16, 16};
            entity->size = (tVector){16, 16};             
            collision_create_entity_points(entity);      
            entity->properties = E_ENT_PROP_NO_BREAKABLE | E_ENT_PROP_PERSISTENT;            
        break;
        case E_ROCK_2_OBJECT_TYPE:            
            if (!objectResources[E_ROCK_2_OBJECT_TYPE])
                objectResources[E_ROCK_2_OBJECT_TYPE] = load_bmp("res/objects/rock2.bmp", NULL);

            entity->img = objectResources[E_ROCK_2_OBJECT_TYPE];
            entity->spriteSize = (tVector){16, 16};
            entity->size = (tVector){16, 16};      
            collision_create_entity_points(entity);                    
        break;
        case E_EGG_OBJECT_TYPE:            
            if (!objectResources[E_EGG_OBJECT_TYPE])
                objectResources[E_EGG_OBJECT_TYPE] = load_bmp("res/objects/egg.bmp", NULL);

            entity->img = objectResources[E_EGG_OBJECT_TYPE];
            entity->spriteSize = (tVector){21, 16};
            entity->size = (tVector){21, 16};      
            collision_create_entity_points(entity);                    
        break;
        default:
            abort_on_error("Tipo de entidad objeto no reconocida");
        break;
    }
    
    //test memory allocation
    MY_ASSERT(objectDataList);

    //set actual instance num
    entity->entInstance = numObjectInstances - 1;  
    
    MY_TRACE_FLAG("Created object instance:%i\n", entity->entInstance);
};

//calls specified object type update function
void object_update(tEntity *entity)
{   
    switch (entity->entType)
    {
        case E_GEM_OBJECT_TYPE:            
            object_gem_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;       
        default:
            object_solid_update(entity, &((tSolidObjectLocalData*)objectDataList)[entity->entInstance]);
        break;
    }
}

//calls specified object type init function
void object_init(tEntity *entity)
{   
    switch (entity->entType)
    {        
        default:
            ((tSolidObjectLocalData*)objectDataList)[numObjectInstances - 1].timer = 0;
        break;
    }
}

void object_destroy(tEntity *entity)
{
    uint16_t objectIndex;
    objectIndex = entity->entInstance;
    
    //copies last enemy to deleted enemy position
    ((tSolidObjectLocalData*)objectDataList)[objectIndex] = ((tSolidObjectLocalData*)objectDataList)[numObjectInstances - 1];
    
    //decrement entity number
    numObjectInstances--;
    if (numObjectInstances == 0)
    {
        //free entity list
        free(objectDataList);
        objectDataList = NULL;
    }
    else
        //reallocates the array with decremented entity number    
        //TODO: this is incorrect when entities has different local data structure
        objectDataList = realloc(objectDataList, numObjectInstances * sizeof(tSolidObjectLocalData));     

    MY_TRACE_FLAG("Destroyed object instance:%i\n", objectIndex);
}

void object_gem_update(tEntity *this, tSolidObjectLocalData *local)
{
    //object states
    enum E_GEM_STATE {E_GEM_ST_IDLE};

    switch (this->state)
    {
        case E_GEM_ST_IDLE:
            this->fixVel.x = this->dir == E_ENT_DIR_LEFT ? itofix(-1) : itofix(1);            
            
            //change direction on range patrol
            if ((this->dir && this->pos.x > (this->initPos.x + 20)) || (!this->dir && this->pos.x < (this->initPos.x - 20)))
                this->dir = !this->dir;
        break;        
        default:
            this->state = E_GEM_ST_IDLE;
    }
}

void object_solid_update(tEntity *this, tSolidObjectLocalData *local)
{
    //object defines
    #define SOLID_THROW_VEL_X   2
    #define SOLID_THROW_VEL_Y   -2

    //object states
    enum E_SOLID_OBJECT_STATES{E_SOLID_ST_IDLE, E_SOLID_ST_PICKED, E_SOLID_ST_THROWING, E_SOLID_ST_BREAK};
    
    if (this->signal == E_ENT_SIGNAL_HURT)
    {
        this->state = E_SOLID_ST_BREAK;
        this->signal = 0;
    }

    switch (this->state)
    {
        case E_SOLID_ST_IDLE:
            CLEAR_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            CLEAR_FLAG(this->properties, E_ENT_PROP_PHYSICS_ON);

            this->fixVel.x = 0;
            this->fixVel.y = 0;

            if (this->signal == E_ENT_SIGNAL_PICKING)
                this->state = E_SOLID_ST_PICKED;           
        break;
        case E_SOLID_ST_PICKED:
            SET_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            
            //TODO: comprobamos si el jugador no muere cuando nos lleva
            //if (exists(idPlayer))
                tEntity *playerEnt = entity_get(PLAYER_ENTITY_ID);
                this->fixPos.x = playerEnt->dir ? playerEnt->fixPos.x - itofix(0) : playerEnt->fixPos.x + itofix(0);
                //isBitSet(idPlayer.flags,B_HMIRROR) ? this.fX = idPlayer.x-cObjectPickedPosX : this.fX = idPlayer.x+cObjectPickedPosX;
                this->fixPos.y  = playerEnt->fixPos.y - itofix(20);
                //correccion de altura por grafico player
                /*if (idPlayer.graph == 28)
                    this.fY ++;
                end;
                if (idPlayer.graph == 29)
                    this.fY += 2;
                end;*/
                //el objeto se vuelve persistente
                //setBit(this.props,PERSISTENT);
                //reseteamos flag boton si lo hubiera seteado el proceso
                //if (idButton == father) 
                //	idButton = 0;
                //end;
            /*else
                this.state = THROWING_STATE;
            end;*/
            if (this->signal == E_ENT_SIGNAL_THROW)
            {
                CLEAR_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
                SET_FLAG(this->properties, E_ENT_PROP_PHYSICS_ON);
                
                this->fixVel.x = playerEnt->dir == E_ENT_DIR_LEFT ? itofix(-SOLID_THROW_VEL_X) : itofix(SOLID_THROW_VEL_X);
                this->fixVel.y = itofix(SOLID_THROW_VEL_Y);
                this->ground = false;
                
                this->state = E_SOLID_ST_THROWING;
            }
        break;
        case E_SOLID_ST_THROWING:            
            uint8_t colDir;
            this->ground = false;
            //check all the entity collision points    
            for (uint8_t i = 0; i < E_NUM_COL_POINTS; i++)
            {                
                //check collision tile for collision point
                colDir = collision_check_tile(this, i);
                if (CHECK_FLAG(this->properties, E_ENT_PROP_NO_BREAKABLE))
                    //apply collision direction
                    collision_apply_dir(this, colDir, E_COLLISION_BOUNCE_SOFT);                        
                else
                {
                    if (colDir)
                        this->state = E_SOLID_ST_BREAK;
                }
            }
            
            //check entities collisions            
            uint8_t numEntities = entities_get_num();
            tEntity *checkEntity;
            for (uint8_t i = 0; i < numEntities; i++)
            {
                checkEntity = entity_get(i);
                if (checkEntity->id != this->id && checkEntity->id != PLAYER_ENTITY_ID && !checkEntity->dead)
                {
                    switch (checkEntity->entClass)
                    {
                        case E_ENT_CLASS_OBJECT:
                            colDir = collision_check_entity(this, checkEntity, E_CHECK_PROCESS_BOTHAXIS);
                            if (CHECK_FLAG(this->properties, E_ENT_PROP_NO_BREAKABLE))
                                collision_apply_dir(this, colDir, E_COLLISION_NO_BOUNCE);
                            else
                            {
                                if (colDir)
                                    this->state = E_SOLID_ST_BREAK;       
                            }
                        break;
                        case E_ENT_CLASS_ENEMY:
                            colDir = collision_check_entity(this, checkEntity, E_CHECK_PROCESS_INFOONLY);
                            if (colDir)
                            {
                                //send signal to entity
                                checkEntity->signal = E_ENT_SIGNAL_HURT;
                                if (!CHECK_FLAG(this->properties, E_ENT_PROP_NO_BREAKABLE))
                                    //change state
                                    this->state = E_SOLID_ST_BREAK;                                 
                            }
                        break;
                    }            
                }
            }

            if (this->ground && abs(this->fixVel.x) < ftofix(0.1))
                this->state = E_SOLID_ST_IDLE;
        break;
        case E_SOLID_ST_BREAK:
            //stop object
            this->fixVel.x = 0;
            this->fixVel.y = 0;                                            
            CLEAR_FLAG(this->properties, E_ENT_PROP_PHYSICS_ON);
            SET_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            //play break animation
            if (play_animation(&this->anim, ANIM_OBJECT_BREAK))
            {
                //put object to sleep
                //this->sleep = true;                
                this->dead = true;
            }
        break;
    }
}

void object_trace(tEntity *this)
{
    MY_TRACE_FLAG("Object Instance: %d\n\tObj Type:%d\n", this->entInstance, this->entType);
}