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

uint16_t numObjectInstances;        //num of object instances
static void *objectDataList;        //list of object local data

void object_system_init()
{
    //empty object list
    free(objectDataList);
    //set number of entities
    numObjectInstances = 0;    
}

void object_system_destroy()
{
    //empty object list
    free(objectDataList);
    //set number of entities
    numObjectInstances = 0;    
}

//check object entity type to add the local data structure to local data list and increases instances number
void object_create(tEntity *entity)
{
    //inc num instances
    numObjectInstances++;

    //alloc memory for specified object type local data
    switch (entity->entType)
    {
        case E_GEM_OBJECT_TYPE:
            //allocate memory for gem Object
            objectDataList = realloc(objectDataList, numObjectInstances * sizeof(tGemLocalData));
            entity->img = load_bmp("res/objects/object.bmp",NULL);
            entity->size = (tVector){14, 16};    
            entity->spriteSize = (tVector){entity->img->w, entity->img->h};          
        break;
        case E_STONE_OBJECT_TYPE:
            //allocate memory for next stone Object
            objectDataList = realloc(objectDataList, numObjectInstances * sizeof(tStoneLocalData));
            entity->img = load_bmp("res/objects/rock.bmp",NULL);  
            entity->size = (tVector){16, 16};  
            entity->spriteSize = (tVector){16, 16};
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
};

//calls specified object type update function
void object_update(tEntity *entity)
{   
    switch (entity->entType)
    {
        case E_GEM_OBJECT_TYPE:            
            object_gem_update(entity, &((tGemLocalData*)objectDataList)[entity->entInstance]);
        break;
        case E_STONE_OBJECT_TYPE:
            object_stone_update(entity, &((tStoneLocalData*)objectDataList)[entity->entInstance]);
        break;
        default:
        break;
    }
}

//calls specified object type init function
void object_init(tEntity *entity)
{   
    switch (entity->entType)
    {
        case E_GEM_OBJECT_TYPE:            
            ((tGemLocalData*)objectDataList)[numObjectInstances - 1].health = 0;
            ((tGemLocalData*)objectDataList)[numObjectInstances - 1].timer = 0;            
        break;
        case E_STONE_OBJECT_TYPE:
            ((tStoneLocalData*)objectDataList)[numObjectInstances - 1].solid = false;            
        break;
        default:
        break;
    }
}

void object_gem_update(tEntity *this, tGemLocalData *local)
{
    switch (this->state)
    {
        case E_GEM_IDLE_STATE:
            if (local->timer >= this->pos.y)
            {
                this->state = E_GEM_MOVE_RIGHT_STATE;
                local->timer = 0;
            }
            else
                local->timer+= (1 * deltaTime);
        break;
        case E_GEM_MOVE_RIGHT_STATE:
            if (this->pos.x > 130)
                this->state = E_GEM_MOVE_LEFT_STATE;
            else   
                this->fixVel.x = ftofix(0.4);
        break;
        case E_GEM_MOVE_LEFT_STATE:
            if (this->pos.x < 70)
                this->state = E_GEM_MOVE_RIGHT_STATE;
            else   
                this->fixVel.x = ftofix(-0.4);
        break;
        default:
            this->state = E_GEM_IDLE_STATE;
    }

    //apply velocity
    this->fixPos.x += fixmul(this->fixVel.x, ftofix(deltaTime));
    this->fixPos.y += fixmul(this->fixVel.y, ftofix(deltaTime));

    //update position
    this->pos.x = fixtoi(this->fixPos.x);
    this->pos.y = fixtoi(this->fixPos.y);

    local->health = this->pos.x;    
}

void object_stone_update(tEntity *this, tStoneLocalData *local)
{
    //object states
    enum E_STONE_OBJECT_STATES{E_STONE_ST_IDLE, E_STONE_ST_PICKED, E_STONE_ST_THROWING, E_STONE_ST_BREAK};
    local->solid = true;

    if (this->signal == E_ENT_SIGNAL_HURT)
    {
        this->state = E_STONE_ST_BREAK;
        this->signal = 0;
    }

    switch (this->state)
    {
        case E_STONE_ST_IDLE:
            CLEAR_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            CLEAR_FLAG(this->properties, E_ENT_PROP_PHYSICS_ON);

            this->fixVel.x = 0;
            this->fixVel.y = 0;

            if (this->signal == E_ENT_SIGNAL_PICKING)
                this->state = E_STONE_ST_PICKED;           
        break;
        case E_STONE_ST_PICKED:
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
                
                this->fixVel.x = playerEnt->dir ? itofix(-2) : itofix(2);
                this->fixVel.y = itofix(-2);
                
                this->state = E_STONE_ST_THROWING;
            }
        break;
        case E_STONE_ST_THROWING:
            uint8_t colDir;
            this->ground = false;
            //check all the entity collision points    
            for (uint8_t i = 0; i < NUM_COL_POINTS; i++)
            {                
                //check collision tile for collision point
                colDir = collision_check_tile(this, i);
                if (CHECK_FLAG(this->properties, E_ENT_PROP_NO_BREAKABLE))
                    //apply collision direction
                    collision_apply_dir(this, colDir);                        
                else
                {
                    if (colDir)
                        this->state = E_STONE_ST_BREAK;
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
                                collision_apply_dir(this, colDir);
                            else
                            {
                                if (colDir)
                                    this->state = E_STONE_ST_BREAK;       
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
                                    this->state = E_STONE_ST_BREAK;                                 
                            }
                        break;
                    }            
                }
            }

            if (this->ground && abs(this->fixVel.x) < ftofix(0.1))
                this->state = E_STONE_ST_IDLE;
        break;
        case E_STONE_ST_BREAK:
            //stop object
            this->fixVel.x = 0;
            this->fixVel.y = 0;                                            
            CLEAR_FLAG(this->properties, E_ENT_PROP_PHYSICS_ON);
            SET_FLAG(this->properties, E_ENT_PROP_NO_COLLISION);
            //play break animation
            if (play_animation(&this->anim, ANIM_OBJECT_BREAK))
            {
                //put object to sleep
                this->sleep = true;                
            }
        break;
    }
}

void object_trace(tEntity *this)
{
    MY_TRACE("Object Instance: %d\n\tObj Type:%d\n", this->entInstance, this->entType);
}