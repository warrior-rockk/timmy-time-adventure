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
#include "map.h"
#include "resources.h"
#include "scroll.h"

#include "data/pdata.h"

#define TRACE_FLAG      "[PLAYER]"

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

static DATAFILE_INDEX* playerDataFileIndex;

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
    uint16_t onStairs       : 1;
    uint16_t spare11        : 1;
    uint16_t spare12        : 1;
    uint16_t spare13        : 1;
    uint16_t spare14        : 1;
    uint16_t spare15        : 1;
} playerFlags;

static int16_t playerInvincible = 0;        //counter for player invincibility
static uint8_t objectForPickID = 0;         //actual frame object collision id
static uint8_t memObjectforPickID = 0;      //save actual object collision id
static uint8_t objectPickedID = 0;          //id to entity object picked
static uint16_t pickingCounter = 0;         //counter delay to pick object when collided
static uint8_t playeridleCounter = 0;             //counter to idle animation
SAMPLE *playerSfx[SFX_PLAYER_NUM];          //player sfx array

//local functions declarations
static void player_update_controls(tEntity *player);
static void player_update_collisions(tEntity *player);
static void player_update_state(tEntity *player);
static void player_update_animations(tEntity *player);
static void player_update_size(tEntity *player);
#ifdef DEBUGMODE
static void player_trace(tEntity *player);
#endif

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

    //create data file index for fast open individual data objects
    playerDataFileIndex = create_dat_index("player.dat");

    //load player sprite
    player->img                     = load_dat_bmp_indexed(playerDataFileIndex, PLAYER_BMP);

    //load player sfx
    playerSfx[SFX_PLAYER_JUMP]      = load_dat_wav_indexed(playerDataFileIndex, JUMP_WAV);
    playerSfx[SFX_PLAYER_HURT]      = load_dat_wav_indexed(playerDataFileIndex, HURT_WAV);
    playerSfx[SFX_PLAYER_THROW]     = load_dat_wav_indexed(playerDataFileIndex, THROW_WAV);
    playerSfx[SFX_PLAYER_BOUNCE]    = load_dat_wav_indexed(playerDataFileIndex, BOUNCE_WAV);
    playerSfx[SFX_PLAYER_PICK]      = load_dat_wav_indexed(playerDataFileIndex, PICK_WAV);
    playerSfx[SFX_PLAYER_DEAD]      = load_dat_wav_indexed(playerDataFileIndex, DEAD_WAV);
    playerSfx[SFX_PLAYER_STAIR]     = load_dat_wav_indexed(playerDataFileIndex, STAIR_WAV);
}

void player_init(tEntity *player)
{
    //initialize player vars    
    player->ground = false;
    playerInvincible = 0;
    playeridleCounter = 0;
    memset(&playerFlags, 0, sizeof(playerFlags));
    
    //initialize state
    player->state = ST_PLAYER_IDLE;
}

void player_destroy(tEntity *player)
{
    MY_TRACE_FLAG("Destroy player on ID: %i\n", player->id);

    //free player samples
    for (uint8_t i = 0; i < SFX_PLAYER_NUM; i++)
    {
        destroy_sample(playerSfx[i]);    
    }    

    //free player bitmaps
    destroy_bitmap(player->img);

    destroy_dat_index(playerDataFileIndex);
}

void player_update(tEntity *player)
{
    //update friction
    localFriction = player->ground ? friction: airFriction;
    localAccelX = player->ground ? accelX : accelXAir;

    //update controls
    player_update_controls(player);
    
    //update size
    player_update_size(player);
    
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
        #if DEBUG_TRACE_PLAYER
            player_trace(player);
        #endif
    #endif
}

static void player_update_controls(tEntity *player)
{
    if (!playerFlags.disableMove)
    {
        
        //Right direction control
        if (input_key_press(E_G_KEY_RIGHT) && player->fixVel.x < maxVelX && !playerFlags.onStairs)
        {
            player->fixVel.x += fixmul(fixmul(localAccelX, (itofix(1) - friction)), ftofix(deltaTime));
            //player->fixVel.x+= fixmul(accelX, ftofix(deltaTime));
            player->dir = E_ENT_DIR_RIGHT;
            playerFlags.moving = true;
        }
        
        //Left direction control
        if (input_key_press(E_G_KEY_LEFT) && player->fixVel.x > -maxVelX && !playerFlags.onStairs && !input_key_press(E_G_KEY_RIGHT))
        {
            player->fixVel.x -= fixmul(fixmul(localAccelX, (itofix(1) - friction)), ftofix(deltaTime));
            //player->fixVel.x -= fixmul(accelX, ftofix(deltaTime));
            player->dir = E_ENT_DIR_LEFT;
            playerFlags.moving = true;
        }
        

        //Down control (crouch / go down stairs)
        if (input_key_press(E_G_KEY_DOWN) && !input_key_press(E_G_KEY_JUMP))
        {
            if (!playerFlags.picked)
            {   
                //check if player down point is on tile stairs
				if (CHECK_FLAG(collision_get_tile_property(player, E_COLPOINT_CENTER_DOWN), E_TILE_PROP_TOP_STAIR) || CHECK_FLAG(collision_get_tile_property(player, E_COLPOINT_CENTER_DOWN), E_TILE_PROP_STAIR))
                {
					//check if center player point is on tile stairs
					if (CHECK_FLAG(collision_get_tile_property(player, E_COLPOINT_CENTER), E_TILE_PROP_TOP_STAIR) || CHECK_FLAG(collision_get_tile_property(player, E_COLPOINT_CENTER), E_TILE_PROP_STAIR))	
                    {
						//snap player on tile
                        player->fixPos.x = itofix(map_snap_x_to_tile(player));
                        //go down stairs
                        player->fixPos.y += fixmul(ftofix(PLAYER_STAIRS_VEL_Y), ftofix(deltaTime));
                    }
                    //else, we are on the top of stairs
					else
                    {
						//adjust player to start of stair
                        player->fixPos.y += itofix((player->size.y >> 1));
					}
					//reset velocities
					player->fixVel = (tFixVector){0,0};                    
					//set flagas
					playerFlags.onStairs = true;
                    //reset flags
                    playerFlags.jump = false;
                    playerFlags.crouched = false;					
                }
                else
                {
                    playerFlags.crouched = player->ground;
                    playerFlags.onStairs = false;
                }
            }
        }        
        else if (playerFlags.crouched)
        {
            //don't reset crouch if solid tile on upper tile
            if(!map_get_tile_property((tVector){player->pos.x + (!player->dir * player->size.x), player->pos.y - PLAYER_CROUCH_CHECK_Y}) ==  E_TILE_PROP_SOLID)
                playerFlags.crouched = false;
        }

        //Up control: climb stairs
		if (input_key_press(E_G_KEY_UP) && !input_key_press(E_G_KEY_JUMP))	
        {		
			//check if not object picked
			if (!playerFlags.picked)
            {
				//check if center of entity are on tile stairs
                if (CHECK_FLAG(collision_get_tile_property(player, E_COLPOINT_CENTER), E_TILE_PROP_TOP_STAIR) || CHECK_FLAG(collision_get_tile_property(player, E_COLPOINT_CENTER), E_TILE_PROP_STAIR))
				{
					//reset velocities
                    player->fixVel = (tFixVector){0, 0};
					//snap to tile
                    player->fixPos.x = itofix(map_snap_x_to_tile(player));
					//going down stairs
                    player->fixPos.y -= fixmul(ftofix(PLAYER_STAIRS_VEL_Y), ftofix(deltaTime));
					//set flags
					playerFlags.onStairs = true;
					//reset flags
                    playerFlags.jump = false;
                }
                //else, if center down point on top of stair, exits stairs
                else if (CHECK_FLAG(collision_get_tile_property(player, E_COLPOINT_CENTER_DOWN), E_TILE_PROP_TOP_STAIR))
                {
					//position to platform (above tile of the stair)
                    uint8_t mapTileSize = map_get_tile_size();
                    player->fixPos.y = itofix((((entity_center_y(player) / mapTileSize) * mapTileSize) + mapTileSize) - player->size.y);
                    //reset flag
					playerFlags.onStairs = false;
				}				
			}
		}

        //Jump control
        if (input_key_press(E_G_KEY_JUMP))
        {
            if (playerFlags.onStairs)
			{
                //fall from stairs
                playerFlags.onStairs = false;
                playerFlags.jump = true;
                player->ground = false;
            }
            //if not flag jump and not falling
            if (!playerFlags.jump && player->fixVel.y <= 0)
            {
                //if player on ground (rising edge of jump)
                if (player->ground)
                {
                    //play sfx jump
                    sfx_play(playerSfx[SFX_PLAYER_JUMP], E_SFX_PLAYER_VOICE);
                    //reset flags
                    playerFlags.attack = false;
                    player->ground = false;                
                }

                //apply y acceleration
                player->fixVel.y += fixmul(-accelY, ftofix(deltaTime));
                //if reached max jump velocity, set flag
                if (player->fixVel.y < -(maxVelY))
                    playerFlags.jump = true;
            }
        }
        else 
        {
            //if not press jump, set flag jump
            playerFlags.jump = true;
            //reset jump flag on ground
            if(player->ground)                
                playerFlags.jump = false;
        }        
    }

    //Action control (atack, pick)
    if (input_key_down(E_G_KEY_ACTION))
    {
        //pick object if it's not picked
        if (playerFlags.picking && !playerFlags.picked)
        {
            playerFlags.picked = true;                
            //send picking signal to entity object
            objectPickedID = memObjectforPickID;
            entity_get(memObjectforPickID)->signal = E_ENT_SIGNAL_PICKING;            
            memObjectforPickID = 0;
            sfx_play(playerSfx[SFX_PLAYER_PICK], E_SFX_PLAYER_VOICE);   
        }
        //throw object if picked
        else if (playerFlags.picked && objectPickedID)
        {            
            playerFlags.throwing = true;
            //send signal of throw or short throw
            entity_get(objectPickedID)->signal = input_key_press(E_G_KEY_DOWN) ? E_ENT_SIGNAL_SHORT_THROW: E_ENT_SIGNAL_THROW;            
            objectPickedID = 0;
            //reset flags
            playerFlags.picked = false;  
            sfx_play(playerSfx[SFX_PLAYER_THROW], E_SFX_PLAYER_VOICE);        
            
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

static void player_update_size(tEntity *player)
{
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
}

static void player_update_collisions(tEntity *player)
{
    uint8_t colDir = 0;
    uint8_t memColTerrainHDir = 0;
    player->ground = false;
    objectForPickID = 0;
    collision_set_player_platform_id(-1);

    //check all the entity collision points with tilemap     
    for (uint8_t i = 0; i < E_NUM_COL_POINTS; i++)
    {                
        //check collision tile for collision point
        colDir = collision_check_tile(player, i);        
        //apply collision direction
        collision_apply_dir(player, colDir, E_COLLISION_NO_BOUNCE);   
        
        //memorize last collision dir
        if (colDir == E_COLLISION_DIR_LEFT || colDir == E_COLLISION_DIR_RIGHT)
            memColTerrainHDir = colDir;

        //reset jump if head collision
        if (colDir == E_COLLISION_DIR_UP)
            playerFlags.jump = true;
        
        //check tile with hurt property
        if (CHECK_FLAG(collision_get_tile_property(player, i), E_TILE_PROP_HURT))
        {
            if (!playerInvincible && (colDir != E_COLLISION_DIR_NONE || CHECK_FLAG(collision_get_tile_property(player, i), E_TILE_PROP_NO_SOLID)))
                playerFlags.hurt = true;
        }
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
                case E_ENT_CLASS_PLATFORM:
                case E_ENT_CLASS_TRIGGER:
                    //check vertical collision with entity
                    colDir = collision_check_entity(player, checkEntity, E_CHECK_PROCESS_VERTICALAXIS);
                    //if collided
                    if (colDir)
                    {
                        //reset jump if head collision
                        if (colDir == E_COLLISION_DIR_UP)
                            playerFlags.jump = true;

                        //if collision dir down and attacking and object breakable
                        if (colDir == E_COLLISION_DIR_DOWN && !CHECK_FLAG(checkEntity->properties, E_ENT_PROP_NO_BREAKABLE) && playerFlags.attack && checkEntity->signal != E_ENT_SIGNAL_HURT)
                        {
                            //send hurt signal
                            checkEntity->signal = E_ENT_SIGNAL_HURT;
                            //set bounce velocity
                            player->fixVel.y = itofix(PLAYER_ATTACK_BOUNCE_VEL);
                            //play sfx
                            sfx_play(playerSfx[SFX_PLAYER_BOUNCE], E_SFX_PLAYER_VOICE);
                        }
                        else
                            //adjust collision position (object solid)
                            collision_apply_dir(player, colDir, E_COLLISION_NO_BOUNCE);
                    }
                    
                    //if entity is platform and collision on above
                    if (checkEntity->entClass == E_ENT_CLASS_PLATFORM && colDir == E_COLLISION_DIR_DOWN && player->ground)
                    {
                        //sets platform entity id
                        collision_set_player_platform_id(checkEntity->id);
                    }
                        
                    //check horizontal collision with entity
                    colDir = collision_check_entity(player, checkEntity, E_CHECK_PROCESS_HORIZONTALAXIS);

                    //if lateral collision, check if is object pickable and on lower position to the player                    
                    if ((colDir == E_COLLISION_DIR_RIGHT || colDir == E_COLLISION_DIR_LEFT) && !CHECK_FLAG(checkEntity->properties, E_ENT_PROP_NO_PICKABLE) 
                         && !playerFlags.picked && checkEntity->pos.y >= player->pos.y && !playerFlags.throwing)                        
                            objectForPickID = checkEntity->id;      
                    
                    //check player crushed between object and terrain
                    if (colDir == E_COLLISION_DIR_LEFT)
                    {                    
                        if (!CHECK_FLAG(map_get_tile_property((tVector){player->pos.x + (player->size.x >> 1), player->pos.y + (player->size.y / 6)}), E_TILE_PROP_NO_SOLID) 
                         //|| !CHECK_FLAG(map_get_tile_property((tVector){player->pos.x + (player->size.x >> 1), player->pos.y + (player->size.y - (player->size.y / 6))}), E_TILE_PROP_NO_SOLID)
                        )
                            playerFlags.dead = true;
                    }
                    else if (colDir == E_COLLISION_DIR_RIGHT)
                    {
                        if (!CHECK_FLAG(map_get_tile_property((tVector){player->pos.x, player->pos.y + (player->size.y / 6)}), E_TILE_PROP_NO_SOLID) 
                         //|| !CHECK_FLAG(map_get_tile_property((tVector){player->pos.x, player->pos.y + (player->size.y - (player->size.y / 6))}), E_TILE_PROP_NO_SOLID)
                        )
                            playerFlags.dead = true;
                    }

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
                            if ((colDir == E_COLLISION_DIR_DOWN || player->pos.y < checkEntity->pos.y) && playerFlags.attack && checkEntity->signal != E_ENT_SIGNAL_HURT && !CHECK_FLAG(checkEntity->properties, E_ENT_PROP_NO_HURT))
                            {
                                //set bounce velocity
                                player->fixVel.y = itofix(PLAYER_ATTACK_BOUNCE_VEL);
                                if (!CHECK_FLAG(checkEntity->properties, E_ENT_PROP_HURTLESS))
                                {
                                    //send signal to entity
                                    checkEntity->signal = E_ENT_SIGNAL_HURT;                                  
                                    //shake camera when kill enemy
                                    scroll_shake_camera();
                                }
                                else
                                    sfx_play(playerSfx[SFX_PLAYER_BOUNCE], E_SFX_PLAYER_VOICE);                                
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

    //check collision with autoscroll (and dead if collision with horizontal terrain)
    if (scroll_get_scroll_mode() == E_SCROLL_MODE_AUTOSCROLL_X)
    {
        tFixVector scrollPosition = scroll_get_fix_position();    
        //left
        if (collision_check_AABB(player, scrollPosition, (tVector){1, GAME_H }, E_CHECK_PROCESS_HORIZONTALAXIS) && memColTerrainHDir == E_COLLISION_DIR_RIGHT)
            playerFlags.dead = true;
        //right
        if (collision_check_AABB(player, (tFixVector){scrollPosition.x + itofix(GAME_W - 1), scrollPosition.y}, (tVector){1, GAME_H }, E_CHECK_PROCESS_HORIZONTALAXIS) && memColTerrainHDir == E_COLLISION_DIR_LEFT)
            playerFlags.dead = true;
    }
}

static void player_update_state(tEntity *player)
{
    //reset flags
    playerFlags.disableMove = false;
    playeridleCounter =  player->state != ST_PLAYER_IDLE ? 0 : playeridleCounter;
    player->noGravity = playerFlags.onStairs; //TODO: move this
    
    //prevent double picking bug
    if (playerFlags.picked && objectPickedID == 0)
    { 
        playerFlags.picked = false;
        playerFlags.picking = false;
    }

    //picking objects
    if (objectForPickID != 0)
    {
        //check picking counter
        if (pickingCounter >= PLAYER_PICKING_TIME)
        {
            playerFlags.picking = true;
            playerFlags.attack = false;
            memObjectforPickID = objectForPickID;
        }
        else
        {
            pickingCounter += clock_tick_get();
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
    playerInvincible = playerInvincible > 0 ? playerInvincible - clock_tick_get() : 0;
    
    //receive hurt signal by another entity
    if (player->signal == E_ENT_SIGNAL_HURT)    
    {
        if (!playerInvincible)
            playerFlags.hurt = true;

        player->signal = E_ENT_SIGNAL_NONE;
    }

    //set the state (priority order)
    if (playerFlags.dead)
    {
        player->state = ST_PLAYER_DEAD;
        playerFlags.disableMove = true;
        playerInvincible = false;
        player->fixVel.x = 0;
        if (player->prevState != player->state)
            sfx_play(playerSfx[SFX_PLAYER_DEAD], E_SFX_PLAYER_VOICE);
    }
    /*else if (game.time <= 0)
    {
        player->state = ST_PLAYER_TIMEOUT;
        playerFlags.disableMove = true;
        player->fixVel.x = 0;
    }*/
    #ifdef DEBUGMODE
    else if (playerFlags.hurt && debugOptions.invencible)
        playerFlags.hurt = false;
    #endif
    else if (playerFlags.hurt)
    {
        playerFlags.disableMove = true;
        player->state = ST_PLAYER_HURT;   
        playerInvincible = PLAYER_INVINCIBLE_TIME; 
        
        //rising edge of the state
        if (player->prevState != player->state)
        {
            //play hurt sfx
            sfx_play(playerSfx[SFX_PLAYER_HURT], E_SFX_PLAYER_VOICE);                                                                          
            //lose life
            game.life -= 1;
            
            //reset ground
            player->ground = false;
            
            //fall on stairs on hurt
			if (playerFlags.onStairs)
            {
				playerFlags.onStairs = false;
			}
            else
            {
                //set hurt velocities
                player->fixVel.y = itofix(PLAYER_HURT_VEL_Y);
                player->fixVel.x = player->dir == E_ENT_DIR_RIGHT ? itofix(-PLAYER_HURT_VEL_X) : itofix(PLAYER_HURT_VEL_X);
            }

            //if object picked, we lose it
            if (playerFlags.picked)
            {                                    
                entity_get(objectPickedID)->signal = E_ENT_SIGNAL_THROW;                
                objectPickedID = 0;
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
    else if (playerFlags.onStairs)
    {
		if (input_key_press(E_G_KEY_DOWN) || input_key_press(E_G_KEY_UP))
            player->state = ST_PLAYER_MOVING_ON_STAIRS;
        else
            player->state = ST_PLAYER_ON_STAIRS;        
    }
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
            {
                if (player->prevState == ST_PLAYER_RUN && playerFlags.crouched)
                    play_animation(&player->anim, ANIM_PLY_CROUCH);
                else
                    play_animation(&player->anim, ANIM_PLY_GET_CROUCH);            
            }
            else if (playeridleCounter >= PLAYER_IDLE_WAIT_TIME)
            {
                if (play_animation(&player->anim, ANIM_PLY_IDLE_WAIT))
                    playeridleCounter = 0;    
            }
            else
            {
                //check if on edge (xor)
                if ((CHECK_FLAG(collision_get_tile_property(player, E_COLPOINT_DOWN_R), E_TILE_PROP_NO_SOLID) ^
                    CHECK_FLAG(collision_get_tile_property(player, E_COLPOINT_DOWN_L), E_TILE_PROP_NO_SOLID)) 
                    && collision_get_player_platform_id() == -1)
                    play_animation(&player->anim, ANIM_PLY_ON_EDGE);
                else
                {
                    play_animation(&player->anim, ANIM_PLY_BREATH);  
                    playeridleCounter += clock_tick_1sec_get();          
                }
            }
        break;
        case ST_PLAYER_RUN:
            if (playerFlags.picked)
                play_animation(&player->anim, ANIM_PLY_RUN_PICKED);
            else if (playerFlags.crouched)
                play_animation(&player->anim, ANIM_PLY_WALK_CROUCH);
            else    
                play_animation(&player->anim, ANIM_PLY_RUN);                
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
            play_animation(&player->anim, ANIM_PLY_GET_CROUCH);
        break;
        case ST_PLAYER_ATTACK:
            play_animation(&player->anim, ANIM_PLY_JUMP_ATTACK);
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
        case ST_PLAYER_ON_STAIRS:
            play_animation(&player->anim, ANIM_PLY_ON_STAIRS);
        break;
        case ST_PLAYER_MOVING_ON_STAIRS:
            play_animation(&player->anim, ANIM_PLY_MOVE_STAIRS);
            if (player->anim.frame != player->anim.lastFrame)
                sfx_play(playerSfx[SFX_PLAYER_STAIR], E_SFX_PLAYER_VOICE);
        break;        
        case ST_PLAYER_TIMEOUT:
            if (player->ground)
            {
                play_animation(&player->anim, ANIM_PLY_TIMEOUT);                    
            }
        break;
    }

    //blink
    if (playerInvincible) 
        entity_blink(player);
    else
        player->visible = true;
}

#ifdef DEBUGMODE
static void player_trace(tEntity *player)
{
    show_debug( "p.X: %i,p.Y: %i", player->pos.x, player->pos.y);
    show_debug( "p.fX: %f,p.fY: %f", fixtof(player->fixPos.x), fixtof(player->fixPos.y));
    show_debug( "p.vX: %f,p.vY: %f", fixtof(player->fixVel.x),fixtof(player->fixVel.y));
    //show_debug( "p.iniX: %i,p.iniY: %i", player->initPos.x, player->initPos.y);
    //show_debug( "crouched: %i", playerFlags.crouched);     
    //show_debug("Property: %i", map_get_tile_property((tVector){1442,71}));
    //show_debug("Test: %i", CHECK_FLAG(map_get_tile_property((tVector){1442,71}), 256));
    //show_debug("for pick %i picked %i", objectForPickID, objectPickedID);
    //show_debug("Init pos %i,%i", player->initPos.x, player->initPos.y);
}
#endif