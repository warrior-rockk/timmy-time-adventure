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
#include "sound.h"

static fixed accelX;
static fixed accelXAir;
static fixed friction;
static fixed airFriction;
static fixed accelY;
static fixed maxVelX;
static fixed maxVelY;
static fixed minVelToReset;

static fixed localFriction;
static fixed localAccelX;

//local player variables
static struct playerFlags
{
    uint16_t crouched       : 1;
    uint16_t moving         : 1;
    uint16_t attack         : 1;
    uint16_t hurt           : 1;
    uint16_t dead           : 1;
    uint16_t throwing       : 1;
    uint16_t disableMove    : 1;
    uint16_t picking        : 1;
    uint16_t picked         : 1;
    uint16_t jump           : 1;
} playerFlags;

static int16_t playerInvincible = 0;        //counter for player invincibility
static uint8_t objectForPickID = 0;         //actual frame object collision id
static uint8_t memObjectforPickID = 0;      //save actual object collision id
tEntity *objectPicked;                      //pointer to entity object picked
static uint16_t pickingCounter = 0;         //counter delay to pick object when collided
SAMPLE *playerSfx[SFX_PLAYER_NUM];          //player sfx array

//local functions declarations
static void player_update_controls(tEntity *player);
static void player_update_collisions(tEntity *player);
static void player_update_state(tEntity *player);
static void player_update_animations(tEntity *player);

void player_create(tEntity *player)
{
    //set fixed constants
    accelX       = ftofix(PLAYER_ACCEL_X); 
    accelXAir   = ftofix(PLAYER_ACCEL_X_AIR); 
    friction      = ftofix(PLAYER_FRICTION);
    airFriction  = ftofix(PLAYER_AIR_FRICTION);
    
    accelY       = ftofix(PLAYER_ACCEL_Y);
    maxVelX     = ftofix(PLAYER_MAX_VEL_X);    
    maxVelY     = ftofix(PLAYER_JUMP_VEL_Y);    
    minVelToReset = ftofix(PLAYER_MIN_VEL_X_TO_RESET);

    //load player sfx
    playerSfx[SFX_PLAYER_JUMP]  = load_wav("res/player/jump.wav");
    playerSfx[SFX_PLAYER_HURT]  = load_wav("res/player/hurt.wav");
    playerSfx[SFX_PLAYER_THROW] = load_wav("res/player/throw.wav");
    playerSfx[SFX_PLAYER_BOUNCE] = load_wav("res/player/bounce.wav");
    playerSfx[SFX_PLAYER_PICK] = load_wav("res/player/pick.wav");    
}

void player_init(tEntity *player)
{
    //initialize player vars    
    player->ground = false;
    playerInvincible = 0;
    memset(&playerFlags, 0, sizeof(playerFlags));
    
    //initialize state
    player->state = ST_PLAYER_IDLE;
}

void player_destroy(tEntity *player)
{
    //free player samples
    for (uint8_t i = 0; i < SFX_PLAYER_NUM; i++)
    {
        destroy_sample(playerSfx[i]);    
    }    
}

void player_update(tEntity *player)
{
    //update friction
    localFriction = player->ground ? friction: airFriction;
    localAccelX = player->ground ? accelX : accelXAir;

    //update controls
    player_update_controls(player);
    
    //update collisions    
    player_update_collisions(player);       
           
    //update state
    player_update_state(player);

    //update animations
    player_update_animations(player);

    #ifdef DEBUGMODE
        #if DEBUG_TRACE_ENTITIES
            if (player->state != player->prevState)
                TRACE("Player changes from state %i to state %i\n", player->prevState, player->state);
        #endif
        show_debug( "p.vX: %f", fixtof(player->fixVel.x));
        show_debug( "p.vY: %f", fixtof(player->fixVel.y));
        show_debug( "p.fX: %f,p.fY: %f", fixtof(player->fixPos.x), fixtof(player->fixPos.y));
        show_debug( "p.x: %d, p.y: %d", player->pos.x, player->pos.y);        
    #endif
}

static void player_update_controls(tEntity *player)
{
    if (!playerFlags.disableMove)
    {
        //Right direction control
        if (input_key_press(E_G_KEY_RIGHT) && player->fixVel.x < maxVelX) //&& !playerFlags.crouched)
        {
            player->fixVel.x += fixmul(fixmul(localAccelX, (itofix(1) - friction)), ftofix(deltaTime));
            //player->fixVel.x+= fixmul(accelX, ftofix(deltaTime));
            player->dir = E_ENT_DIR_RIGHT;
            playerFlags.moving = true;
        }
        
        //Left direction control
        if (input_key_press(E_G_KEY_LEFT) && player->fixVel.x > -maxVelX) // && !playerFlags.crouched)
        {
            player->fixVel.x -= fixmul(fixmul(localAccelX, (itofix(1) - friction)), ftofix(deltaTime));
            //player->fixVel.x -= fixmul(accelX, ftofix(deltaTime));
            player->dir = E_ENT_DIR_LEFT;
            playerFlags.moving = true;
        }

        //Down control (crouch)
        playerFlags.crouched = input_key_press(E_G_KEY_DOWN) && player->ground && !playerFlags.picked;

        //Jump control
        if (input_key_press(E_G_KEY_JUMP))
        {
            //if not flag jump and not falling
            if (!playerFlags.jump && player->fixVel.y <= 0)
            {
                //clear flags
                player->ground = false;
                playerFlags.attack = false;

                //apply y acceleration
                player->fixVel.y += -accelY;
                //if reached max jump velocity, set flag
                if (player->fixVel.y < -(maxVelY))
                    playerFlags.jump = true;
                
                //play sfx jump
                if (player->state != ST_PLAYER_JUMP)
                    sfx_play(playerSfx[SFX_PLAYER_JUMP], E_SFX_PLAYER_VOICE, false);
            }
        }
        else 
        {
            //if not press jump, set flag jump
            if (!input_key_press(E_G_KEY_JUMP))
                playerFlags.jump = true;
            //reset jump flag on ground
            if(player->ground)                
                playerFlags.jump = false;
        }        
    }

    //Action control (atack, pick)
    if (input_key_pressed(E_G_KEY_ACTION))
    {
        //pick object if it's not picked
        if (playerFlags.picking && !playerFlags.picked)
        {
            //check if can pick object
            //TODO: if (checkObjectPicking(memObjectforPickID))
            //{
                playerFlags.picked = true;                
                //send picking signal to entity object
                objectPicked = entity_get(memObjectforPickID);
                objectPicked->signal = E_ENT_SIGNAL_PICKING;
                memObjectforPickID = 0;
                sfx_play(playerSfx[SFX_PLAYER_PICK], E_SFX_PLAYER_VOICE, false);
            /*}
            else
            {
                picked = false;
                failPick = true;
            }*/
        }
        //throw object if picked
        else if (playerFlags.picked && objectPicked)
        {            
            playerFlags.throwing = true;
            objectPicked->signal = E_ENT_SIGNAL_THROW;
            objectPicked = NULL;
            //reset flags
            playerFlags.picked = false;  
            sfx_play(playerSfx[SFX_PLAYER_THROW], E_SFX_PLAYER_VOICE, false);        
            
        }
        else if(!player->ground && !playerFlags.picked)
            playerFlags.attack = true;
    }

    //apply friction
    if ((!input_key_press(E_G_KEY_RIGHT) && !input_key_press(E_G_KEY_LEFT)) || playerFlags.crouched)
    {
        //this the equivalent formula for vX *= friction with deltaTime
        player->fixVel.x = fixmul(player->fixVel.x, ftofix(pow(fixtof(localFriction), (deltaTime * fixtof(localFriction))))); 
        //limit min x velocity
        if (abs(player->fixVel.x) > minVelToReset)
            playerFlags.moving = true;
        else{    
            playerFlags.moving = false;
            player->fixVel.x = 0;
        }
    }
}

static void player_update_collisions(tEntity *player)
{
    uint8_t colDir = 0;
    player->ground = false;
    objectForPickID = 0;

    //dimensions control
    if (playerFlags.crouched)
    {
        if (player->size.y != PLAYER_SIZE_H_CROUCH)
        {
            //set size crouched
            player->size.y = PLAYER_SIZE_H_CROUCH;
            //adjust position
            player->fixPos.y = player->fixPos.y + itofix(PLAYER_SIZE_H - PLAYER_SIZE_H_CROUCH);
            //recalculate collision points
            collision_set_collision_points(player, collision_get_point_index_by_entId(player->id));
        }
    }
    else
    {
        if (player->size.y != PLAYER_SIZE_H)
        {            
            //set normal size
            player->size.y = PLAYER_SIZE_H;
            //adjust position
            player->fixPos.y = player->fixPos.y - itofix(PLAYER_SIZE_H - PLAYER_SIZE_H_CROUCH);            
            //recalculate collision points
            collision_set_collision_points(player, collision_get_point_index_by_entId(player->id));
        }
    }

    //check all the entity collision points with tilemap     
    for (uint8_t i = 0; i < E_NUM_COL_POINTS; i++)
    {                
        //check collision tile for collision point
        colDir = collision_check_tile(player, i);        
        //apply collision direction
        collision_apply_dir(player, colDir, E_COLLISION_NO_BOUNCE);        
    }

    //check entities collisions
    uint8_t numEntities = entities_get_num();
    tEntity *checkEntity;
    for (uint8_t i = 0; i < numEntities; i++)
    {
        //get entity to check
        checkEntity = entity_get(i);
        
        //if the entity is not the player and it's not dead
        if (checkEntity->id != player->id && !checkEntity->dead)
        {
            //check entity class
            switch (checkEntity->entClass)
            {
                //objects
                case E_ENT_CLASS_OBJECT:
                    //check vertical collision with entity
                    colDir = collision_check_entity(player, checkEntity, E_CHECK_PROCESS_VERTICALAXIS);
                    //if collided
                    if (colDir)
                    {
                        //if collision dir down and attacking and object breakable
                        if (colDir == E_COLLISION_DIR_DOWN && !CHECK_FLAG(checkEntity->properties, E_ENT_PROP_NO_BREAKABLE) && playerFlags.attack && checkEntity->signal != E_ENT_SIGNAL_HURT)
                        {
                            //send hurt signal
                            checkEntity->signal = E_ENT_SIGNAL_HURT;
                            //set bounce velocity
                            player->fixVel.y = itofix(PLAYER_ATTACK_BOUNCE_VEL);
                            //play sfx
                            sfx_play(playerSfx[SFX_PLAYER_BOUNCE], E_SFX_PLAYER_VOICE, false);
                        }
                        else
                            //adjust collision position (object solid)
                            collision_apply_dir(player, colDir, E_COLLISION_NO_BOUNCE);
                    }
                    
                    //check horizontal collision with entity
                    colDir = collision_check_entity(player, checkEntity, E_CHECK_PROCESS_HORIZONTALAXIS);

                    //if lateral collision, check if is object pickable and on lower position to the player                    
                    if ((colDir == E_COLLISION_DIR_RIGHT || colDir == E_COLLISION_DIR_LEFT) && !CHECK_FLAG(checkEntity->properties, E_ENT_PROP_NO_PICKABLE) 
                         && !playerFlags.picked && checkEntity->pos.y >= player->pos.y)                        
                            objectForPickID = checkEntity->id;                                             
                    
                    //adjust collision position (object solid)
                    collision_apply_dir(player, colDir, E_COLLISION_NO_BOUNCE);            
                break;

                //enemies
                case E_ENT_CLASS_ENEMY:
                    //if player is not dead
                    if (!playerFlags.dead)
                    {
                        //check collision with entity but not adjust positions
                        colDir = collision_check_entity(player, checkEntity, E_CHECK_PROCESS_INFOONLY);     
                        //if collided
                        if (colDir)
                        {
                            //if collision down and attacking
                            if (colDir == E_COLLISION_DIR_DOWN && playerFlags.attack && checkEntity->signal != E_ENT_SIGNAL_HURT)
                            {
                                //send signal to entity
                                checkEntity->signal = E_ENT_SIGNAL_HURT;  
                                //add score
                                game.score += SCORE_POINT_HURT_ENEMY;             
                                //set bounce velocity
                                player->fixVel.y = itofix(PLAYER_ATTACK_BOUNCE_VEL);
                            } 
                            //if not attacking (hurt player)
                            else if (checkEntity->signal != E_ENT_SIGNAL_HURT && !playerFlags.hurt && !playerInvincible)
                            {
                                //set hurt flag
                                playerFlags.hurt = true;                                 
                            }
                        }
                    }
                break;
            }            
        }
    }
}

static void player_update_state(tEntity *player)
{
    //reset flag
    playerFlags.disableMove = false;
    
    //picking objects
    if (objectForPickID != 0)
    {
        //check picking counter
        if (pickingCounter >= PLAYER_PICKING_TIME)
        {
            playerFlags.picking = true;
            memObjectforPickID = objectForPickID;
        }
        else
        {
            pickingCounter += get_clock_tick();
        }
    }
    else
        pickingCounter = 0;  
    
    //disable picking when move, crouch or not picked
    if (playerFlags.picking && (player->fixVel.x != 0 || player->fixVel.y != 0 || playerFlags.crouched) && !playerFlags.picked)
    {
        playerFlags.picking = false;
        memObjectforPickID = 0;
    }

    //when throwing disable move
    if (playerFlags.throwing)
    {
        playerFlags.disableMove = true;
        player->fixVel.x = 0;        
    }    

    //invincible flag
    playerInvincible = playerInvincible > 0 ? playerInvincible - get_clock_tick() : 0;

    //set the state (priority order)
    if (playerFlags.dead)
    {
        player->state = ST_PLAYER_DEAD;
        playerFlags.disableMove = true;
        player->fixVel.x = 0;
    }
    else if (playerFlags.hurt)
    {
        playerFlags.disableMove = true;
        player->state = ST_PLAYER_HURT;   
        playerInvincible = PLAYER_INVINCIBLE_TIME; 
        
        //rising edge of the state
        if (player->prevState != player->state)
        {
            //play hurt sfx
            sfx_play(playerSfx[SFX_PLAYER_HURT], E_SFX_PLAYER_VOICE, false);                                                                          
            //lose life
            game.life -= 1;
            //set hurt velocities
            player->ground = false;
            player->fixVel.y = itofix(PLAYER_HURT_VEL_Y);
            player->fixVel.x = player->dir == E_ENT_DIR_RIGHT ? itofix(-PLAYER_HURT_VEL_X) : itofix(PLAYER_HURT_VEL_X);
            //if object picked, we lose it
            if (playerFlags.picked)
            {                                    
                objectPicked->signal = E_ENT_SIGNAL_THROW;
                objectPicked = NULL;
                //reset flags
                playerFlags.picked = false;                                    
            }
        }
    }
    else if (playerFlags.picking && !playerFlags.picked)
    	player->state = ST_PLAYER_PICKING;	
    else if (playerFlags.picked && playerFlags.picking)
        player->state = ST_PLAYER_PICKED;
    else if (playerFlags.throwing)
        player->state = ST_PLAYER_THROWING;
    else if (!player->ground == true)
    {
        if (playerFlags.attack)
            player->state = ST_PLAYER_ATTACK;
        else        
            player->state = ST_PLAYER_JUMP;           
    }
    else if (playerFlags.attack)
    {
        player->state = ST_PLAYER_LAND;
    }    
    //else if (playerFlags.crouched)
    //{
    //    player->state = ST_PLAYER_CROUCHED;        
    //}
    else if (abs(player->fixVel.x) > minVelToReset || playerFlags.moving)
    {
        player->state = ST_PLAYER_RUN;
    }
    else
    {
        player->state = ST_PLAYER_IDLE;
    }
}

static void player_update_animations(tEntity *player)
{
    //update animations
    switch (player->state)
    {
        case ST_PLAYER_IDLE:
            if (playerFlags.picked)
                play_animation(&player->anim, ANIM_PLY_PICKED);
            else if(playerFlags.crouched)
                play_animation(&player->anim, ANIM_PLY_CROUCH);            
            else
                play_animation(&player->anim, ANIM_PLY_BREATH);            
        break;
        case ST_PLAYER_RUN:
            if (playerFlags.picked)
                play_animation(&player->anim, ANIM_PLY_RUN_PICKED);
            else{
                if (!playerFlags.crouched)
                    play_animation(&player->anim, ANIM_PLY_RUN);
                else
                    play_animation(&player->anim, ANIM_PLY_WALK_CROUCH);
            }
        break;
        case ST_PLAYER_JUMP:
            if (player->fixVel.y < 0)
            {
                if (playerFlags.picked)
                    play_animation(&player->anim, ANIM_PLY_JUMP_UP_PICKED);
                else if (playerFlags.moving)
                    play_animation(&player->anim, ANIM_PLY_JUMP_RUN_UP);
                else
                    play_animation(&player->anim, ANIM_PLY_JUMP_UP);
            }
            else
            {
                if (playerFlags.picked)
                    play_animation(&player->anim, ANIM_PLY_JUMP_DOWN_PICKED);
                else if (playerFlags.moving)
                    play_animation(&player->anim, ANIM_PLY_JUMP_RUN_DOWN);
                else
                    play_animation(&player->anim, ANIM_PLY_JUMP_DOWN);
            }
        break;
        case ST_PLAYER_CROUCHED:
            play_animation(&player->anim, ANIM_PLY_CROUCH);
        break;
        case ST_PLAYER_ATTACK:
            play_animation(&player->anim, ANIM_PLY_ATTACK);
        break;
        case ST_PLAYER_LAND:
            if (play_animation(&player->anim, ANIM_PLY_LAND))
                playerFlags.attack = false;
        break;
        case ST_PLAYER_HURT:
            if (play_animation(&player->anim, ANIM_PLY_HURT))
            {
                playerFlags.hurt = false;                
                if (game.life == 0)
                {
                    playerFlags.dead = true;
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
            if (play_animation(&player->anim, ANIM_PLY_PICKED))
            {
                playerFlags.picking = false;
                player->state = ST_PLAYER_IDLE;
            }
        break;
        case ST_PLAYER_THROWING:
            if (player->ground)
            {
                if (play_animation(&player->anim, ANIM_PLY_THROW))
                {
                    playerFlags.throwing = false;
                    player->state = ST_PLAYER_IDLE;
                }    
            }
            else
            {
                if (play_animation(&player->anim, ANIM_PLY_THROW_AIR))
                {
                    playerFlags.throwing = false;
                    player->state = ST_PLAYER_IDLE;
                }    
            }
        break;
    }

    //blink
    if (playerInvincible) 
        entity_blink(player);
    else
        player->visible = true;
}