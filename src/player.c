/********************************************************************
* Player system
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#include <stdbool.h>
#include <math.h>
#include "allegro.h"
#include "player.h"
#include "input.h"
#include "utils.h"
#include "game.h"
#include "entity.h"
#include "collisions.h"
#include "timer.h"

static fixed accel_x;
static fixed accel_x_air;
static fixed friction;
static fixed air_friction;
static fixed gravity;
static fixed accel_y;
static fixed max_vel_x;
static fixed max_vel_y;
static fixed cMinVelToIdle;

static fixed localFriction;
static fixed localAccelX;

//local player variables
static uint8_t playerCrouched = false;
static uint8_t playerMoving = false;
static uint8_t playerAttack = false;
static uint8_t playerHurt = false;
static uint8_t playerDead = false;
static int16_t playerInvincible = 0;

//test picking
static uint8_t objectForPickID = 0;
static uint8_t memObjectforPickID = 0;
tEntity *idObjectPicked;
static uint8_t picked = false;
static uint8_t picking = false;
static uint16_t pickingCounter = 0;

//local functions declarations
static void player_update_controls(tEntity *player);
static void player_update_collisions(tEntity *player);
static void player_update_state(tEntity *player);
static void player_update_animations(tEntity *player);

void player_init(tEntity *player)
{
    //initialize player vars
    player->state = ST_PLAYER_IDLE;
    playerCrouched = false;
    playerMoving = false;
    playerHurt = false;
    playerDead = false;
    playerInvincible = 0;
    player->ground = false;

    accel_x       = ftofix(PLAYER_ACCEL_X); 
    accel_x_air   = ftofix(PLAYER_ACCEL_X_AIR); 
    friction      = ftofix(PLAYER_FRICTION);
    air_friction  = ftofix(PLAYER_AIR_FRICTION);
    gravity       = ftofix(PLAYER_GRAVITY);
    accel_y       = ftofix(PLAYER_ACCEL_Y);
    max_vel_x     = ftofix(PLAYER_MAX_VEL_X);
    max_vel_y     = ftofix(PLAYER_MAX_VEL_Y);
    cMinVelToIdle = ftofix(PLAYER_MIN_VEL_TO_IDLE);
}

void player_update(tEntity *player)
{
    //update friction
    localFriction = player->ground ? friction: air_friction;
    localAccelX = player->ground ? accel_x : accel_x_air;

    //update controls
    player_update_controls(player);
    
    //update collisions    
    player_update_collisions(player);       

    
    //update velocities and position on main entity_update?
    /*
    //apply velocities
    player->fixPos.x += fixmul(player->fixVel.x, ftofix(deltaTime));        
    if (player->ground)
        player->fixVel.y = 0;  
    else  
    {
        //gravity 
	    player->fixVel.y += player->fixVel.y >= max_vel_y ? 0 : fixmul(gravity, ftofix(deltaTime));
        player->fixPos.y += fixmul(player->fixVel.y, ftofix(deltaTime));
    }
    
    
    //update position
    player->pos.x = fixtoi(player->fixPos.x);
    player->pos.y = fixtoi(player->fixPos.y);
    */
   
    //update state
    player_update_state(player);

    //update animations
    player_update_animations(player);
}

static void player_update_controls(tEntity *player)
{
    //update controls
    if (input_key_press(G_KEY_RIGHT) && player->fixVel.x < max_vel_x && !playerCrouched)
    {
        player->fixVel.x += fixmul(fixmul(localAccelX, (itofix(1) - friction)), ftofix(deltaTime));
        //player->fixVel.x+= fixmul(accel_x, ftofix(deltaTime));
        player->dir = E_ENT_DIR_RIGHT;
        playerMoving = true;
    }

    if (input_key_press(G_KEY_LEFT) && player->fixVel.x > -max_vel_x && !playerCrouched)
    {
        player->fixVel.x -= fixmul(fixmul(localAccelX, (itofix(1) - friction)), ftofix(deltaTime));
        //player->fixVel.x -= fixmul(accel_x, ftofix(deltaTime));
        player->dir = E_ENT_DIR_LEFT;
        playerMoving = true;
    }

    if (input_key_pressed(G_KEY_JUMP) && player->ground)
    {
        player->fixVel.y = -accel_y;
        player->ground = false;
    }

    if (input_key_pressed(G_KEY_ACTION))
    {
        //recojer objeto
			if (picking && !picked)
            {
				//comprobamos si podemos cojer el objeto
				//TODO: if (checkObjectPicking(memObjectforPickID))
				//{
                	picked = true;
					//cambiamos el estado del objeto a recogiendo
					idObjectPicked = entity_get(memObjectforPickID);
					idObjectPicked->signal = E_ENT_SIGNAL_PICKING;
					memObjectforPickID = 0;
                /*}
                else
                {
					picked = false;
					failPick = true;
				}*/
            }
            else if(!player->ground)
                playerAttack = true;
    }

    playerCrouched = input_key_press(G_KEY_DOWN) && player->ground;

    //update vels
    if ((!input_key_press(G_KEY_RIGHT) && !input_key_press(G_KEY_LEFT)) || playerCrouched)
    {
        //this the equivalent formula for vX *= friction with deltaTime
        player->fixVel.x = fixmul(player->fixVel.x, ftofix(pow(fixtof(localFriction), (deltaTime * fixtof(localFriction))))); 
        if (abs(player->fixVel.x) > cMinVelToIdle)
            playerMoving = true;
        else    
            playerMoving = false;
    }
}

static void player_update_state(tEntity *player)
{
    //update state
    player->prevState = player->state;    

    //recogiendo objetos
    //activacion picking
    if (objectForPickID != 0)
    {
        //si se cumple el tiempo definido
        if (pickingCounter >= PLAYER_PICKING_TIME)
        {
            //activamos el picking
            picking = true;
            memObjectforPickID = objectForPickID;
        }
        else
        {
            //cronometro				
            pickingCounter += get_clock_tick();
        }
    }
    else
        pickingCounter = 0;  
    
    //desactivacion picking
    if (picking && (player->fixVel.x != 0 || player->fixVel.y != 0 || playerCrouched) && !picked)
    {
        //si me muevo o sthis.alto o me agacho,salgo del picking
        picking = false;
        memObjectforPickID = 0;
    }


    if (playerDead)
        player->state = ST_PLAYER_DEAD;
    else if (playerHurt)
    {
        player->state = ST_PLAYER_HURT;   
        playerInvincible = PLAYER_INVINCIBLE_TIME;     
    }
    else if (picking && !picked)
    	player->state = ST_PLAYER_PICKING;	
    else if (picked && picking)
        player->state = ST_PLAYER_PICKED;
    else if (!player->ground == true)
    {
        if (playerAttack)
            player->state = ST_PLAYER_ATTACK;
        else
            player->state = ST_PLAYER_JUMP;
    }
    else if (playerAttack)
    {
        player->state = ST_PLAYER_LAND;
    }    
    else if (playerCrouched)
        player->state = ST_PLAYER_CROUCHED;
    else if (abs(player->fixVel.x) > cMinVelToIdle || playerMoving)
    {
        player->state = ST_PLAYER_RUN;
    }
    else
    {
        player->state = ST_PLAYER_IDLE;
    }

    //invincible flag
    playerInvincible = playerInvincible > 0 ? playerInvincible - get_clock_tick() : 0;
}

static void player_update_animations(tEntity *player)
{
    //update animations
    switch (player->state)
    {
        case ST_PLAYER_IDLE:
            play_animation(&player->anim, ANIM_PLY_BREATH);            
        break;
        case ST_PLAYER_RUN:
            play_animation(&player->anim, ANIM_PLY_RUN);
        break;
        case ST_PLAYER_JUMP:
            if (player->fixVel.y < 0)
                if (playerMoving)
                    play_animation(&player->anim, ANIM_PLY_JUMP_RUN_UP);
                else
                    play_animation(&player->anim, ANIM_PLY_JUMP_UP);
            else
                if (playerMoving)
                    play_animation(&player->anim, ANIM_PLY_JUMP_RUN_DOWN);
                else
                    play_animation(&player->anim, ANIM_PLY_JUMP_DOWN);
        break;
        case ST_PLAYER_CROUCHED:
            play_animation(&player->anim, ANIM_PLY_CROUCH);
        break;
        case ST_PLAYER_ATTACK:
            play_animation(&player->anim, ANIM_PLY_ATTACK);
        break;
        case ST_PLAYER_LAND:
            if (play_animation(&player->anim, ANIM_PLY_LAND))
                playerAttack = false;
        break;
        case ST_PLAYER_HURT:
            if (play_animation(&player->anim, ANIM_PLY_HURT))
            {
                playerHurt = false;                
                if (game.life == 0)
                {
                    playerDead = true;
                    playerInvincible = false;
                }
            }
        break;
        case ST_PLAYER_DEAD:
            if (play_animation(&player->anim, ANIM_PLY_FALL_DEAD))
                game.loseLive = true;    
        break;
        case ST_PLAYER_PICKING:
            play_animation(&player->anim, ANIM_PLY_PICKING);
        break;
        case ST_PLAYER_PICKED:
            play_animation(&player->anim, ANIM_PLY_PICKED);
        break;
    }

    //blink
    if (playerInvincible) 
        entity_blink(player);
    else
        player->visible = true;
}

static void player_update_collisions(tEntity *player)
{
    uint8_t colDir = 0;
    player->ground = false;
    objectForPickID = 0;

    //check all the entity collision points    
    for (uint8_t i = 0; i < NUM_COL_POINTS; i++)
    {                
        //check collision tile for collision point
        colDir = collision_check_tile(player, i);        
        //apply collision direction
        collision_apply_dir(player, colDir);        
    }

    //check entities collisions
    uint8_t numEntities = entities_get_num();
    tEntity *checkEntity;
    for (uint8_t i = 0; i < numEntities; i++)
    {
        checkEntity = entity_get(i);
        if (checkEntity->id != player->id && !checkEntity->dead)
        {
            switch (checkEntity->entClass)
            {
                case E_ENT_CLASS_OBJECT:
                    colDir = collision_check_entity(player, checkEntity, E_CHECK_PROCESS_VERTICALAXIS);
                    collision_apply_dir(player, colDir);
            
                    colDir = collision_check_entity(player, checkEntity, E_CHECK_PROCESS_HORIZONTALAXIS);

                    //comprobamos si colisionamos con un objeto recogible y esta en la mitad inferior
                    if (!picked && (colDir == E_COLLISION_RIGHT || colDir == E_COLLISION_LEFT)) 
                        //if (isBitSet(colID.this.props,OBJ_PICKABLE) && colID.y >= y)                        
                            objectForPickID = checkEntity->id;                                             

                    collision_apply_dir(player, colDir);            
                break;
                case E_ENT_CLASS_ENEMY:
                    if (!playerHurt && !playerInvincible && !playerDead)
                    {
                        colDir = collision_check_entity(player, checkEntity, E_CHECK_PROCESS_INFOONLY);     

                        if (colDir)
                        {
                            if (colDir == E_COLLISION_DOWN && playerAttack && checkEntity->signal != E_ENT_SIGNAL_HURT)
                            {
                                //send signal to entity
                                checkEntity->signal = E_ENT_SIGNAL_HURT;               
                                //set bounce velocity
                                player->fixVel.y = itofix(PLAYER_ENEMY_BOUNCE_VEL);
                            } 
                            else if (checkEntity->signal != E_ENT_SIGNAL_HURT)
                            {
                                //set flags
                                playerHurt = true;                                           
                                player->ground = false;
                                //lose 1 life
                                game.life -= 1;
                                //set hurt velocities
                                player->fixVel.y = itofix(PLAYER_HURT_VEL_Y);
                                player->fixVel.x = player->dir == E_ENT_DIR_RIGHT ? itofix(-PLAYER_HURT_VEL_X) : itofix(PLAYER_HURT_VEL_X);
                            }
                        }
                    }
                break;
            }            
        }
    }
}